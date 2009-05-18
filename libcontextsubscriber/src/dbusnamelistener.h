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

// FIXME: Documentation for this class

#ifndef DBUSNAMELISTENER_H
#define DBUSNAMELISTENER_H

#include <QObject>
#include <QDBusConnection>
#include <QString>

template<typename P1, typename P2> class QPair;
template<typename K, typename V> class QMap;

class QDBusPendingCallWatcher;

class DBusNameListener : public QObject
{
    Q_OBJECT
public:
    static DBusNameListener* instance(const QDBusConnection::BusType busType, const QString &busName);
    bool isServicePresent() const;

private slots:
    void onServiceOwnerChanged(const QString &name, const QString &oldOwner, const QString &newOwner);
    void onNameHasOwnerFinished(QDBusPendingCallWatcher* watcher);

private:
    bool servicePresent;
    QString busName;
    static QMap<QPair<QDBusConnection::BusType, QString>, DBusNameListener*> listenerInstances;

    DBusNameListener(const QDBusConnection::BusType busType, const QString &busName);
    void setServicePresent();
    void setServiceGone();

signals:
    void nameAppeared();
    void nameDisappeared();
};

#endif
