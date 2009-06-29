/*
 * Copyright (C) 2008 Nokia Corporation.
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

#ifndef MANAGERINTERFACE_H
#define MANAGERINTERFACE_H

// This is a mock implementation

#include <QObject>
#include <QDBusConnection>

namespace ContextSubscriber {

class ManagerInterface : public QObject
{
    Q_OBJECT


public:
    ManagerInterface(const QDBusConnection connection, const QString &busName, QObject *parent = 0);

    void getSubscriber();
    bool isGetSubscriberFailed() const;

signals:
    void getSubscriberFinished(QString objectPath);

public:
    // Logging
    static int getSubscriberCount;
    static int creationCount;
    static QStringList creationBusNames;

    // For tests
    static void resetLogs();
    bool getSubscriberFailed;

    friend class PropertyProviderUnitTests;

};

} // end namespace

#endif
