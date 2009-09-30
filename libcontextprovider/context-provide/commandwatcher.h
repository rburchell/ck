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
template <typename K, typename V> class QMap;

class CommandWatcher : public QObject
{
    Q_OBJECT

public:
    CommandWatcher(QString busName, QDBusConnection::BusType busType, int commandfd, QObject *parent = 0);
    ~CommandWatcher();
    void addCommand(const QStringList& args);

private:
    int commandfd;
    QSocketNotifier *commandNotifier;
    void interpret(const QString& command);
    QMap <QString, QString> types;          // key -> type
    QMap <QString, Property*> properties;   // property index
    void help();
    QString unquote(const QString& str);
    void unsetCommand(const QStringList& args);
    void setCommand(const QString& command);
    void sleepCommand(const QStringList& args);
    void flushCommand();
    void dumpCommand();
    void startCommand();
    QTextStream out;
    bool silent;
    QString busName;
    QDBusConnection::BusType busType;

private slots:
    void onActivated();
};

#endif
