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

#include "manager.h"
#include "logging.h"
#include "subscriber.h"
#include "loggingfeatures.h"

namespace ContextProvider {

/*!
    \class Manager
    \brief Manager implements the org.freedesktop.ContextKit.Manager DBus
    interface. (With the help of ManagerAdaptor.) It creates and manages the
    Subscriber objects. There is one Manager object per each service.

    Manager is responsible of allocating Subscriber objects to clients. It
    ensures that each client gets a unique Subscriber object (even if it executes
    GetSubscriber multiple times). When a client terminates its DBus connection,
    the Manager (through the help of the adapter) deletes the corresponding 
    Subscriber object.

    Each service started gets it's own Manager object that manages the keys
    belonging to the service. The Manager is registered as the 
    org.freedesktop.ContextKit.Manager on the bus belonging to the service.
*/

/// Creates a new Manager object with the given list of \a keys. Those 
/// keys will be managed by this manager. Duplicate keys are ignored with a 
/// warning.
Manager::Manager(const QStringList &keys) : subscriberCounter(0)
{
    contextDebug() << F_MANAGER << "Creating new Manager with" << keys.size() << "keys";

    foreach (QString key, keys) {
        if (keysToValues.contains(key))
            contextWarning() << "Key:" << key << "is already provided, duplicate?";
        else {
            contextDebug() << F_MANAGER << "Adding key:" << key << "to the list of provided keys";
            keysToValues.insert(key, QVariant());
            keysToSubscriptionCount.insert(key, 0);
        }
    }
}

/// Destroys the Manager. Also destroys all the belonging subscriber objects created
/// through getSubscriber().
Manager::~Manager()
{
    contextDebug() << F_MANAGER << F_DESTROY << "Destroying Manager";

    // Delete here because while deleting those subscribers might
    // call us (increase/decrease count).
    foreach (Subscriber *subscriber, busNamesToSubscribers)
        delete subscriber;
}

/// Gives the Subscriber object to a client with a given \a busName.
/// If the client is new, creates a new Subscriber object and gives
/// it to the client. If the client has requested a Subscriber previously,
/// returns the same object.
Subscriber* Manager::getSubscriber(const QString &busName)
{
    contextDebug() << F_MANAGER << "Get subscriber called for bus name:" << busName;

    if (busNamesToSubscribers.contains(busName)) {
        contextDebug() << F_MANAGER << "Subscriber already present for" << busName;
        return busNamesToSubscribers.value(busName);
    }
    
    QString subscriberPath = QString("/org/freedesktop/ContextKit/Subscriber/%1").arg(subscriberCounter);
    contextDebug() << F_MANAGER << F_DBUS << "Creating new subscriber, subscriber path is" << subscriberPath;
    subscriberCounter++;

    Subscriber *subscriber = new Subscriber(subscriberPath, this);
    busNamesToSubscribers.insert(busName, subscriber);

    return subscriber;
}

/// Returns true if the Manager has a Subscriber object with the given \a name.
/// The \a name is the bus name.
bool Manager::hasSubscriberWithBusName(const QString &name) const
{
    return busNamesToSubscribers.contains(name);
}

/// Informs the Manager that the specified bus name is now gone.
/// The manager should remove all the subscribers created with this busname.
void Manager::busNameIsGone(const QString &busName)
{
    if (busNamesToSubscribers.contains(busName)) {
        Subscriber *subscriber = busNamesToSubscribers.value(busName);
        contextDebug() << F_MANAGER << F_DBUS << "Subscriber with bus name:" << busName << "and path:" << subscriber->dbusPath() << "is being removed";
        busNamesToSubscribers.remove(busName);
        delete subscriber;
    }
}

/// Returns true if the given \a key is valid for this Manager. In other words, 
/// returns true if the \a key is managed by this manager.
bool Manager::keyIsValid(const QString &key) const
{
    // The value-list is also our list of valid keys
    return (keysToValues.contains(key));
}

/// Increases the subscription count for the given \a key. The Manager will
/// emit a firstSubscriberAppeared signal if this is the first subscription.
/// This is called by the child Subscriber objects.
void Manager::increaseSubscriptionCount(const QString &key)
{
    if (! keysToSubscriptionCount.contains(key)) {
        contextCritical() << "Can't increase subscription count for non-existent key" << key;
        return;
    }

    int currentCount = keysToSubscriptionCount.value(key);
    currentCount++;
    keysToSubscriptionCount.insert(key, currentCount);

    contextDebug() << F_MANAGER << "Subscription count for" << key << "is now" << currentCount;

    // First subscritpion for this key? Let others know.
    if (currentCount == 1)
        emit firstSubscriberAppeared(key);
}

/// Decreases the subscription count for the given \a key. The Manager will
/// emit a lastSubscriberDisappeared signal if this is the last subscription.
/// This is called by the child Subscriber objects.
void Manager::decreaseSubscriptionCount(const QString &key)
{
    if (! keysToSubscriptionCount.contains(key)) {
        contextCritical() << F_MANAGER << "Can't decrease subscription count for non-existent key" << key;
        return;
    }

    int currentCount = keysToSubscriptionCount.value(key);
    currentCount--;

    if (currentCount < 0) {
        contextWarning() << "Tried setting key:" << key << "subscription count to < 0, should not happen";
        return;
    }

    keysToSubscriptionCount.insert(key, currentCount);

    contextDebug() << F_MANAGER << "Subscription count for" << key << "is now" << currentCount;

    // Last subscritpion for this key? Let others know.
    if (currentCount == 0)
        emit lastSubscriberDisappeared(key);
}

/// Sets the given \a key to a new value \a v. This triggers the emission of
/// the Manager::keyValueChanged signal. 
void Manager::setKeyValue(const QString &key, const QVariant &v)
{
    if (! keysToValues.contains(key)) {
        contextWarning() << "Manager does not contain key" << key << "can't set it's value";
        return;
    }
    
    if (v == keysToValues.value(key)) {
        // Same value, skip
        return;
    }
    
    contextDebug() << F_MANAGER << "Setting key:" << key << "to type:" << v.typeName();
    keysToValues.insert(key, v);
    emit keyValueChanged(key, v);
}

/// Returns the current value of the \a key. The returned QVariant
/// is invalid if the value is undetermined or they key is not valid
/// for this Manager.
QVariant Manager::getKeyValue(const QString &key)
{
    // FIXME Unify with setKeyValue check
    if (! keysToValues.contains(key)) {
        contextWarning() << "Manager does not contain key" << key << "can't get it's value";
        return QVariant();
    }
    
    return QVariant(keysToValues.value(key));
}

/// Returns the current subscription count for the given \a key.
/// Returns -1 if the key could not be found.
int Manager::getSubscriptionCount(const QString &key) const
{
    if (! keysToSubscriptionCount.contains(key)) {
        contextWarning() << "Requested subscription count for non-existent key" << key;
        return -1;
    }

    return keysToSubscriptionCount.value(key);
}

/// Returns the list of keys handled by this Manager.
QStringList Manager::getKeys()
{
    return QStringList(keysToValues.keys());
}

} // namespace ContextProvider
