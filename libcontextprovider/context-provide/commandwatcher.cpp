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
#include "propertyproxy.h"
#include "sconnect.h"
#include <contextpropertyinfo.h>
#include <contextregistryinfo.h>
#include <contexttyperegistryinfo.h>
#include <nanoxml.h>
#include <contexttypeinfo.h>
#include <service.h>

#include <QByteArray>
#include <QBuffer>
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
#include <QSet>
#include <qjson/parser.h>

const QString CommandWatcher::commanderBusName = "org.freedesktop.ContextKit.Commander";

CommandWatcher::CommandWatcher(QString bn, QDBusConnection::BusType bt, int commandfd, QObject *parent) :
    QObject(parent), commandfd(commandfd),
    registryInfo(ContextRegistryInfo::instance()),
    out(stdout), busName(bn), busType(bt), started(false)
{
    commandNotifier = new QSocketNotifier(commandfd, QSocketNotifier::Read, this);
    sconnect(commandNotifier, SIGNAL(activated(int)), this, SLOT(onActivated()));
    if (busName == commanderBusName) {
        ContextProperty::ignoreCommander();
        ContextProperty::setTypeCheck(true);
        sconnect(registryInfo, SIGNAL(changed()), this, SLOT(onRegistryChanged()));
        onRegistryChanged();
    }
}

CommandWatcher::~CommandWatcher()
{
    foreach(Property *p, properties)
        delete p;
    foreach(PropertyProxy *p, proxies)
        delete p;
}

void CommandWatcher::onRegistryChanged()
{
    foreach (PropertyProxy *p, proxies)
        delete p;
    proxies.clear();
    // (Re)create the proxies and restore the user's will of overriding.
    foreach (QString key, registryInfo->listKeys()) {
        if (ContextPropertyInfo(key).provided()) {
            qDebug() << "creating proxy for" << key;
            proxies.insert(key, new PropertyProxy(key, !properties.contains(key),
                                                  this));
        }
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
    qDebug() << "  add TYPE KEY [VALUE]            - create new key with the given type";
    qDebug() << "  settype KEY TYPE                - set the type of KEY";
    qDebug() << "  KEY=VALUE                       - set KEY to the given VALUE";
    qDebug() << "  info KEY                        - prints the real (proxied) value of KEY";
    qDebug() << "  unset KEY                       - sets KEY to unknown";
    qDebug() << "  del KEY                         - delete KEY, useful if the tool is commander";
    qDebug() << "  list                            - same as calling info for all known keys";
    qDebug() << "  sleep INTERVAL                  - sleep the INTERVAL amount of seconds";
    qDebug() << "  dump [FILENAME]                 - dump the xml content of the defined props";
    qDebug() << "  restart                         - reregister everything on D-Bus";
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
        } else if (QString("settype").startsWith(commandName)) {
            setTypeCommand(args);
        } else if (QString("del").startsWith(commandName)) {
            delCommand(args);
        } else if (QString("info").startsWith(commandName)) {
             infoCommand(args);
        } else if (QString("list").startsWith(commandName)) {
             listCommand();
        } else if (QString("sleep").startsWith(commandName)) {
            sleepCommand(args);
        } else if (QString("exit").startsWith(commandName)) {
            exit(0);
        } else if (QString("dump").startsWith(commandName)) {
            dumpCommand(args);
        } else if (QString("restart").startsWith(commandName)) {
            restartCommand();
        } else if (QString("unset").startsWith(commandName)) {
            unsetCommand(args);
        } else
            help();
    }
    out.flush();
}

void CommandWatcher::addCommand(const QStringList& args)
{
    if (args.count() < 2) {
        qDebug() << "ERROR: need to specify both KEY and TYPE";
        return;
    }

    QString keyType = args.at(0);
    const QString &keyName = args.at(1);

    // Because of the lameness of the command parser, you cannot specify
    // arbitrary NanoXML fragments to describe a full type instance here.
    // Give something parameterless (like "value") then later change the type
    // with `settype'.

    keyType = keyType.toLower();
    // Legacy types that don't exist in core.types (INT, DOUBLE, TRUTH) are
    // converted to the corresponding new type.
    bool legacy = (keyType == "int" ||
                   keyType == "double" ||
                   keyType == "truth");
    if (keyType == "truth") keyType = "bool";
    if (keyType == "int") keyType = "integer";
    if (keyType == "double") keyType = "number";

    // The type, if not a legacy one, must exist in the type registry.
    if (!legacy) {
        AssocTree typeDef =
            ContextTypeRegistryInfo::instance()->typeDefinitionForName(keyType);
        if (typeDef.isNull()) {
            qDebug() << "type" << keyType << "doesn't exist in the type registry";
            return;
        }
    }
    // Assign a "parameterless instance" of keyType as the type of keyName.
    types.insert(keyName, ContextTypeInfo(QVariant(keyType)));

    if (properties.contains(keyName)) {
        qDebug() << "Already existing key, changing the type and value of it";
    } else {
        properties.insert(keyName, new Property(keyName));
        if (busName == commanderBusName && proxies.contains(keyName))
            proxies[keyName]->enable(false);
        out << "Added key: " << keyName << " with type: " << keyType << endl;
        out.flush();
    }

    // handle default value
    if (args.count() > 2)
        setCommand(keyName + "=" + QStringList(args.mid(2)).join(" "));
    else
        unsetCommand(QStringList(keyName));

    // If service is already started then it has to be restarted after
    // a property is added.  In commander mode if the property is
    // already provided with a proxy, then no restart is necessary.
    if (started && (busName != commanderBusName || !proxies.contains(keyName)))
        restartCommand();
}

void CommandWatcher::setTypeCommand(QStringList &args)
{
    if (args.size() < 2) {
        qDebug() << "ERROR: need to specify KEY and TYPE";
        return;
    }
    QString key = args.takeFirst();
    if (!properties.contains(key)) {
        qDebug() << "unknown key" << key;
        return;
    }
    QByteArray tba = args.join(" ").trimmed().toLocal8Bit();
    // Allow barewords, treating them as parameterless type.
    if (!tba.startsWith('<'))
        tba.prepend('<').append("/>");
    QBuffer typefrag(&tba);
    ContextTypeInfo typeInfo = NanoXml(&typefrag).result();
    if (typeInfo.definition().isNull()) {
        qDebug() << typeInfo.name() << "doesn't exist in the type registry";
        return;
    }
    types.insert(key, typeInfo);

    // If service is already started then it has to be restarted after
    // a property is added.  In commander mode if the property is
    // already provided with a proxy, then no restart is necessary.
    if (started && (busName != commanderBusName || !proxies.contains(key)))
        restartCommand();
}

void CommandWatcher::delCommand(const QStringList &args)
{
    if (args.count() < 1) {
        qDebug() << "ERROR: need to specify KEY";
        return;
    }

    const QString keyName = args.at(0);

    types.remove(keyName);
    delete properties.take(keyName);
    if (busName == commanderBusName && proxies.contains(keyName))
        proxies[keyName]->enable(true);
    out << "Deleted key: " << keyName << endl;
    out.flush();
}

void CommandWatcher::listCommand()
{
    foreach (QString key,
             QSet<QString>::fromList(properties.keys()) +
             QSet<QString>::fromList(proxies.keys()))
        infoCommand(QStringList(key));
}

void CommandWatcher::infoCommand(const QStringList &args)
{
    if (args.count() < 1) {
        qDebug() << "ERROR: need to specify KEY";
        return;
    }
    QString keyName = args.at(0);
    if (properties.contains(keyName)) {
        out << types[keyName].name() << " " << keyName;
        if (properties[keyName]->value().isNull())
            out << " is Unknown" << endl;
        else
            out << " = " << properties[keyName]->value().toString() << endl;
    } else if (busName != commanderBusName) {
        qDebug() << "ERROR:" << keyName << "not added";
    }
    if (busName == commanderBusName) {
        if (!proxies.contains(keyName)) {
            if (!properties.contains(keyName))
                qDebug() << keyName << "is not known";
            return;
        }
        out << "real: " << proxies[keyName]->type() << " "
            << keyName;
        if (proxies[keyName]->realValue().isNull())
            out << " is Unknown" << endl;
        else
            out << " = " << proxies[keyName]->realValue().toString() << endl;
    }
}

void CommandWatcher::sleepCommand(const QStringList& args)
{
    if (args.count() < 1) {
        qDebug() << "ERROR: need to specify sleep INTERVAL";
        return;
    }

    int interval = args.at(0).toInt();
    out << "Sleeping " << interval << " seconds" << endl;
    out.flush();
    sleep(interval);
}

void CommandWatcher::dumpCommand(const QStringList &args)
{
    QString fileName;
    QString dirName;

    if (args.size() > 0)
        fileName = args[0];
    else
        fileName = QString("context-provide.context");
    if (!fileName.startsWith("/"))
        fileName = QString("./") + fileName;

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
        xml <<         "    <type>\n";
        xml << types[key].dumpXML(3);
        xml <<         "    </type>\n";
        xml << QString("    <doc>A phony but very flexible property.</doc>\n");
        xml << QString("  </key>\n");
    }
    xml << "</provider>\n";

    f.close();
    close(fd);

    // Atomically rename
    rename(tmpPathChars, fileName.toUtf8().constData());
    free(tmpPathChars);

    out << "Wrote " << fileName << endl;
    out.flush();
}

void CommandWatcher::setCommand(const QString& command)
{
    const QString keyName = command.left(command.indexOf('=')).trimmed();
    QString value = command.mid(command.indexOf('=')+1).trimmed();

    if (!properties.contains(keyName)) {
        qDebug() << "ERROR: key" << keyName << "not known/added";
        return;
    }

    Property *prop = properties[keyName];
    QString keyType = types[keyName].name();

    // Backward compatibility hacks:
    // ... for people using unquoted strings.
    if (keyType == "string" && !value.startsWith("\""))
        value.prepend('"').append('"');
    // ... for people using 1, true or True as booleans.
    if (keyType == "bool")
        value = (value.toLower() == "true" || value == "1") ? "true" : "false";

    // Now all input is treated as JSon.
    QJson::Parser parser;
    bool ok;
    QVariant v = parser.parse(value.toUtf8(), &ok);
    if (!ok) {
        qDebug() << "An error occurred during parsing";
        return;
    }

    QString vstr;
    QDebug(&vstr) << v; 
    out << "Setting key: " << keyName << " to value: " << vstr << endl;
    out.flush();
    prop->setValue(v);
}

void CommandWatcher::unsetCommand(const QStringList& args)
{
    if (args.count() < 1) {
        qDebug() << "ERROR: need to specify key to unset";
        return;
    }

    QString keyName = args[0].trimmed();

    if (!properties.contains(keyName)) {
        qDebug() << "ERROR: key" << keyName << "not known/added";
        return;
    }

    Property *prop = properties.value(keyName);
    out << "Setting key: " << keyName << " to unknown" << endl;
    out.flush();
    prop->unsetValue();
}

void CommandWatcher::restartCommand()
{
    Service service(busType, busName);
    service.stop(); // this is harmless if we are not started yet, but useful if we are
    if (!service.start()) {
        qDebug() << "Starting service failed, no D-Bus or the service name is already taken";
        exit(2);
    }
    out << "Service started" << endl;
    out.flush();
    started = true;
}
