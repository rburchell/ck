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

#include "provider.h"
#include "iproviderplugin.h"
#include "handlesignalrouter.h"
#include "sconnect.h"
#include "contextkitplugin.h"
#include "logging.h"
#include "loggingfeatures.h"
#include <QTimer>
#include <QMutexLocker>
#include <QCoreApplication>
#include <QThread>
#include <QLibrary>

namespace ContextSubscriber {

/*!
  \class IProviderPlugin
  \brief Interface for provider plugins.

  Every Provider instance contains exactly one plugin (pointer) with
  this interface which is constructed on initialization time and never
  change after that.  This way the concrete protocol (dbus, shared
  memory, etc.) between the library and the provider is abstracted.

  The Provider instance communicates need for subscribe and
  unsubscribe calls (on the wire) using the \c subscribe and \c
  unsubscribe methods.

  The plugin can fail or became ready anytime because of things
  happening on the wire inside the plugin (socket closed, dbus service
  appears/disappears).  Whenever the plugin has new information about
  this it should emit the signal \c ready or \c failed accordingly.

  When the plugin is ready, it has to be able to handle \c subscribe
  and \c unsubscribe function calls.  Also, after emitting \c ready it
  should be in a state where it is not subscribed to anything on the
  wire, since immediately after \c ready is emitted, the provider will
  place a subscribe call with all of the properties that should be
  subscribed.

  Subscription failures or successes can be signaled with emitting \c
  subscribeFailed and \c subscribeFinished.

  At last, but not least, the plugin can emit \c valueChanged, when it
  has a new value for any property.  It is not required to only signal
  new values, the library takes care of keeping the old value and only
  emit change signals to the upper layers if the new value is really
  new.

  An implementation of this interface doesn't have to care about
  threads at all, all of the methods, starting from the constructor
  will be only called from inside the Qt event loop of the main
  thread.  This means that neither the constructor nor the \c
  subscribe, \c unsubscribe calls should block.  They have to finish
  as soon as possible and signal the results later via signals.

  \class Provider
  \brief Connects to a group of properties via the help of a plugin.

  Each instance of this class keeps a plugin dependent communication
  channel (DBus, shared memory, etc.) open and handles subscriptions,
  value changes of the properties belonging to the provider on the
  other end of the channel.

  This class is thread safe, the \c instance, \c subscribe and \c
  unsubscribe methods can be called from any threads.  However this
  class also guarantees that the signal \c subscribeFinished and \c
  valueChanged will be always emitted from inside the main thread's
  main loop.

  \fn void Provider::subscribeFinished(QSet<QString> keys)
  \brief Emitted when the subscription procedure for \c keys finished
  (either succeeded, either failed) */

/// Stores the passed plugin name and construction paramater, then
/// moves into the main thread and queues a constructPlugin call.
Provider::Provider(const QString &plugin, const QString &constructionString)
    : plugin(0), pluginState(INITIALIZING), pluginName(plugin), constructionString(constructionString)
{
    // Move the PropertyHandle (and all children) to main thread.
    moveToThread(QCoreApplication::instance()->thread());

    queueOnce("constructPlugin");
}

/// Decides which plugin to instantiate based on the \c plugin passed
/// to the constructor.  Always called in the main loop after the
/// constructor is finished.  Each plugin library implements a
/// function which can create new instances of that plugin (TODO: come
/// up with the name of the function).
void Provider::constructPlugin()
{
    contextDebug() << F_PLUGINS;
    if (pluginName == "contextkit-dbus") {
        plugin = contextKitPluginFactory(constructionString);
    }
    else {
        // Require the plugin name to start with /
        if (pluginName.startsWith("/")) {

            // Enable overriding the plugin location with an environment variable
            const char *pluginPath = getenv("CONTEXT_SUBSCRIBER_PLUGINS");
            if (! pluginPath)
                pluginPath = DEFAULT_CONTEXT_SUBSCRIBER_PLUGINS;

            QString pluginFilename(pluginPath);
            // Allow pluginPath to have a trailing / or not
            if (pluginFilename.endsWith("/")) {
                pluginFilename.chop(1);
            }

            pluginFilename.append(pluginName);

            QLibrary library(pluginFilename);
            library.load();

            if (library.isLoaded()) {
                PluginFactoryFunc factory = (PluginFactoryFunc) library.resolve("pluginFactory");
                if (factory) {
                    contextDebug() << "Resolved factory function";
                    plugin = factory(constructionString);
                } else {
                    contextCritical() << "Error resolving function pluginFactory from plugin" << pluginFilename;
                }
            }
            else {
                contextCritical() << "Error loading plugin" << pluginFilename << ":" << library.errorString();
            }
        }
        else {
            contextCritical() << "Illegal plugin name" << pluginName << ", doesn't start with /";
        }
    }

    if (plugin == 0) {
        pluginState = FAILED;
        handleSubscribes();
        return;
    }

    // Connect the signal of changing values to the class who handles it
    HandleSignalRouter* handleSignalRouter = HandleSignalRouter::instance();
    sconnect(plugin, SIGNAL(valueChanged(QString, QVariant)),
             this, SLOT(onPluginValueChanged(QString, QVariant)));
    sconnect(this, SIGNAL(valueChanged(QString, QVariant)),
             handleSignalRouter, SLOT(onValueChanged(QString, QVariant)));

    sconnect(plugin, SIGNAL(ready()),
             this, SLOT(onPluginReady()));
    sconnect(plugin, SIGNAL(failed(QString)),
             this, SLOT(onPluginFailed(QString)));

    sconnect(plugin, SIGNAL(subscribeFinished(QString)),
             this, SLOT(onPluginSubscribeFinished(QString)), Qt::QueuedConnection);
    sconnect(plugin, SIGNAL(subscribeFailed(QString, QString)),
             this, SLOT(onPluginSubscribeFailed(QString, QString)), Qt::QueuedConnection);
    sconnect(this, SIGNAL(subscribeFinished(QString)),
             handleSignalRouter, SLOT(onSubscribeFinished(QString)));
}

/// Updates \c pluginState to \c READY and requests subscription for
/// the keys that should be subscribed.
void Provider::onPluginReady()
{
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

/// Updates \c pluginState to \c FAILED and signals subscribeFinished
/// for keys we are trying to subscribe to.
void Provider::onPluginFailed(QString error)
{
    contextWarning() << error;

    QMutexLocker lock(&subscribeLock);
    pluginState = FAILED;
    lock.unlock();
    handleSubscribes();
}

/// The plugin has finished subscribing to a key, signals this fact to
/// the upper layer.  The final API for this is the
/// <tt>waitForSubscription()</tt> method in \c ContextProperty.
void Provider::signalSubscribeFinished(QString key)
{
    QMutexLocker lock(&subscribeLock);
    if (subscribedKeys.contains(key))
        emit subscribeFinished(key);
}

/// Forwards the call to \c signalSubscribeFinished.
void Provider::onPluginSubscribeFinished(QString key)
{
    contextDebug() << key;

    signalSubscribeFinished(key);
}

/// Forwards the call to \c signalSubscribeFinished, after logging a
/// warning.
void Provider::onPluginSubscribeFailed(QString key, QString error)
{
    contextWarning() << key << error;

    signalSubscribeFinished(key);
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

/// Executed when the main loop is entered and we have previously
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
        contextDebug() << "Plugin init has failed";
        if (toSubscribe.size() > 0)
            foreach (QString key, toSubscribe)
                emit subscribeFinished(key);
        toSubscribe.clear();
        toUnsubscribe.clear();
        break;
    case INITIALIZING:
        // we just have to wait,
        break;
    }

    contextDebug() << "Provider::handleSubscribes processed";
}

/// Forwards the \c newValue for \c key received from the plugin to
/// the upper layers via \c HandleSignalRouter.
void Provider::onPluginValueChanged(QString key, QVariant newValue)
{
    QMutexLocker lock(&subscribeLock);
    if (subscribedKeys.contains(key))
        emit valueChanged(key, newValue);
    else
        contextWarning() << "Received a property not subscribed to:" << key;
}

/// Returns a singleton for the named \c plugin with the \c constructionString.
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
