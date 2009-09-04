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

#ifndef DBUSNAMELISTENER_H
#define DBUSNAMELISTENER_H

#include <QObject>
#include <QString>
#include <QDBusConnection>

class QDBusPendingCallWatcher;

namespace ContextSubscriber {


class DBusNameListener : public QObject
{
    Q_OBJECT
public:
    enum ServicePresence {NotPresent = 0, Present, Unknown};

    explicit DBusNameListener(QDBusConnection::BusType busType, const QString &busName, QObject *parent = 0);
    explicit DBusNameListener(const QDBusConnection bus, const QString &busName, QObject *parent = 0);
    virtual ~DBusNameListener();

    void startListening(bool nameHasOwnerCheck);

    ServicePresence isServicePresent() const;

private slots:
    void onServiceOwnerChanged(const QString &name, const QString &oldOwner, const QString &newOwner);
    void onNameHasOwnerFinished(QDBusPendingCallWatcher* watcher);

private:
    ServicePresence servicePresent; ///< Our current understanding about the service name's state
    QDBusConnection::BusType busType; ///< The service bus type we are interested in
    QString busName; ///< The service name we are interested in
    bool listeningStarted;
    QDBusConnection* connection;

    void setServicePresent();
    void setServiceGone();

signals:
    void nameAppeared();
    void nameDisappeared();
};

} // end namespace

#endif
