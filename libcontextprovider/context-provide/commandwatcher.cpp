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

CommandWatcher::CommandWatcher(int commandfd, QObject *parent) :
    QObject(parent), commandfd(commandfd)
{
    commandNotifier = new QSocketNotifier(commandfd, QSocketNotifier::Read, this);
    sconnect(commandNotifier, SIGNAL(activated(int)), this, SLOT(onActivated()));
    help();
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
    qDebug() << "Available commands:";
    qDebug() << "  add KEY TYPE                    - create new key with the given type";
    qDebug() << "  KEY=VALUE                       - set KEY to the given VALUE";
    qDebug() << "  sleep INTERVAL                  - sleep the INTERVAL amount of seconds";
    qDebug() << "  flush                           - write FLUSHED to stderr and stdout";
    qDebug() << "Any prefix of a command can be used as an abbreviation";
}

void CommandWatcher::interpret(const QString& command)
{
    QTextStream out(stdout);
    QTextStream err(stderr);
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
        } else
            help();
   }
}

void CommandWatcher::addCommand(const QStringList& args)
{
    if (args.count() < 2) {
        qDebug() << "ERROR: need to specify both KEY and TYPE";
        return;
    }

    const QString keyName = args.at(0);
    const QString keyType = args.at(1);

    if (keyType != "integer" && keyType != "string" &&
        keyType != "double") {
        qDebug() << "Unknown type";
        return;
    }

    types.insert(keyName, keyType);
    properties.insert(keyName, new Property(keyName));

    qDebug() << "Added key:" << keyName << "with type:" << keyType;
}

void CommandWatcher::sleepCommand(const QStringList& args)
{
    if (args.count() < 1) {
        qDebug() << "ERROR: need to specify sleep INTERVAL";
        return;
    }

    int interval = args.at(0).toInt();
    qDebug() << "Sleeping" << interval;
    sleep(interval);
}

void CommandWatcher::setCommand(const QString& command)
{
    QStringList parts = command.split("=");

    const QString keyName = parts.at(0).trimmed();
    const QString value = parts.at(1).trimmed();

    if (! types.contains(keyName)) {
        qDebug() << "ERROR: key" << keyName << "not known/added";
        return;
    }

    Property *prop = properties.value(keyName);
    const QString keyType = types.value(keyName);
    QVariant v;

    if (keyType == "integer")
        v = QVariant(value.toInt());
    else if (keyType == "string")
        v = QVariant(value);
    else if (keyType == "double")
        v = QVariant(value.toDouble());

    qDebug() << "Setting key:" << keyName << "to value:" << v << QString("(" + keyType + ")");
    prop->setValue(v);
}
