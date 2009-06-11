/*
 * Copyright (C) 2008, 2009 Nokia Corporation.
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

#include <QObject>
#include <QString>
#include <QDBusConnection>

class QDBusPendingCallWatcher;
class QDBusInterface;

namespace ContextSubscriber {

class ManagerInterface : public QObject
{
    Q_OBJECT

    static const QString interfaceName;
    static const QString objectPath;

public:
    ManagerInterface(const QDBusConnection::BusType busType, const QString &busName, QObject *parent = 0);

    void getSubscriber();
    bool isGetSubscriberFailed() const;

signals:
    void getSubscriberFinished(QString objectPath);

private slots:
    void onGetSubscriberFinished(QDBusPendingCallWatcher* watcher);

private:
    QDBusInterface* iface; //< DBus interface for the manager
    bool getSubscriberFailed; //< Previous async GetSubscriber call failed
};

} // end namespace

#endif
