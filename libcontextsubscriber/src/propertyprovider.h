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

#ifndef PROPERTYPROVIDER_H
#define PROPERTYPROVIDER_H

#include "managerinterface.h"

#include <QObject>
#include <QDBusConnection>
#include <QSet>

class QTimer;

namespace ContextSubscriber {

class PropertyHandle;
class SubscriberInterface;
class DBusNameListener;

class PropertyProvider : public QObject
{
    Q_OBJECT

public:
    void subscribe(const QString &key);
    void unsubscribe(const QString &key);
    static PropertyProvider* instance(const QDBusConnection::BusType busType, const QString& busName);

signals:
    void subscribeFinished(QSet<QString> keys);
    void valueChanged(QString key, QVariant value, bool processingSubscription);

private slots:
    void onValuesChanged(QMap<QString, QVariant> values, bool processingSubscription);
    void onGetSubscriberFinished(QString objectPath);
    void onSubscribeFinished(QSet<QString> keys);
    void idleHandler();
    void onProviderAppeared();
    void onProviderDisappeared();

private:
    PropertyProvider (QDBusConnection::BusType busType, const QString& busName);

    DBusNameListener *providerListener; //< Listens to provider's (dis)appearance over DBus
    SubscriberInterface *subscriberInterface; //< The DBus interface for the Subscriber object
    QTimer *idleTimer; //< For scheduling subscriptions / unsubscriptions as idle processing
    ManagerInterface managerInterface; //< The DBus interface for the Manager object
    bool getSubscriberFailed; //< Whether the GetSubscriber dbus call failed on the manager interface

    QSet<QString> toSubscribe; //< Keys pending for subscription
    QSet<QString> toUnsubscribe; //< Keys pending for unsubscription

    QDBusConnection::BusType busType; //< The bus type of the DBus provider connected to
    QString busName; //< The bus name of the DBus provider connected to

    QSet<QString> subscribedKeys; //< The keys that should be currently subscribed to
};

} // end namespace

#endif
