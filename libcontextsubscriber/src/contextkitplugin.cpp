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

#include "contextkitplugin.h"
#include "logging.h"
#include "managerinterface.h"
#include "subscriberinterface.h"
#include "sconnect.h"
#include <QStringList>

namespace ContextSubscriber {

IProviderPlugin* contextKitPluginFactory(const QString& constructionString)
{
    QStringList constr = constructionString.split(":");
    if (constr.size() != 2) {
        contextCritical() << "Bad syntax for contextkit-dbus plugin:" << constructionString;
        return 0;
    }
    if (constr[0] == "session")
        return new ContextKitPlugin(QDBusConnection::sessionBus(), constr[1]);
    else if (constr[0] == "system")
        return new ContextKitPlugin(QDBusConnection::systemBus(), constr[1]);

    contextCritical() << "Unknown bus type: " << constructionString;
    return 0;
}

ContextKitPlugin::ContextKitPlugin(const QDBusConnection bus, const QString& busName)
    : subscriberInterface(0), managerInterface(0), connection(new QDBusConnection(bus)),
      busName(busName)
{
    // Call GetSubscriber asynchronously
    managerInterface = new ManagerInterface(*connection, busName, this);
    sconnect(managerInterface, SIGNAL(getSubscriberFinished(QString)), this, SLOT(onDBusGetSubscriberFinished(QString)));

    // Notice if the provider on the dbus comes and goes
    providerListener = new DBusNameListener(bus, busName, this);
    sconnect(providerListener, SIGNAL(nameAppeared()),
             this, SLOT(onProviderAppeared()));
    sconnect(providerListener, SIGNAL(nameDisappeared()),
             this, SLOT(onProviderDisappeared()));
    // Listen to the provider appearing and disappearing, but don't
    // perform the initial NameHasOwner check.  We will try to connect
    // to the provider at startup, whether it's present or not.
    providerListener->startListening(false);

    // Just try to connect to the provider.
    onProviderAppeared();
}

/// Provider reappeared on the DBus, so get a new subscriber interface from it
void ContextKitPlugin::onProviderAppeared()
{
    contextDebug() << "ContextKitPlugin::onProviderAppeared";
    delete subscriberInterface;
    subscriberInterface = 0;
    managerInterface->getSubscriber();
}

/// Delete our subscriber interface when the provider went away
void ContextKitPlugin::onProviderDisappeared()
{
    contextDebug() << "ContextKitPlugin::onProviderDisappeared";
    delete subscriberInterface;
    subscriberInterface = 0;
    emit failed("Provider went away");
}

/// Called when the manager interface is ready with the asynchronous
/// GetSubscriber call.
void ContextKitPlugin::onDBusGetSubscriberFinished(QString objectPath)
{
    if (objectPath != "") {
        delete subscriberInterface;
        subscriberInterface = new SubscriberInterface(*connection, busName, objectPath, this);
        sconnect(subscriberInterface,
                 SIGNAL(valuesChanged(QMap<QString, QVariant>)),
                 this,
                 SLOT(onDBusValuesChanged(QMap<QString, QVariant>)));
        sconnect(subscriberInterface,
                 SIGNAL(subscribeFinished(QList<QString>)),
                 this,
                 SLOT(onDBusSubscribeFinished(QList<QString>)));
        sconnect(subscriberInterface,
                 SIGNAL(subscribeFailed(QList<QString>, QString)),
                 this,
                 SLOT(onDBusSubscribeFailed(QList<QString>, QString)));

        emit ready();
    } else
        emit failed("Was unable to get subscriber object on dbus");
}

void ContextKitPlugin::onDBusSubscribeFinished(QList<QString> keys)
{
    foreach (const QString& key, keys)
        emit subscribeFinished(key);
}

void ContextKitPlugin::onDBusSubscribeFailed(QList<QString> keys, QString error)
{
    foreach (const QString& key, keys)
        emit subscribeFailed(key, error);
}

void ContextKitPlugin::subscribe(QSet<QString> keys)
{
    subscriberInterface->subscribe(keys);
}

void ContextKitPlugin::unsubscribe(QSet<QString> keys)
{
    subscriberInterface->unsubscribe(keys);
}

/// Slot, handling changed values coming from the provider over DBUS.
void ContextKitPlugin::onDBusValuesChanged(QMap<QString, QVariant> values)
{
    foreach (const QString& key, values.keys())
        emit valueChanged(key, values[key]);
}

}
