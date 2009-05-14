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

#include "propertyhandle.h"
#include "sconnect.h"
#include "subscriberinterface.h"
#include "dbusnamelistener.h"

QMap<QPair<QDBusConnection::BusType, QString>, PropertyProvider*> PropertyProvider::providerInstances;

/*!
   \class PropertyProvider

   \brief Keeps the DBUS connection with a specific provider and
   handles subscriptions with the properties of that provider.
*/

/// Constructs a new instance. ContextProperty handles providers for
/// you, no need to (and can't) call this constructor.
PropertyProvider::PropertyProvider(QDBusConnection::BusType busType, const QString& busName)
    : busType(busType), busName(busName), managerInterface(busType, busName, this), subscriberInterface(0)
{
    // Setup idle timer
    idleTimer.setSingleShot(true);
    idleTimer.setInterval(0);

    // Call GetSubscriber asynchronously
    sconnect(&managerInterface, SIGNAL(getSubscriberFinished(QString)), this, SLOT(onGetSubscriberFinished(QString)));
    sconnect(&idleTimer, SIGNAL(timeout()),
             this, SLOT(idleHandler()));
    managerInterface.getSubscriber();

    // Note if the provider on the dbus comes and go
    dbusNameListener = new DBusNameListener(busType, busName);
    sconnect(dbusNameListener,
             SIGNAL(nameAppeared()),
             this,
             SLOT(onProviderAppears()));
    sconnect(dbusNameListener,
             SIGNAL(nameAppeared()),
             this,
             SLOT(onProviderDisappears()));
}

void PropertyProvider::onProviderAppears()
{
    if (subscriberInterface != 0) {
        delete subscriberInterface;
        subscriberInterface = 0;
    }
    managerInterface.getSubscriber();
}

void PropertyProvider::onProviderDisappears()
{
    if (subscriberInterface != 0) {
        delete subscriberInterface;
        subscriberInterface = 0;
    }
}

void PropertyProvider::onGetSubscriberFinished(QString objectPath)
{
    qDebug() << "PropertyProvider::onGetSubscriberFinished" << objectPath;
    if (objectPath != "") {
        // GetSubscriber was successful
        subscriberInterface = new SubscriberInterface(busType, busName, objectPath, this);

        sconnect(subscriberInterface,
                 SIGNAL(valuesChanged(QMap<QString, QVariant>, bool)),
                 this,
                 SLOT(onValuesChanged(QMap<QString, QVariant>, bool)));
        sconnect(subscriberInterface,
                 SIGNAL(subscribeFinished(QSet<QString>)),
                 this,
                 SLOT(onSubscribeFinished(QSet<QString>)));

        // TODO: foreach toSubscribe -> if handle.provider == me -> add to toSubscribe()
        // TODO: toSubscribe.clear();
        // TODO: toUnsubscribe.clear();

        idleTimer.start();
    }
    else {
        // GetSubscriber failed
        emit subscribeFinished(toSubscribe);
        toSubscribe.clear();
        toUnsubscribe.clear();
    }
}

void PropertyProvider::onSubscribeFinished(QSet<QString> keys)
{
    qDebug() << "PropertyProvider::onSubscribeFinished" << keys;
    emit subscribeFinished(keys);
}

/// Returns the dbus name and bus type of the provider
QString PropertyProvider::getName() const
{
    return ""; /// FIXME: Remove this function?
}

/// Schedules a property to be subscribed to when the main loop is
/// entered the next time.
void PropertyProvider::subscribe(const QString &key)
{
    qDebug() << "PropertyProvider::subscribe";

    if (managerInterface.isGetSubscriberFailed()) {
        qDebug() << "GetSubscriber has failed previously";
        // The subscription will never be successful
        QSet<QString> toEmit;
        toEmit.insert(key);
        emit subscribeFinished(toEmit);
        return;
    }

    toSubscribe.insert(key);
    toUnsubscribe.remove(key);

    if (subscriberInterface != 0)
        idleTimer.start();
}

void PropertyProvider::idleHandler()
{
    if (subscriberInterface != 0) {
        subscriberInterface->subscribe(toSubscribe);
        subscriberInterface->unsubscribe(toUnsubscribe);
        toSubscribe.clear();
        toUnsubscribe.clear();
    }
}

/// Schedules a property to be unsubscribed from when the main loop is
/// entered the next time.
void PropertyProvider::unsubscribe(const QString &key)
{
    toUnsubscribe.insert(key);
    toSubscribe.remove(key);

    if (subscriberInterface != 0)
        idleTimer.start();
}

/// Slot, handling changed values coming from the provider over DBUS.
void PropertyProvider::onValuesChanged(QMap<QString, QVariant> values, bool processingSubscription)
{
    for (QMap<QString, QVariant>::const_iterator i = values.constBegin(); i != values.constEnd(); ++i) {
        // Get the PropertyHandle corresponding to the property to update.
        // FIXME: handlefactory->handle(i.key())
        PropertyHandle *h = PropertyHandle::instance(i.key());
        if (h->provider() != this) {
            if (h->provider() == 0)
                qWarning() << "Received property not in registry:" << i.key();
            else
                qWarning() << "Received property not handled by this provider:" << i.key();
            continue;
        }

        h->setValue(i.value(), processingSubscription);
    }
}

PropertyProvider::~PropertyProvider()
{
    // Note: The SubscriberInterface* subscriber was constructed with "this" as parent.
    // It doesn't need to be deleted.
}

PropertyProvider* PropertyProvider::instance(const QDBusConnection::BusType busType, const QString& busName)
{
    QPair<QDBusConnection::BusType, QString> lookupValue(busType, busName);
    if (!providerInstances.contains(lookupValue))
            providerInstances.insert(lookupValue,
                                     new PropertyProvider(busType, busName));

    qDebug() << "Returning provider instance for" << busType << ":" << busName;

    return providerInstances[lookupValue];
}

