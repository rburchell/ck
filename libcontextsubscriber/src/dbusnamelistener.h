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
#include <QDBusConnection>
#include <QString>

class QDBusPendingCallWatcher;

namespace ContextSubscriber {

class DBusNameListener : public QObject
{
    Q_OBJECT
public:
    DBusNameListener(const QDBusConnection::BusType busType, const QString &busName,
                     bool initialCheck = true, QObject *parent = 0);
    bool isServicePresent() const;

private slots:
    void onServiceOwnerChanged(const QString &name, const QString &oldOwner, const QString &newOwner);
    void onNameHasOwnerFinished(QDBusPendingCallWatcher* watcher);

private:
    bool servicePresent; //< Our current understanding about the service name's state
    QString busName; //< The service name we are interested in

    void setServicePresent();
    void setServiceGone();

signals:
    void nameAppeared();
    void nameDisappeared();
};

} // end namespace

#endif
