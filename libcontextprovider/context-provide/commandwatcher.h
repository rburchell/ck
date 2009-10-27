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

#ifndef COMMANDWATCHER_H
#define COMMANDWATCHER_H

#include <QObject>
#include <QTextStream>
#include "property.h"

using namespace ContextProvider;

class QFile;
class QSocketNotifier;
class QString;
class PropertyProxy;
class ContextRegistryInfo;
template <typename K, typename V> class QMap;

class CommandWatcher : public QObject
{
    Q_OBJECT

public:
    CommandWatcher(QString busName, QDBusConnection::BusType busType, int commandfd, QObject *parent = 0);
    ~CommandWatcher();
    void addCommand(const QStringList& args);
    static const QString commanderBusName;

private:
    void interpret(const QString& command);
    void help();
    void unsetCommand(const QStringList& args);
    void setCommand(const QString& command);
    void sleepCommand(const QStringList& args);
    void flushCommand();
    void dumpCommand(const QStringList& args);
    void delCommand(const QStringList& args);
    void infoCommand(const QStringList& args);
    void listCommand();
    void startCommand();
    QString unquote(const QString& str);

    int commandfd;
    QSocketNotifier *commandNotifier;
    QMap <QString, QString> types;          // key -> type
    QMap <QString, Property*> properties;   // property index
    QMap <QString, PropertyProxy*> proxies;
    ContextRegistryInfo *registryInfo;

    QTextStream out;
    QString busName;
    QDBusConnection::BusType busType;
    bool started;

private slots:
    void onActivated();
    void onRegistryChanged();
};

#endif
