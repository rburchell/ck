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

#include "context.h"
#include "logging.h"
#include "manager.h"
#include "manageradaptor.h"
#include "sconnect.h"
#include "loggingfeatures.h"

/*!
    \class Context

    \brief The main class used to provide context data. Typically, to provide
    some keys you would:

    \code
    QStringList keys;
    keys.append("Some.Key1");
    keys.append("Some.Key2");

    Context::initService(QDBusConnection::SessionBus, "org.test.somename", keys);
    Context someKey1("Some.Key1");
    Context someKey2("Some.Key2");
    // Do something with someKey1, someKey2...
    \endcode

    \c Context::initService initializes a new bus connection with a specified service name
    and creates a Manager object on it. The clients obtain Subscriber objects through this
    Manager. 

    For each service there is one Manager object.
*/

QHash<QString, Manager*> Context::busesToManagers;
QHash<QString, Manager*> Context::keysToManagers;

/// Constructor. This creates a new context property that should
/// be used to provide/set values and get notified about subscribers appearing.
Context::Context(const QString &k, QObject* parent)
    : QObject(parent), key(k)
{
    contextDebug() << F_CONTEXT << "Creating new Context for key:" << key;

    manager = keysToManagers.value(key);
    if (manager == NULL) {
        contextCritical() << key << "is currently not provided by any service";
        return;
    }

    sconnect(manager, SIGNAL(firstSubscriberAppeared(const QString&)),
             this, SLOT(onManagerFirstSubscriberAppeared(const QString&)));

    sconnect(manager, SIGNAL(lastSubscriberDisappeared(const QString&)),
             this, SLOT(onManagerLastSubscriberDisappeared(const QString&)));
}

/// Checks if a Context is valid (can be set/get/manipulated), prints an error message if not.
/// Returns true if the Context is valid. False otherwise. Helper for the Context::set 
/// and Context::get familly of functions.
bool Context::keyCheck() const
{
    if (manager == NULL) {
        contextWarning() << "Trying to manipulate an invalid key:" << key;
        return false;
    } else
        return true;
}

/// Returns true if the key is valid.
bool Context::isValid() const
{
    return (manager != NULL);
}

/// Returns true if the key is set (it's value is determined).
bool Context::isSet() const
{
    if (! keyCheck())
        return false;
        
    return (manager->getKeyValue(key) != QVariant());
}

/// Returns the name of the key this Context represents.
QString Context::getKey() const
{
    return key;
}

/// Unsets the key value. The key value becomes undetermined.
void Context::unset()
{
    if (! keyCheck())
        return;

    manager->setKeyValue(key, QVariant());
}

/// Sets the key value to boolean \a v.
void Context::set(bool v)
{
    if (! keyCheck())
        return;
        
    manager->setKeyValue(key, QVariant(v));
}

/// Sets the key value to integer \a v.
void Context::set(int v)
{
    if (! keyCheck())
        return;
        
    manager->setKeyValue(key, QVariant(v));
}

/// Sets the key value to double \a v.
void Context::set(double v)
{
    if (! keyCheck())
        return;
        
    manager->setKeyValue(key, QVariant(v));
}

/// Sets the key value to QString \a v.
void Context::set(const QString &v)
{
    if (! keyCheck())
        return;
        
    manager->setKeyValue(key, QVariant(v));
   
}
    
/// Sets the key value to QVariant \a v.
void Context::set(const QVariant &v)
{
    if (! keyCheck())
        return;
        
    manager->setKeyValue(key, v);
}

/// Returns the current value of the key. The returned QVariant is invalid 
/// if the key value is undetermined or the Context is invalid.
QVariant Context::get()
{
    if (! keyCheck())
        return QVariant();
        
    return manager->getKeyValue(key);
}

/// Called by Manager when first subscriber appears. Delegated if 
/// this concerns us.
void Context::onManagerFirstSubscriberAppeared(const QString &key)
{
    if (key == this->key) {
        contextDebug() << F_SIGNALS << F_CONTEXT << "First subscriber appeared for key:" << key;
        emit firstSubscriberAppeared(key);
    }
}

/// Called by Manager when last subscriber disappears. Delegate if
/// this concerns us.
void Context::onManagerLastSubscriberDisappeared(const QString &key)
{
    if (key == this->key) {
        contextDebug() << F_SIGNALS << F_CONTEXT << "Last subscriber disappeared for key:" << key;
        emit lastSubscriberDisappeared(key);
    }
}

/// Destructor.
Context::~Context()
{
    contextDebug() << F_CONTEXT << F_DESTROY << "Destroying Context for key:" << key;
}

/// Initialize a new service on a bus \a busType with service name \a busName and a given
/// set of \a keys. This is the main method used to setup/bootstrap the keys that 
/// we want to provide. 
/// Returns true if service was registered, false otherwise.
bool Context::initService(QDBusConnection::BusType busType, const QString &busName, const QStringList &keys)
{
    contextDebug() << F_CONTEXT << "Initializing service for bus:" << busName;

    // Basic sanity check
    if (busesToManagers.contains(busName)) {
        contextCritical() << busName << "service is already registered";
        return false;
    }
    
    QDBusConnection connection = QDBusConnection::connectToBus(busType, busName);

    Manager *manager = new Manager(keys);
    ManagerAdaptor *managerAdaptor = new ManagerAdaptor(manager, connection);

    // Register service
    if (! connection.registerService(busName)) {
        contextCritical() << "Failed to register service with name" << busName;
        return false;
    }

    // Register object
    if (managerAdaptor && !connection.registerObject("/org/freedesktop/ContextKit/Manager", manager)) {
        contextCritical() << "Failed to register the Manager object for" << busName;
        return false;
    }

    busesToManagers.insert(busName, manager);

    // Add a mapping for all the keys -> manager
    foreach(QString key, keys) {
        if (keysToManagers.contains(key)) {
            contextCritical() << key << "is already provided by another manager/service!";
            return false;
        } else {
            keysToManagers.insert(key, manager);
        }
    }

    return true;
}

