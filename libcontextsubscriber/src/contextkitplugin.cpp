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
#include "subscriberinterface.h"
#include "sconnect.h"
#include <QStringList>
#include <QTimer>

/// Creates a new instance, the service to connect to has to be passed
/// in \c constructionString in the format <tt>[session|dbus]:servicename</tt>.
ContextSubscriber::IProviderPlugin* contextKitPluginFactory(QString constructionString)
{
    QStringList constr = constructionString.split(":");
    if (constr.size() != 2) {
        contextCritical() << "Bad syntax for contextkit-dbus plugin:" << constructionString;
        return 0;
    }
    if (constr[0] == "session")
        return new ContextSubscriber::ContextKitPlugin(QDBusConnection::sessionBus(), constr[1]);
    else if (constr[0] == "system")
        return new ContextSubscriber::ContextKitPlugin(QDBusConnection::systemBus(), constr[1]);

    contextCritical() << "Unknown bus type: " << constructionString;
    return 0;
}

namespace ContextSubscriber {

/*!
  \class ContextKitPlugin
  \brief Implementation of the ContextKit D-Bus protocol.
*/

const QString ContextKitPlugin::managerPath = "/org/freedesktop/ContextKit/Manager";
const QString ContextKitPlugin::managerIName = "org.freedesktop.ContextKit.Manager";

/// Creates subscriber and manager interface, tries to get a
/// subscriber instance from the manager and starts listening for
/// provider appearing and disappearing on D-Bus.
ContextKitPlugin::ContextKitPlugin(const QDBusConnection bus, const QString& busName)
    : providerListener(new DBusNameListener(bus, busName, this)),
      subscriberInterface(0),
      managerInterface(0),
      connection(new QDBusConnection(bus)),
      busName(busName)
{
    // Notice if the provider on the dbus comes and goes
    sconnect(providerListener, SIGNAL(nameAppeared()),
             this, SLOT(onProviderAppeared()));
    sconnect(providerListener, SIGNAL(nameDisappeared()),
             this, SLOT(onProviderDisappeared()));

    // Listen to the provider appearing and disappearing, but don't
    // perform the initial NameHasOwner check.  We try to connect to
    // the provider at startup, whether it's present or not.
    providerListener->startListening(false);
    QMetaObject::invokeMethod(this, "onProviderAppeared", Qt::QueuedConnection);
}

/// Gets a new subscriber interface from manager when the provider
/// appears.
void ContextKitPlugin::onProviderAppeared()
{
    contextDebug() << "ContextKitPlugin::onProviderAppeared";

    delete subscriberInterface;
    subscriberInterface = 0;
    delete managerInterface;
    managerInterface = new AsyncDBusInterface(busName, managerPath, managerIName, *connection, this);
    if (!managerInterface->callWithCallback("GetSubscriber",
                                            QList<QVariant>(),
                                            this,
                                            SLOT(onDBusGetSubscriberFinished(QDBusObjectPath)),
                                            SLOT(onDBusGetSubscriberFailed(QDBusError)))) {
        emit failed(QString("Wasn't able to call GetSubscriber on the managerinterface: ") +
                    managerInterface->lastError().message());
    }
}

/// Delete our subscriber interface when the provider goes away.
void ContextKitPlugin::onProviderDisappeared()
{
    contextDebug() << "ContextKitPlugin::onProviderDisappeared";
    delete subscriberInterface;
    subscriberInterface = 0;
    emit failed("Provider went away");
}

/// Starts using the fresh subscriber interface when it is returned by
/// the manager in response to the GetSubscriber call.
void ContextKitPlugin::onDBusGetSubscriberFinished(QDBusObjectPath objectPath)
{
    delete subscriberInterface;
    subscriberInterface = new SubscriberInterface(*connection, busName, objectPath.path(), this);
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
}

void ContextKitPlugin::onDBusGetSubscriberFailed(QDBusError err)
{
    emit failed("Was unable to get subscriber object on dbus: " + err.message());
}

/// Signals the Provider that the subscribe is finished.
void ContextKitPlugin::onDBusSubscribeFinished(QList<QString> keys)
{
    foreach (const QString& key, keys)
        emit subscribeFinished(key);
}

/// Signals the Provider that the subscribe is failed.
void ContextKitPlugin::onDBusSubscribeFailed(QList<QString> keys, QString error)
{
    foreach (const QString& key, keys)
        emit subscribeFailed(key, error);
}

/// Forwards the subscribe request to the wire.
void ContextKitPlugin::subscribe(QSet<QString> keys)
{
    subscriberInterface->subscribe(keys);
}

/// Forwards the unsubscribe request to the wire.
void ContextKitPlugin::unsubscribe(QSet<QString> keys)
{
    subscriberInterface->unsubscribe(keys);
}

/// Forwards value changes from the wire to the upper layer (Provider).
void ContextKitPlugin::onDBusValuesChanged(QMap<QString, QVariant> values)
{
    foreach (const QString& key, values.keys())
        emit valueChanged(key, values[key]);
}

}
