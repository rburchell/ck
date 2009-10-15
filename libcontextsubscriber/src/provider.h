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

#ifndef PROVIDER_H
#define PROVIDER_H

#include "queuedinvoker.h"
#include "contextproviderinfo.h"
#include "timedvalue.h"

#include <QObject>
#include <QDBusConnection>
#include <QSet>
#include <QMutex>

class ContextPropertyInfo;

namespace ContextSubscriber {

class PropertyHandle;
class SubscriberInterface;
class DBusNameListener;
class ManagerInterface;
class IProviderPlugin;

class Provider : public QueuedInvoker
{
    Q_OBJECT

public:
    static Provider* instance(const ContextProviderInfo& providerInfo);
    bool subscribe(const QString &key);
    void unsubscribe(const QString &key);
    TimedValue get(const QString &key) const;

signals:
    void subscribeFinished(Provider *provider, QString key);
    void valueChanged(QString key);

private slots:
    void onPluginReady();
    void onPluginFailed(QString error);
    void onPluginSubscribeFinished(QString key);
    void onPluginSubscribeFailed(QString failedKey, QString error);
    void onPluginValueChanged(QString key, QVariant newValue);

private:
    enum PluginState { INITIALIZING, READY, FAILED };
    Provider(const ContextProviderInfo& providerInfo);
    Q_INVOKABLE void handleSubscribes();
    Q_INVOKABLE void constructPlugin();
    void signalSubscribeFinished(QString key);

    IProviderPlugin* plugin; ///< Plugin instance communicating with the concrete provider.
    PluginState pluginState;
    ContextProviderInfo providerInfo;  ///< Parameters used to initialize the plugin.

    QMutex subscribeLock;
    QSet<QString> toSubscribe; ///< Keys pending for subscription
    QSet<QString> toUnsubscribe; ///< Keys pending for unsubscription

    // FIXME: rename this to something which contains the word intention in it
    QSet<QString> subscribedKeys; ///< The keys that should be currently subscribed to

    QMap<QString, TimedValue> values; ///< A cache of values already received from the plugin
};

} // end namespace

#endif
