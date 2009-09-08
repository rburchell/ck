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
#include "contextkitplugin.h"
#include "logging.h"
#include <QTimer>
#include <QMutexLocker>
#include <QCoreApplication>
#include <QThread>

namespace ContextSubscriber {

/*!
   \class Provider

   \brief Keeps the DBUS connection with a specific provider and
   handles subscriptions and value changes with the properties of that
   provider.
*/

Provider::Provider(const QString &plugin, const QString &constructionString)
    : plugin(0), pluginState(INITIALIZING), pluginName(plugin), constructionString(constructionString)
{
    queueOnce("constructPlugin");
}

void Provider::constructPlugin()
{
    if (pluginName == "contextkit-dbus") {
        plugin = contextKitPluginFactory(constructionString);
    } else ; // FIXME: implement plugin system in the else branch

    if (plugin == 0) {
        pluginState = FAILED;
        handleSubscribes();
        return;
    }

    // Connect the signal of changing values to the class who handles it
    HandleSignalRouter* handleSignalRouter = HandleSignalRouter::instance();
    sconnect(plugin, SIGNAL(valueChanged(QString, QVariant)),
             handleSignalRouter, SLOT(onValueChanged(QString, QVariant)));

    sconnect(plugin, SIGNAL(ready()),
             this, SLOT(onPluginReady()), Qt::QueuedConnection);
    sconnect(plugin, SIGNAL(failed(QString)),
             this, SLOT(onPluginFailed(QString)), Qt::QueuedConnection);

    // FIXME: signal these things through the handlesignalrouter
    sconnect(plugin, SIGNAL(subscribeFinished(QString)),
             this, SLOT(onPluginSubscribeFinished(QString)), Qt::QueuedConnection);
    sconnect(plugin, SIGNAL(subscribeFailed(QString, QString)),
             this, SLOT(onPluginSubscribeFailed(QString, QString)), Qt::QueuedConnection);
}

void Provider::onPluginReady()
{
    // FIXME: try this out :)
    contextDebug();

    QMutexLocker lock(&subscribeLock);
    // Renew the subscriptions (if any).
    // Renewing happens when a provider has disappeared and now it appeared again.
    toUnsubscribe.clear();
    toSubscribe.clear();
    toSubscribe += subscribedKeys;
    pluginState = READY;
    lock.unlock();
    handleSubscribes();
}

void Provider::onPluginFailed(QString error)
{
    contextWarning() << error;

    QMutexLocker lock(&subscribeLock);
    pluginState = FAILED;
    lock.unlock();
    handleSubscribes();
}

/// Called when the subscriber interface is ready with the
/// asynchronous Subscribe call. Emit the \c subscribeFinished signal
/// which will be caught by the \c PropertyHandle objects to set the
/// \c subscribePending boolean which is used by the not so busy
/// busyloop in <tt>waitForSubscription()</tt>.
void Provider::onPluginSubscribeFinished(QString key)
{
    contextDebug() << key;

    QMutexLocker lock(&subscribeLock);
    if (subscribedKeys.contains(key))
        // FIXME: get rid of QSet, go through signalhandlerouter
        emit subscribeFinished(QSet<QString>() << key);
}

void Provider::onPluginSubscribeFailed(QString key, QString error)
{
    contextDebug() << "Provider::onPluginSubscribeFailed" << key << error;

    QMutexLocker lock(&subscribeLock);
    if (subscribedKeys.contains(key))
        emit subscribeFinished(QSet<QString>() << key);
}

/// Schedules a property to be subscribed to.  Returns true if and
/// only if the main loop has to run for the subscription to be
/// finalized.
bool Provider::subscribe(const QString &key)
{
    contextDebug() << "plugin" << pluginName << constructionString;

    QMutexLocker lock(&subscribeLock);
    // Note: the intention is saved in all cases; whether we can really subscribe or not.
    subscribedKeys.insert(key);

    // If the key was scheduled to be unsubscribed then remove that
    // scheduling, and return false.
    if (toUnsubscribe.contains(key)) {
        toUnsubscribe.remove(key);
        return false;
    }

    // Schedule the key to be subscribed to and return true
    contextDebug() << "Inserting the key to toSubscribe" << QThread::currentThread();

    // Really schedule the key to be subscribed to
    toSubscribe.insert(key);

    queueOnce("handleSubscribes");

    return true;
}

/// Schedules a property to be unsubscribed from when the main loop is
/// entered the next time.
void Provider::unsubscribe(const QString &key)
{
    contextDebug() << "Provider::unsubscribe, plugin" << pluginName << constructionString;

    QMutexLocker lock(&subscribeLock);
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

        queueOnce("handleSubscribes");
    }
}

/// Is executed when the main loop is entered and we have previously
/// scheduled subscriptions / unsubscriptions.
void Provider::handleSubscribes()
{
    contextDebug() << "Provider::handleSubscribes in thread" << QThread::currentThread();

    QMutexLocker lock(&subscribeLock);

    switch (pluginState) {
    case READY:
        if (toSubscribe.size() > 0) plugin->subscribe(toSubscribe);
        if (toUnsubscribe.size() > 0) plugin->unsubscribe(toUnsubscribe);
        toSubscribe.clear();
        toUnsubscribe.clear();
        break;
    case FAILED:
        // it failed for good.
        contextDebug() << "Plugin init has failed previously";
        if (toSubscribe.size() > 0) emit subscribeFinished(toSubscribe);
        toSubscribe.clear();
        toUnsubscribe.clear();
        break;
    case INITIALIZING:
        // we just have to wait,
        break;
    }

    contextDebug() << "Provider::handleSubscribes processed";
}

void Provider::onPluginValueChanged(QString key, QVariant newValue)
{
    QMutexLocker lock(&subscribeLock);
    if (subscribedKeys.contains(key))
        HandleSignalRouter::instance()->onValueChanged(key, newValue);
    else
        contextWarning() << "Received a property not subscribed to:" << key;
}

Provider* Provider::instance(const QString& plugin, const QString& constructionString)
{
    // Singleton instance container
    // plugin path, constructionstring -> Provider
    static QMap<QPair<QString, QString>, Provider*> providerInstances;
    QPair<QString, QString> lookupValue(plugin, constructionString);

    static QMutex providerInstancesLock;
    QMutexLocker locker(&providerInstancesLock);
    if (!providerInstances.contains(lookupValue))
        providerInstances.insert(lookupValue, new Provider(plugin, constructionString));

    contextDebug() << "Returning provider instance for" << plugin << ":" << constructionString;

    return providerInstances[lookupValue];
}


} // end namespace


