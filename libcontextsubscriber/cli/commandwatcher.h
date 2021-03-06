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
class QFile;
class QTextStream;
class QSocketNotifier;
class ContextProperty;
class QString;
class PropertyListener;
template <typename K, typename V> class QMap;

class CommandWatcher : public QObject
{
    Q_OBJECT
public:
    CommandWatcher(int commandfd, QMap<QString, PropertyListener*> *properties, QObject *parent = 0);
private:
    int commandfd;
    QSocketNotifier *commandNotifier;
    void interpret(const QString& command) const;
    QMap<QString, PropertyListener*> *properties;
    static void help();

private Q_SLOTS:
    void onActivated();
};
#endif
