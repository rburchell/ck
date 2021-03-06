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

  Note: this interface is private, currently it is not advised to use
  it and create ContextKit subscriber plugins on your own, we can and
  will change this interface anytime in the future even between small
  bugfix releases.

  Every Provider instance contains exactly one plugin (pointer) with
  this interface which is constructed on initialization time and never
  change after that.  This way the concrete protocol (dbus, shared
  memory, etc.) between the library and the provider is abstracted.

  The Provider instance communicates need for subscribe and
  unsubscribe calls (on the wire) using the \c subscribe and \c
  unsubscribe methods.

  When the plugin is constructed, it should emit the signal ready()
  when it is ready to take in subscriptions. However, the signal
  ready() should not be emitted in the plugin constructor. If the
  plugin is able to take in subscriptions immediately, you can use
  QMetaObject::invokeMethod with QueuedConnection to emit the signal
  when the main loop is entered the next time.

  The plugin can fail or became ready again anytime because of things
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
  subscribeFailed and \c subscribeFinished. When the upper layer has called
  subscribe(), the plugin must eventually emit \c subscribeFinished or \c
  subscribeFailed for all the subscribed keys, or emit \c failed.

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
Provider::Provider(const ContextProviderInfo& providerInfo)
    : plugin(0), pluginState(INITIALIZING), providerInfo(providerInfo),
      subscribeLock(QMutex::Recursive), pluginConstructed(false)
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
    if (pluginConstructed)
        return;
    pluginConstructed = true;

    contextDebug() << F_PLUGINS;
    if (providerInfo.plugin == "contextkit-dbus") {
        plugin = contextKitPluginFactory(providerInfo.constructionString);
    }
    else if (providerInfo.plugin.startsWith("/")) {
        // Dynamically loaded plugins have to start with a '/', otherwise we consider them internal.
        // Enable overriding the plugin location with an environment variable
        const char *pluginPath = getenv("CONTEXT_SUBSCRIBER_PLUGINS");
        if (! pluginPath)
            pluginPath = DEFAULT_CONTEXT_SUBSCRIBER_PLUGINS;

        QString pluginFilename(pluginPath);
        // Allow pluginPath to have a trailing / or not
        if (pluginFilename.endsWith("/")) {
            pluginFilename.chop(1);
        }

        pluginFilename.append(providerInfo.plugin);

        QLibrary library(pluginFilename);
        library.load();

        if (library.isLoaded()) {
            PluginFactoryFunc factory = (PluginFactoryFunc) library.resolve("pluginFactory");
            if (factory) {
                contextDebug() << "Resolved factory function";
                plugin = factory(providerInfo.constructionString);
            } else {
                contextCritical() << "Error resolving function pluginFactory from plugin" << pluginFilename;
            }
        }
        else {
            contextCritical() << "Error loading plugin" << pluginFilename << ":" << library.errorString();
        }
    }
    else {
        contextCritical() << "Illegal plugin name" << providerInfo.plugin << ", doesn't start with /";
    }

    // Connect the subscribeFinished signal early enough; if plugin loading has
    // failed, we still need to send it.
    HandleSignalRouter* handleSignalRouter = HandleSignalRouter::instance();
    sconnect(this, SIGNAL(subscribeFinished(Provider *,QString)),
             handleSignalRouter, SLOT(onSubscribeFinished(Provider *,QString)));

    if (plugin == 0) {
        pluginState = FAILED;
        handleSubscribes();
        return;
    }

    // Connect the signal of changing values to the class who handles it
    sconnect(plugin, SIGNAL(valueChanged(QString, TimedValue)),
             this, SLOT(onPluginValueChanged(QString, TimedValue)));
    sconnect(plugin, SIGNAL(valueChanged(QString, QVariant)),
             this, SLOT(onPluginValueChanged(QString, QVariant)));
    sconnect(this, SIGNAL(valueChanged(QString)),
             handleSignalRouter, SLOT(onValueChanged(QString)));

    // Ready and failed are supposed to be handled immediately; not
    // queued. The plugin should also emit them as soon as possible
    // and not queue them internally.
    sconnect(plugin, SIGNAL(ready()),
             this, SLOT(onPluginReady()));
    sconnect(plugin, SIGNAL(failed(QString)),
             this, SLOT(onPluginFailed(QString)));

    // The following signals (as well as valueChanged) can be emitted in
    // subscribe() of the plugin.  Here we utilize the fact that our mutexes are
    // recursive.
    qRegisterMetaType<TimedValue>("TimedValue");
    sconnect(plugin, SIGNAL(subscribeFinished(QString, TimedValue)),
             this, SLOT(onPluginSubscribeFinished(QString, TimedValue)));
    sconnect(plugin, SIGNAL(subscribeFinished(QString)),
             this, SLOT(onPluginSubscribeFinished(QString)));
    sconnect(plugin, SIGNAL(subscribeFailed(QString, QString)),
             this, SLOT(onPluginSubscribeFailed(QString, QString)));
}

/// Updates \c pluginState to \c READY and requests subscription for
/// the keys that should be subscribed.
void Provider::onPluginReady()
{
    contextDebug();

    // Ignore the signal if the plugin is already in the ready state.
    if (pluginState == READY)
        return;

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

/// Clears the cached values for this provider.  This is used when the
/// provider instance is (re)connected to the commander.
void Provider::clearValues()
{
    values.clear();
}

/// Updates \c pluginState to \c FAILED and signals subscribeFinished
/// for keys we are trying to subscribe to.
void Provider::onPluginFailed(QString error)
{
    // This function is called during "normal operation" when the subscriber is
    // started before the provider. Contextkit recovers from that. Hence, don't
    // print out a warning in that case.
    if (error.startsWith("Provider not present"))
        contextDebug() << error;
    else
        // but do print out the interesting warnings
        contextWarning() << error;
    // TODO: startsWith is ugly, but we don't have error numbers to make this
    // more elegant.

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
        Q_EMIT subscribeFinished(this, key);
}

/// Forwards the call to \c signalSubscribeFinished.
void Provider::onPluginSubscribeFinished(QString key, TimedValue value)
{
    signalSubscribeFinished(key);
    onPluginValueChanged(key, value);
}

/// Deprecated.
void Provider::onPluginSubscribeFinished(QString key)
{
    contextDebug() << key;

    signalSubscribeFinished(key);
}

/// Forwards the call to \c signalSubscribeFinished, after logging a
/// warning.
void Provider::onPluginSubscribeFailed(QString key, QString error)
{
    // Don't print a warning here; under some circumstances it is normal that
    // the provider is not running (e.g., during boot, subscribers might start
    // before providers).  When the provider really starts, contextkit recovers
    // from the situation gracefully.
    contextDebug() << key << error;

    signalSubscribeFinished(key);
}

/// Schedules a property to be subscribed to.  Returns true if and
/// only if the main loop has to run for the subscription to be
/// finalized.
bool Provider::subscribe(const QString &key)
{
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
            Q_FOREACH (QString key, toSubscribe)
                Q_EMIT subscribeFinished(this, key);
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
void Provider::onPluginValueChanged(QString key, TimedValue newValue)
{
    QMutexLocker lock(&subscribeLock);
    if (subscribedKeys.contains(key)) {
        // FIXME: try out if everything works with lock.unlock() here
        values.insert(key, newValue);
        Q_EMIT valueChanged(key);
    }
    else
        // Plugins are allowed to send values which are not subscribed to, but
        // only if they get them for free.
        contextDebug() << "Received a property not subscribed to:" << key;
}

/// Deprecated: plugins should use the variant taking a TimedValue.
/// Forwards the \c newValue for \c key received from the plugin to
/// the upper layers via \c HandleSignalRouter.
void Provider::onPluginValueChanged(QString key, QVariant newValue)
{
    QMutexLocker lock(&subscribeLock);
    if (subscribedKeys.contains(key)) {
        // FIXME: try out if everything works with lock.unlock() here
        values.insert(key, TimedValue(newValue));
        Q_EMIT valueChanged(key);
    }
    else
        // Plugins are allowed to send values which are not subscribed to, but
        // only if they get them for free.
        contextDebug() << "Received a property not subscribed to:" << key;
}

void Provider::blockUntilSubscribed(const QString& key)
{
    // This function might be called before the plugin is constructed (since
    // it's constructed in a queued way).  If so, construct it now.
    constructPlugin();

    if (plugin == 0) // we couldn't construct a plugin
    {
        return;
    }

    // Maybe the plugin hasn't had time to emit ready() yet.  Force the plugin
    // to be ready, then.

    // When this is called, the plugin waits until it's ready, and emits the
    // ready() signal (the connection is not queued).  As a result, we
    // handleSubscribes() and that calls subscribe().  Or, the plugin emits
    // failed(), we handleSubscribes(), and don't call anything.
    plugin->blockUntilReady();

    // Force handleSubscribes().  If the plugin emits ready() and it was already
    // ready, onPluginReady ignores it.  That's correct behaviour, since we
    // don't want to renew the subscriptions ( toSubscribe += subscribedKeys ).
    // But we want to tell the provider to subscribe immediately, otherwise only
    // the Provider knows that this key should be subscribed, but Plugin doesn't
    // know it yet.
    handleSubscribes();

    if (pluginState == READY) {
        // And tell the plugin to block until the subscription of this key is
        // complete.
        plugin->blockUntilSubscribed(key);
    }
}

TimedValue Provider::get(const QString &key) const
{
    return values.value(key, TimedValue(QVariant()));
}

/// Returns a singleton for the named \c plugin with the \c constructionString.
Provider* Provider::instance(const ContextProviderInfo& providerInfo)
{
    // Singleton instance container
    // plugin path, constructionstring -> Provider
    static QMap<ContextProviderInfo, Provider*> providerInstances;

    static QMutex providerInstancesLock;
    QMutexLocker locker(&providerInstancesLock);
    if (!providerInstances.contains(providerInfo))
        providerInstances.insert(providerInfo, new Provider(providerInfo));

    contextDebug() << "Returning provider instance for" << providerInfo.plugin
                   << ":" << providerInfo.constructionString;

    return providerInstances[providerInfo];
}

} // end namespace
