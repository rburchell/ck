/*
 * Copyright (C) 2008, 2009 Nokia Corporation.
 *
 * Contact: Marius Vollmer <marius.vollmer@nokia.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include <QDebug>

#include "subscriber.h"
#include "logging.h"
#include "manager.h"
#include "sconnect.h"
#include "loggingfeatures.h"

/*!
    \class Subscriber
    \brief Subscriber implements the org.freedesktop.ContextKit.Subscriber DBus
    interface with help of the SubsciberAdaptor.

    Subscriber lets the user to suscribe to or unsubscribe from properties and 
    get informed when their values change. 
*/

/// Constructor. Creates a new Subscriber with the given \a dbusPath object path and
/// a parent Manager \a man. The Subscriber will use the Manager to obtain values etc.
Subscriber::Subscriber(const QString& dbusPath, Manager *man) : QueuedInvoker(), path(dbusPath), manager(man)
{
    contextDebug() << F_SUBSCRIBER << "New subscriber object created with path" << dbusPath;
    sconnect(man, SIGNAL(keyValueChanged(const QString&, const QVariant&)),
             this, SLOT(onManagerKeyValueChanged(const QString&)));
}

/// Destroys the subscriber. Decreases the subscription count on the parent Manager
/// before going down.
Subscriber::~Subscriber()
{
    contextDebug() << F_SUBSCRIBER << F_DESTROY << "Subscriber with path" << path << "destroyed, decreasing subscription counts";
    // Decrease all the subscription counts
    foreach (QString key, subscribedKeys)
        manager->decreaseSubscriptionCount(key);
}

/// For the given list of \a keys, creates a map and undetermined list using the
/// keys values from the parent Manager.
void Subscriber::buildMapAndUndetermined(const QStringList &keys, QMap<QString, QVariant> &map, QStringList &undetermined)
{
    foreach (QString key, keys) {
        QVariant v = manager->getKeyValue(key);
        if (v.isValid())
            map.insert(key, v);
        else if (! undetermined.contains(key))
            undetermined.append(key);
    }
}

/// Subscribes to te specified list of \a keys. The keys for which values could not
/// be determined are placed in the \c undetermined list. Otherwise the current values are
/// placed in the returned map in a key -> value order.
QMap<QString, QVariant> Subscriber::subscribe(const QStringList &keys, QStringList &undetermined)
{
    contextDebug() << F_SUBSCRIBER << F_DBUS << "Subscribe called with" << keys.size() << "keys";

    QStringList newKeys;
    foreach(QString key, keys) {
        if (! manager->keyIsValid(key)) 
            contextWarning() << "Key:" << key << "is not a valid key";
        else if (! subscribedKeys.contains(key) && ! newKeys.contains(key)) {
            contextDebug() << F_SUBSCRIBER << "Key:" << key << "not yet subscribed, adding to new keys to subscribe";
            newKeys.append(key);
            manager->increaseSubscriptionCount(key);
        } else 
            contextWarning() << "Trying to subscribe to key:" << key << "but it's already subscribed";
    }

    subscribedKeys += newKeys;
    
    QMap<QString, QVariant> map;
    buildMapAndUndetermined(keys, map, undetermined);
    
    return map;
}

/// Unsubscribes from the passed list of keys.
void Subscriber::unsubscribe(const QStringList &keys)
{
    contextDebug() << F_SUBSCRIBER << F_DBUS << "Unsubscribe called with" << keys.size() << "keys";

    QStringList removedKeys;
    foreach(QString key, keys) {
        if (! subscribedKeys.contains(key)) 
            contextWarning() << "Trying to unsubscribe from key:" << key << "but key is not subscribed";
        else 
            removedKeys.append(key);
    }

    foreach (QString key, removedKeys) {
        contextDebug() << F_SUBSCRIBER << "Unsubscribing from previously subscribed key:" << key;
        subscribedKeys.removeOne(key);
        manager->decreaseSubscriptionCount(key);
    }
}

/// Returns the dbus object path for this Subscriber.
QString Subscriber::dbusPath() const
{
    return path;
}

/// Returns true if the Subscriber is currently subscribed to the specified key.
bool Subscriber::isSubscribedToKey(const QString &key)
{
    return (subscribedKeys.contains(key));
}

/// This is executed when on the parent Manager key changes. If the key concerns us
/// (we're subscribed to it) we schedule a handling operation to be executed in the next
/// mainloop run.
void Subscriber::onManagerKeyValueChanged(const QString &key)
{
    if (! subscribedKeys.contains(key)) {
        contextDebug() << F_SUBSCRIBER << F_SIGNALS << key << "value changed, ignoring (not subscribed)";
        return;
    }
    
    contextDebug() << F_SUBSCRIBER << F_SIGNALS << "subscribed key:" << key << "changed, will handled the change in a sec";
    changedKeys.append(key);
    queueOnce("handleChanged");
}

/// This is executed via QueuedInvoker. We emit a Subscriber::changed signal with the new
/// values for the changed properties fetched from parent Manager.
void Subscriber::handleChanged()
{
    contextDebug() << F_SUBSCRIBER << "Handling the changed keys etc.";
    
    QStringList undetermined;
    QMap<QString, QVariant> map;
    
    buildMapAndUndetermined(changedKeys, map, undetermined);    
    emit changed(map, undetermined);
    changedKeys.clear();
}
