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

/*!
  \class SubscriberSignallingInterface

  \brief Proxy class for the DBus interface
  org.freedesktop.ContextKit.Subscriber which connects automatically
  to the Changed signal over DBus.

  It has to be a separate class because it needs the connection at
  initialization time, and we want to pass only the bus type which
  will be used to create the connection _after_ initialization time.
*/

/*!
  \class SubscriberInterface

  \brief Proxy class for using the DBus interface
  org.freedesktop.ContextKit.Subscriber asynchronously.

  Implements methods for constructing the interface objects (given the
  DBus type, session or system, and bus name), calling the functions
  Subscribe and Unsubscribe asynchronously, and listening to the
  Changed signal.
*/

#include "subscriberinterface.h"
#include "safedbuspendingcallwatcher.h"
#include "sconnect.h"
#include "logging.h"
#include "contextkitplugin.h"
#include <QDebug>
#include <QDBusConnection>
#include <QDBusPendingReply>

namespace ContextSubscriber {

const char *SubscriberInterface::interfaceName = "org.freedesktop.ContextKit.Subscriber";

/// Constructs the SubscriberInterface. Connects to the DBus object specified
/// by \a busType (session or system bus), \a busName and \a objectPath.
SubscriberInterface::SubscriberInterface(const QDBusConnection connection,
                                         const QString &busName,
                                         const QString &objectPath,
                                         QObject *parent)
    : QDBusAbstractInterface(busName, objectPath, interfaceName, connection, parent)
{
    sconnect(this, SIGNAL(Changed(const QMap<QString, QVariant>&, const QStringList &)),
             this, SLOT(onChanged(const QMap<QString, QVariant>&, const QStringList &)));
}

/// Calls the Subscribe function over DBus asynchronously.
void SubscriberInterface::subscribe(QSet<QString> keys)
{
    contextDebug() << "SubscriberInterface::subscribe";
    // FIXME contextDebug() << "SubscriberInterface::subscribe " << keys;
    if (isValid() == false || keys.size() == 0) {
        contextDebug() << "Subscriber cannot subscribe -> emitting subscribeFinished()";
        Q_EMIT subscribeFinished(keys.toList());
        return;
    }

    // Construct the asynchronous call
    QStringList keyList = keys.toList();

    QDBusPendingCall subscribeCall = asyncCall("Subscribe", keyList);
    SafeDBusPendingCallWatcher *watcher = new SafeDBusPendingCallWatcher(subscribeCall, this);
    sconnect(watcher, SIGNAL(finished(QDBusPendingCallWatcher *)),
             this, SLOT(onSubscribeFinished(QDBusPendingCallWatcher *)));

    // When the pending call returns, we need to know the keys we
    // tried to subscribe to.
    watcher->setProperty("keysToSubscribe", keyList);
}

/// Calls the Unsubscribe function over DBus asynchronously.
void SubscriberInterface::unsubscribe(QSet<QString> keys)
{
    if (isValid() && keys.size() != 0) {
        // Construct the asynchronous call
        QStringList keyList = keys.toList();

        asyncCall("Unsubscribe", keyList);
        // The possible errors are not tracked, because we can't do anything if Unsubscribe fails.
    }
}

/// Processes the results of the Changed signal which comes over DBus.
void SubscriberInterface::onChanged(const QMap<QString, QVariant> &values, const QStringList& unknownKeys)
{
    QMapIterator<QString, QVariant> it(values);
    QMap<QString, QVariant> copy;
    while (it.hasNext()) {
        it.next();
        copy.insert(it.key(), demarshallValue(it.value()));
    }
    Q_EMIT valuesChanged(mergeNullsWithMap(copy, unknownKeys));
}

/// A helper function. Sets the values of given keys to a null QVariant in a QMap.
QMap<QString, QVariant>& SubscriberInterface::mergeNullsWithMap(QMap<QString, QVariant> &map, QStringList nulls) const
{
    Q_FOREACH (QString null, nulls) {
        if (map.contains(null))
            contextWarning() << "Provider error, provided unknown and a value for" << null;
        else
            map[null] = QVariant();
    }
    return map;
}

/// Is called when the asynchronous DBus call to Subscribe has finished. Emits
/// the signal valuesChanged with the return values of the subscribed keys.
void SubscriberInterface::onSubscribeFinished(QDBusPendingCallWatcher* watcher)
{
    QDBusPendingReply<QMap<QString, QVariant>, QStringList> reply = *watcher;

    QList<QString> requestedKeys = watcher->property("keysToSubscribe").toStringList();

    if (reply.isError()) {
        // Possible causes of the error:
        // The provider is not running
        // The provider didn't implement the needed interface + function
        // The function resulted in an error
        contextWarning() << "Provider error while subscribing:" << reply.error().message();
        Q_EMIT subscribeFailed(requestedKeys, "Provider error");
    } else {
        QMap<QString, QVariant> subscribeTimeValues = reply.argumentAt<0>();
        QStringList unknowns = reply.argumentAt<1>();

        // TODO: the protocol should be better, this is just a workaround
        QMap<QString, QVariant> okMap = mergeNullsWithMap(subscribeTimeValues, unknowns);
        Q_EMIT subscribeFinished(okMap.keys());
        Q_EMIT valuesChanged(okMap);
        Q_EMIT subscribeFailed((requestedKeys.toSet() - okMap.keys().toSet()).toList(), "Not provided");
    }
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
void SubscriberInterface::connectNotify(const char *signal)
{
    // only Changed signal should be AddMatch'd on the DBus side
    if (qstrcmp(signal, SIGNAL(Changed(QMap<QString,QVariant>,QStringList))) == 0)
        QDBusAbstractInterface::connectNotify(signal);
    else
        QObject::connectNotify(signal);
}
#else
void SubscriberInterface::connectNotify(const QMetaMethod &signal)
{
    // only Changed signal should be AddMatch'd on the DBus side
    if (qstrcmp(signal.methodSignature(), SIGNAL(Changed(QMap<QString,QVariant>,QStringList))) == 0)
        QDBusAbstractInterface::connectNotify(signal);
    else
        QObject::connectNotify(signal);
}
#endif

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
void SubscriberInterface::disconnectNotify(const char *signal)
{
    // only Changed signal should be AddMatch'd on the DBus side
    if (qstrcmp(signal, SIGNAL(Changed(QMap<QString,QVariant>,QStringList))) == 0)
        QDBusAbstractInterface::disconnectNotify(signal);
    else
        QObject::disconnectNotify(signal);
}
#else
void SubscriberInterface::disconnectNotify(const QMetaMethod &signal)
{
    // only Changed signal should be AddMatch'd on the DBus side
    if (qstrcmp(signal.methodSignature(), SIGNAL(Changed(QMap<QString,QVariant>,QStringList))) == 0)
        QDBusAbstractInterface::disconnectNotify(signal);
    else
        QObject::disconnectNotify(signal);
}
#endif

} // end namespace
