/*
 * Copyright (C) 2009 Nokia Corporation.
 *
 * Contact: Marius Vollmer <marius.vollmer@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include "commandwatcher.h"
#include "sconnect.h"
#include <service.h>
#include <QTextStream>
#include <QFile>
#include <QSocketNotifier>
#include <QStringList>
#include <QCoreApplication>
#include <QDebug>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <QMap>
#include <QDir>

CommandWatcher::CommandWatcher(QString bn, QDBusConnection::BusType bt, int commandfd, QObject *parent) :
    QObject(parent), commandfd(commandfd), out(stdout), busName(bn), busType(bt)
{
    commandNotifier = new QSocketNotifier(commandfd, QSocketNotifier::Read, this);
    sconnect(commandNotifier, SIGNAL(activated(int)), this, SLOT(onActivated()));
}

CommandWatcher::~CommandWatcher()
{
    foreach(Property* p, properties) {
        delete p;
    }
}

void CommandWatcher::onActivated()
{
    // read all available input to commandBuffer
    static QByteArray commandBuffer = "";
    static char buf[1024];
    int readSize;
    fcntl(commandfd, F_SETFL, O_NONBLOCK);
    while ((readSize = read(commandfd, &buf, 1024)) > 0)
        commandBuffer += QByteArray(buf, readSize);

    // handle all available whole input lines as commands
    int nextSeparator;
    while ((nextSeparator = commandBuffer.indexOf('\n')) != -1) {
        // split lines to separate commands by semicolons
        QStringList commands = QString::fromUtf8(commandBuffer.constData()).left(nextSeparator).split(";");
        foreach (QString command, commands)
            interpret(command.trimmed());
        commandBuffer.remove(0, nextSeparator + 1);
    }

    if (readSize == 0) // EOF
        QCoreApplication::exit(0);
}

void CommandWatcher::help()
{
    qDebug() << "Available commands:\n";
    qDebug() << "  add TYPE KEY VALUE              - create new key with the given type";
    qDebug() << "  KEY=VALUE                       - set KEY to the given VALUE";
    qDebug() << "  sleep INTERVAL                  - sleep the INTERVAL amount of seconds";
    qDebug() << "  dump                            - dump the xml content of the defined props";
    qDebug() << "  start                           - (re)register everything on D-Bus";
    qDebug() << "  unset KEY                       - sets KEY to unknown";
    qDebug() << "  exit                            - quit this program";
    qDebug() << "Any unique prefix of a command can be used as an abbreviation";
}

void CommandWatcher::interpret(const QString& command)
{
    if (command == "") {
        // Show help
        help();
    } else if (command.contains('=')) {
        // Setter command
        setCommand(command);
    } else {
        QStringList args = command.split(" ");
        QString commandName = args[0];
        args.pop_front();

        // Interpret commands
        if (QString("add").startsWith(commandName)) {
            addCommand(args);
        } else if (QString("sleep").startsWith(commandName)) {
            sleepCommand(args);
        } else if (QString("exit").startsWith(commandName)) {
            exit(0);
        } else if (QString("dump").startsWith(commandName)) {
            dumpCommand();
        } else if (QString("start").startsWith(commandName)) {
            startCommand();
        } else if (QString("unset").startsWith(commandName)) {
            unsetCommand(args);
        } else
            help();
    }
    out.flush();
}

QString CommandWatcher::unquote(const QString& str)
{
    QString m = str;
    if (m.startsWith('"'))
        m = m.right(m.length() - 1);

    if (m.endsWith('"'))
        m = m.left(m.length() - 1);

    return m;
}

void CommandWatcher::addCommand(const QStringList& args)
{
    if (args.count() < 2) {
        out << "ERROR: need to specify both KEY and TYPE\n";
        return;
    }

    QString keyType = unquote(args.at(0)).toUpper();
    const QString keyName = unquote(args.at(1));

    if (keyType != "INT" && keyType != "STRING" &&
        keyType != "DOUBLE" && keyType != "TRUTH" && keyType != "BOOL") {
        out << "ERROR: Unknown type (has to be: INT, STRING, DOUBLE or TRUTH)\n";
        return;
    }
    if (keyType == "BOOL") keyType = "TRUTH";

    if (properties.contains(keyName)) {
        qDebug() << "Already existing key, changing the type and value of it";
        types[keyName] = keyType;
    } else {
        types.insert(keyName, keyType);
        properties.insert(keyName, new Property(keyName));
        qDebug() << "Added key: " << keyName << " with type: " << keyType << "\n";
    }

    // handle default value
    if (args.count() > 2)
        setCommand(keyName + "=\"" + QStringList(args.mid(2)).join(" ") + "\"");
}

void CommandWatcher::sleepCommand(const QStringList& args)
{
    if (args.count() < 1) {
        out << "ERROR: need to specify sleep INTERVAL\n";
        return;
    }

    int interval = unquote(args.at(0)).toInt();
    out << "Sleeping " << interval << " seconds" << "\n";
    sleep(interval);
}

void CommandWatcher::dumpCommand()
{
    QString fileName;
    QString dirName = "./";

    if (getenv("CONTEXT_PROVIDE_REGISTRY_FILE"))
        fileName = QString(getenv("CONTEXT_PROVIDE_REGISTRY_FILE"));
    else
        fileName = QString("./context-provide.context");

    if (fileName.indexOf('/') != -1)
        dirName = QString(fileName.left(fileName.lastIndexOf('/'))) + "/";

    QDir dir(dirName);
    QString tmpPath = dir.absoluteFilePath("temp-XXXXXX");
    QByteArray templ = tmpPath.toUtf8();
    char *tmpPathChars = (char*) malloc(strlen(templ.data())+1);
    if (tmpPathChars  == NULL)
        qFatal("Not enough memory");
    strcpy(tmpPathChars, templ.data());
    int fd = mkstemp(tmpPathChars);
    QFile f;
    f.open(fd, QIODevice::WriteOnly);
    QTextStream xml(&f);

    QString bType = (busType == QDBusConnection::SystemBus) ? "system" : "session";
    xml << "<?xml version=\"1.0\"?>\n";
    xml << QString("<provider bus=\"%1\" service=\"%2\">\n").arg(bType).arg(busName);

    foreach(QString key, properties.keys()) {
        xml << QString("  <key name=\"%1\">\n").arg(key);
        xml << QString("    <type>%1</type>\n").arg(types.value(key));
        xml << QString("    <doc>A phony but very flexible property.</doc>\n");
        xml << QString("  </key>\n");
    }
    xml << "</provider>\n";

    f.close();
    close(fd);

    // Atomically rename
    rename(tmpPathChars, fileName.toUtf8().constData());
    free(tmpPathChars);

    out << "Wrote " << fileName << "\n";

}

void CommandWatcher::setCommand(const QString& command)
{
    const QString keyName = unquote(command.left(command.indexOf('=')).trimmed());
    const QString value = unquote(command.mid(command.indexOf('=')+1).trimmed());

    if (! types.contains(keyName)) {
        out << "ERROR: key " << keyName << " not known/added\n";
        return;
    }

    Property *prop = properties.value(keyName);
    const QString keyType = types.value(keyName);
    QVariant v;

    if (keyType == "INT")
        v = QVariant(value.toInt());
    else if (keyType == "STRING")
        v = QVariant(value);
    else if (keyType == "DOUBLE")
        v = QVariant(value.toDouble());
    else if (keyType == "TRUTH") {
        if (value == "True" || value == "true" || value == "1")
            v = QVariant(true);
        else
            v = QVariant(false);
    }

    out << "Setting key: " << keyName << " to value: " << v.toString() << "\n";
    prop->setValue(v);
}

void CommandWatcher::unsetCommand(const QStringList& args)
{
    if (args.count() < 1) {
        out << "ERROR: need to specify key to unset\n";
        return;
    }

    QString keyName = unquote(args[0].trimmed());

    if (! types.contains(keyName)) {
        out << "ERROR: key " << keyName << " not known/added\n";
        return;
    }

    Property *prop = properties.value(keyName);
    out << "Setting key: " << keyName << " to unknown\n";
    prop->unsetValue();
}

void CommandWatcher::startCommand()
{
    Service service(busType, busName);
    service.start();
    // FIXME: exit here if the registration is unsuccessful
    out << "Service started\n";
}
