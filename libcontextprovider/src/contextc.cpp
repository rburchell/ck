/*
 * Copyright (C) 2008 Nokia Corporation.
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

#include "contextc.h"
#include "context.h"
#include "logging.h"
#include "sconnect.h"
#include "contextgroup.h"
#include "listeners.h"

using namespace ContextProvider;

QStringList *serviceKeyList = NULL;
QString *serviceBusName = NULL;
QDBusConnection::BusType serviceBusType;
QList<Listener*> *listeners = NULL;

/// Stops and restarts the service with proper key. Not part of the
/// public API.
static int reinitialize_service()
{
    if (serviceKeyList->length() > 0) 
        Context::stopService(*serviceBusName);

    return Context::initService(serviceBusType, *serviceBusName, *serviceKeyList);
}

/// Initializes and starts the service with a given \a bus_type and a \a bus_name.
/// Te \a bus_type can be DBUS_BUS_SESSION or DBUS_BUS_SYSTEM. This function can be 
/// called only once till a matching context_provider_stop is called. 
int context_provider_init (DBusBusType bus_type, const char* bus_name)
{
    if (serviceBusName != NULL) {
    contextCritical() << "Service already initialize. You can only initialize one service with C API";
        return 0;
    }

    serviceBusType = (bus_type == DBUS_BUS_SESSION) ? 
                      QDBusConnection::SessionBus   :
                      QDBusConnection::SystemBus;
    serviceBusName = new QString(bus_name);
    serviceKeyList = new QStringList();
    listeners = new QList<Listener*>;

    return reinitialize_service();
}

/// Stops the currently started service with context_provider_init. After calling
/// this function a new service can be started by calling context_provider_init. 
void context_provider_stop (void)
{
    contextDebug() << "Stopping service";

    if (serviceBusName) {
        Context::stopService(*serviceBusName);
    }

    // Delete all listeners
    if (listeners) {
        foreach (Listener *listener, *listeners)
            delete listener;
    }

    delete serviceBusName; serviceBusName = NULL;
    delete serviceKeyList; serviceKeyList = NULL;
    delete listeners; listeners = NULL;
}

/// Installs (adds) a \a key to be provided by the service. The callback function \a 
/// subscription_changed_cb will be called with the passed user data \a subscription_changed_cb_target
/// when the status of the subscription changes -- when the first subscriber appears or the
/// last subscriber disappears.
void context_provider_install_key (const char* key, 
                                   int clear_values_on_subscribe, 
                                   ContextProviderSubscriptionChangedCallback subscription_changed_cb, 
                                   void* subscription_changed_cb_target)
{
    if (! serviceKeyList) {
        contextCritical() << "Can't install key:" << key << "because no service started.";
        return;
    }
    
    if (serviceKeyList->contains(QString(key))) {
        contextCritical() << "Key:" << key << "is already installed";
        return;
    }

    serviceKeyList->append(key);
    reinitialize_service();

    listeners->append(new ContextListener(key, subscription_changed_cb, subscription_changed_cb_target));
}

/// Installs (adds) a \a key_group to be provided by the service. The \a key_group is a NULL-terminated 
/// array containing the keys. The callback function \a subscription_changed_cb will be called with the 
/// passed user data \a subscription_changed_cb_target when the status of the subscription changes -- 
/// when the first subscriber appears or the last subscriber disappears.
void context_provider_install_group (const char** key_group, 
                                     int clear_values_on_subscribe, 
                                     ContextProviderSubscriptionChangedCallback subscription_changed_cb, 
                                     void* subscription_changed_cb_target)
{
    if (! serviceKeyList) {
        contextCritical() << "Can't install key group because no service started.";
        return;
    }

    QStringList keys;
    int i = 0;
    while(key_group[i] != NULL) {
        QString key = QString(key_group[i]);

        if (serviceKeyList->contains(key)) {
            contextCritical() << "Key:" << key << "is already installed";
            return;
        }

        keys << key;
        serviceKeyList->append(key);
        i++;
    }

    reinitialize_service();
    listeners->append(new ContextGroupListener(keys, subscription_changed_cb, subscription_changed_cb_target));
}

/// Sets the \a key to a specified integer \a value.
void context_provider_set_integer (const char* key, int value)
{
    Context(key).set(value);
}

/// Sets the \a key to a specified double \a value.
void context_provider_set_double (const char* key, double value)
{
    Context(key).set(value);
}

/// Sets the \a key to a specified boolean \a value.
void context_provider_set_boolean (const char* key, int value)
{
    Context(key).set(value);
}

/// Sets the \a key to a specified string \a value.
void context_provider_set_string (const char* key, const char* value)
{
    Context(key).set(value);
}

/// Sets the \a key to NULL. In other words - unsets the key.
void context_provider_set_null (const char* key)
{
    Context(key).unset();
}

