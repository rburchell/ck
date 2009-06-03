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

#include "propertyprovider.h"
#include "handlesignalrouter.h"
#include "sconnect.h"
#include "subscriberinterface.h"
#include "dbusnamelistener.h"
#include "logging.h"
#include <QTimer>

QMap<QPair<QDBusConnection::BusType, QString>, PropertyProvider*> PropertyProvider::providerInstances;

/*!
   \class PropertyProvider

   \brief Keeps the DBUS connection with a specific provider and
   handles subscriptions and value changes with the properties of that
   provider.
*/

PropertyProvider::PropertyProvider(QDBusConnection::BusType busType, const QString& busName)
    : subscriberInterface(0), managerInterface(busType, busName, this), busType(busType), busName(busName)
{
    // Setup idle timer, 0: always run it when we are entering the
    // mainloop and the timer is enabled.
    idleTimer = new QTimer(this);
    idleTimer->setSingleShot(true);
    idleTimer->setInterval(0);

    // Call GetSubscriber asynchronously
    sconnect(&managerInterface, SIGNAL(getSubscriberFinished(QString)), this, SLOT(onGetSubscriberFinished(QString)));
    sconnect(idleTimer, SIGNAL(timeout()), this, SLOT(idleHandler()));
    managerInterface.getSubscriber();

    // Note if the provider on the dbus comes and go
    providerListener = new DBusNameListener(busType, busName, false, this);
    sconnect(providerListener, SIGNAL(nameAppeared()),
             this, SLOT(onProviderAppeared()));
    sconnect(providerListener, SIGNAL(nameDisappeared()),
             this, SLOT(onProviderDisappeared()));

    // Connect the signal of changing values to the class who handles it
    HandleSignalRouter* handleSignalRouter = HandleSignalRouter::instance();
    sconnect(this, SIGNAL(valueChanged(QString, QVariant, bool)),
             handleSignalRouter, SLOT(onValueChanged(QString, QVariant, bool)));
}

/// Provider reappeared on the DBus, so get a new subscriber interface from it
void PropertyProvider::onProviderAppeared()
{
    delete subscriberInterface;
    subscriberInterface = 0;
    managerInterface.getSubscriber();
}

/// Delete our subscriber interface when the provider went away
void PropertyProvider::onProviderDisappeared()
{
    delete subscriberInterface;
    subscriberInterface = 0;
}

/// Called when the manager interface is ready with the asynchronous
/// GetSubscriber call.  We use the new subscriber interface to
/// subscribe to the currently subscribed keys.
void PropertyProvider::onGetSubscriberFinished(QString objectPath)
{
    contextDebug() << "PropertyProvider::onGetSubscriberFinished " << objectPath;
    if (objectPath != "") {
        // GetSubscriber was successful
        delete subscriberInterface;
        subscriberInterface = new SubscriberInterface(busType, busName, objectPath, this);

        sconnect(subscriberInterface,
                 SIGNAL(valuesChanged(QMap<QString, QVariant>, bool)),
                 this,
                 SLOT(onValuesChanged(QMap<QString, QVariant>, bool)));
        sconnect(subscriberInterface,
                 SIGNAL(subscribeFinished(QSet<QString>)),
                 this,
                 SLOT(onSubscribeFinished(QSet<QString>)));

        // Renew the subscriptions (if any).
        // Renewing happens when a provider has disappeared and now it appeared again.

        toUnsubscribe.clear();
        toSubscribe.clear();
        toSubscribe += subscribedKeys;

        idleTimer->start();
    }
    else {
        // GetSubscriber failed
        emit subscribeFinished(toSubscribe);
        toSubscribe.clear();
        toUnsubscribe.clear();
    }
}

/// Called when the subscriber interface is ready with the
/// asynchronous Subscribe call. Emit the \c subscribeFinished signal
/// which will be caught by the \c PropertyHandle objects to set the
/// \c subscribePending boolean which is used by the not so busy
/// busyloop in <tt>waitForSubscription()</tt>.
void PropertyProvider::onSubscribeFinished(QSet<QString> keys)
{
    contextDebug() << "PropertyProvider::onSubscribeFinished ";
    // FIXME: contextDebug() << "PropertyProvider::onSubscribeFinished " << keys;
    // Drop keys which were not supposed to be subscribed to (by this provider)
    emit subscribeFinished(keys.intersect(subscribedKeys));
}

/// Schedules a property to be subscribed to when the main loop is
/// entered the next time.
void PropertyProvider::subscribe(const QString &key)
{
    contextDebug() << "PropertyProvider::subscribe, provider" << busName << " " << key;

    // Note: the intention is saved in all cases; whether we can really subscribe or not.
    subscribedKeys.insert(key);

    if (managerInterface.isGetSubscriberFailed()) {
        contextDebug() << "GetSubscriber has failed previously";
        // The subscription will not be successful, emit the subscribeFinished
        // signal so that the handles will stop waiting.
        QSet<QString> toEmit;
        toEmit.insert(key);
        emit subscribeFinished(toEmit);
        return;
    }

    // Schedule the key to be subscribed to
    if (toUnsubscribe.contains(key)) {
        // The key was scheduled to be unsubscribed
        // Remove that scheduling, and nothing else needs to be done.
        toUnsubscribe.remove(key);
    }
    else {
        // Really schedule the key to be subscribed to
        toSubscribe.insert(key);

        if (subscriberInterface != 0)
            idleTimer->start();
    }
}

/// Schedules a property to be unsubscribed from when the main loop is
/// entered the next time.
void PropertyProvider::unsubscribe(const QString &key)
{
    contextDebug() << "PropertyProvider::unsubscribe, provider" << busName << " " << key;

    // Save the intention of the higher level
    subscribedKeys.remove(key);

    // Schedule the key to be unsubscribed from
    if (toSubscribe.contains(key)) {
        // The key was scheduled to be subscribed
        // Remove that scheduling, and nothing else needs to be done.
        toSubscribe.remove(key);
    }
    else {
        // Really schedule the key to be unsubscribed from
        toUnsubscribe.insert(key);

        if (subscriberInterface != 0)
            idleTimer->start();
    }
}

/// Is executed when the main loop is entered and we have previously
/// scheduled subscriptions / unsubscriptions.
void PropertyProvider::idleHandler()
{
    if (subscriberInterface != 0) {
        if (toSubscribe.size() > 0) subscriberInterface->subscribe(toSubscribe);
        if (toUnsubscribe.size() > 0) subscriberInterface->unsubscribe(toUnsubscribe);
        toSubscribe.clear();
        toUnsubscribe.clear();
    }
}

/// Slot, handling changed values coming from the provider over DBUS.
void PropertyProvider::onValuesChanged(QMap<QString, QVariant> values, bool processingSubscription)
{
    for (QMap<QString, QVariant>::const_iterator i = values.constBegin(); i != values.constEnd(); ++i) {
        if (subscribedKeys.contains(i.key()) == false) {
            contextWarning() << "Received a property not subscribed to:" << i.key();
        } else {
            // Note: HandleSignalRouter will catch this signal and set the value of the
            // corresponding PropertyHandle.
            emit valueChanged(i.key(), i.value(), processingSubscription);
        }
    }
}

PropertyProvider* PropertyProvider::instance(const QDBusConnection::BusType busType, const QString& busName)
{
    QPair<QDBusConnection::BusType, QString> lookupValue(busType, busName);
    if (!providerInstances.contains(lookupValue))
            providerInstances.insert(lookupValue,
                                     new PropertyProvider(busType, busName));

    contextDebug() << "Returning provider instance for " << busType << ":" << busName;

    return providerInstances[lookupValue];
}

