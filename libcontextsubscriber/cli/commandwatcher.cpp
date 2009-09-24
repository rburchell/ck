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
#include "contextproperty.h"
#include "propertylistener.h"
#include <QTextStream>
#include <QFile>
#include <QSocketNotifier>
#include <QStringList>
#include <QCoreApplication>
#include "contextpropertyinfo.h"
#include <QDebug>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <QMap>

CommandWatcher::CommandWatcher(int commandfd, QMap<QString, ContextProperty*> *properties, QObject *parent) :
    QObject(parent), commandfd(commandfd), properties(properties)
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
        qDebug() << "  new KEY...                      - create context properties";
        qDebug() << "  delete KEY...                   - delete context properties";
        qDebug() << "  subscribe KEY...                - subscribe to keys";
        qDebug() << "  waitforsubscription KEY...      - subscribe to keys";
        qDebug() << "  unsubscribe KEY...              - unsubscribe from keys";
        qDebug() << "  value KEY [DEF]                 - get value for a key";
        qDebug() << "  key KEY                         - get the key for a key (rather useless)";
        qDebug() << "  ikey KEY                        - get the info()->key for a key (rather useless)";
        qDebug() << "  man KEY                         - get the info()->doc for a key";
        qDebug() << "  type KEY                        - get the info()->type for a key";
        qDebug() << "  plugin KEY                      - get the info()->plugin for a key";
        qDebug() << "  constructionstring KEY          - get the info()->constructionstring for a key";
        qDebug() << "  flush                           - write FLUSHED to stderr and stdout";
        qDebug() << "Any prefix of a command can be used as an abbreviation";
}

void CommandWatcher::interpret(const QString& command) const
{
    QTextStream out(stdout);
    QTextStream err(stderr);
    if (command == "") {
        help();
    } else {
        QStringList args = command.split(" ");
        QString commandName = args[0];
        args.pop_front();

        if (args.size() == 0 && !QString("flush").startsWith(commandName)) {
            help();
            return;
        }

        if (QString("new").startsWith(commandName)) {
            foreach (QString key, args)
                if (properties->contains(key))
                    qDebug() << "key already exists: " << key;
                else {
                    properties->insert(key, new ContextProperty(key, QCoreApplication::instance()));
                    new PropertyListener(properties->value(key));
                }
        } else if (QString("delete").startsWith(commandName)) {
            foreach (QString key, args)
                if (properties->contains(key))
                    delete properties->take(key);
                else
                    qDebug() << "no such key:" << key;
        } else if (QString("subscribe").startsWith(commandName)) {
            foreach (QString key, args)
                if (properties->contains(key))
                    properties->value(key)->subscribe();
                else
                    qDebug() << "no such key:" << key;
        } else if (QString("waitforsubscription").startsWith(commandName)) {
            foreach (QString key, args)
                if (properties->contains(key)) {
                    properties->value(key)->waitForSubscription();
                    out << "wait finished for " << key << endl;
                } else
                    qDebug() << "no such key:" << key;
        } else if (QString("unsubscribe").startsWith(commandName)) {
            foreach (QString key, args)
                if (properties->contains(key))
                    properties->value(key)->unsubscribe();
                else
                    qDebug() << "no such key:" << key;
        } else if (QString("value").startsWith(commandName)) {
            QString key = args[0];
            if (properties->contains(key)) {
                QVariant value;
                if (args.size() > 1) {
                    value = properties->value(key)->value(args[1]);
                } else {
                    value = properties->value(key)->value();
                }
                if (value.isNull())
                    out << "value is Unknown" << endl;
                else
                    out << "value: " << value.typeName() << ":" << value.toString() << endl;
            } else
                qDebug() << "no such key:" << key;
        } else if (QString("key").startsWith(commandName)) {
            QString key = args[0];
            if (properties->contains(key))
                out << "key: " << properties->value(key)->key() << endl;
            else
                qDebug() << "no such key:" << key;
        } else if (QString("ikey").startsWith(commandName)) {
            QString key = args[0];
            if (properties->contains(key))
                out << "ikey: " << properties->value(key)->info()->key() << endl;
            else
                qDebug() << "no such key:" << key;
        } else if (QString("man").startsWith(commandName)) {
            QString key = args[0];
            if (properties->contains(key))
                out << "man: " << properties->value(key)->info()->doc() << endl;
            else
                qDebug() << "no such key:" << key;
        } else if (QString("type").startsWith(commandName)) {
            QString key = args[0];
            if (properties->contains(key))
                out << "type: " << properties->value(key)->info()->type() << endl;
            else
                qDebug() << "no such key:" << key;
        } else if (QString("plugin").startsWith(commandName)) {
            QString key = args[0];
            if (properties->contains(key))
                out << "plugin: " << properties->value(key)->info()->plugin() << endl;
            else
                qDebug() << "no such key:" << key;
        } else if (QString("constructionstring").startsWith(commandName)) {
            QString key = args[0];
            if (properties->contains(key))
                out << "constructionstring: " << properties->value(key)->info()->constructionString() << endl;
            else
                qDebug() << "no such key:" << key;
        } else if (QString("flush").startsWith(commandName)) {
            out << "FLUSHED" << endl;
            out.flush();
            err << "FLUSHED" << endl;
            err.flush();
        } else
            help();
    }
}
