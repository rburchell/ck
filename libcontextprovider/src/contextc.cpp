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
#include "service.h"
#include "property.h"
#include "group.h"
#include "logging.h"
#include "sconnect.h"
#include "listeners.h"
#include "loggingfeatures.h"

#include <QCoreApplication>

namespace ContextProvider {
}

using namespace ContextProvider;

/*!
    \page CApi C Api

    \brief The libcontextprovider library offers a simple plain-C API to be used
    from any C program.

    \section Usage

    \code
    context_provider_init(DBUS_BUS_SESSION, "org.test.provider");

    context_provider_install_key("Battery.OnBattery", 0, NULL, NULL);
    context_provider_install_key("Battery.ChargePercentage", 0, NULL, NULL);

    context_provider_set_boolean("Battery.OnBattery", 1);
    context_provider_set_integer("Battery.ChargePercentage", 55);
    \endcode

    The above code snippet starts a service at "org.test.provider" with two
    keys ("Battery.OnBattery", "Battery.ChargePercentage") and sets their
    respective values.

    Note: If the provider used other D-Bus bindings than QDBus, the
    service name ("org.test.provider") needs to be unique, i.e., the
    provider process should not register it itself.

    \section Callbacks

    The context_provider_install_key function and context_provider_install_group function
    take arguments specyfying the callback function. The callback is executed when the
    subscription status changes for the key or the key group (first subscriber
    appears or last subscriber goes away). Basing on this info the provider can
    manage it's resources. It's okay also to use the callback function to actually
    set the value.

    \code
    void callback_function_example(int subscribed, void* user_data)
    {
        if (subscribed) {
            // First subscriber appeared.
            // Start ie. pooling the data.
        } else {
            // Last subscribed disappeared.
            // Stop  pooling data, release resources.
        }
    }
    \endcode
*/

static Service *cService;

QList<Listener*> *listeners = NULL;

char argv[] = "libcontextprovider";
char* p= argv;
int argc = 1;
QCoreApplication* app = 0;

/// Initializes and starts the service with a given \a bus_type and a \a bus_name.
/// The \a bus_type can be DBUS_BUS_SESSION or DBUS_BUS_SYSTEM. This function can be
/// called only once till a matching context_provider_stop is called.
int context_provider_init (DBusBusType bus_type, const char* bus_name)
{
    contextDebug() << F_C << bus_name;

    if (QCoreApplication::instance() == 0) {
        // No QCoreApplication created, so crate one.
        // This will also create an event dispatcher (QEventDispatcherGlibPrivate.)
        app = new QCoreApplication(argc, &p);
    }

    if (cService != NULL) {
        contextCritical() << "Service already initialize. You can only initialize one service with C API";
        return 0;
    }

    cService = new Service(bus_type == DBUS_BUS_SESSION
                           ? QDBusConnection::SessionBus
                           : QDBusConnection::SystemBus,
                           bus_name);
    listeners = new QList<Listener*>;

    return 1;
}

/// Stops the currently started service with context_provider_init. After calling
/// this function a new service can be started by calling context_provider_init.
void context_provider_stop (void)
{
    contextDebug() << F_C;
    if (cService) {
        contextDebug() << "Stopping service";

        // Delete all listeners
        if (listeners) {
            Q_FOREACH (Listener *listener, *listeners)
                delete listener;
        }
        delete listeners; listeners = NULL;

        delete cService;
        cService = NULL;
    }
    // FIXME: Do we need to do something with the QCoreApplication we might have constructed?
}

/// Installs (adds) a \a key to be provided by the service. The callback function \a
/// subscription_changed_cb will be called with the passed user data \a subscription_changed_cb_target
/// when the status of the subscription changes -- when the first subscriber appears or the
/// last subscriber disappears. The \a clear_values_on_subscribe when enabled will automatically
/// clear (set to null/undetermined) the group keys on first subscribe.
void context_provider_install_key (const char* key,
                                   int clear_values_on_subscribe,
                                   ContextProviderSubscriptionChangedCallback subscription_changed_cb,
                                   void* subscription_changed_cb_target)
{
    contextDebug() << F_C << key;

    if (! cService) {
        contextCritical() << "Can't install key:" << key << "because no service started.";
        return;
    }

    listeners->append(new PropertyListener(*cService, key,
                                           clear_values_on_subscribe,
                                           subscription_changed_cb, subscription_changed_cb_target));
    cService->restart();
}

/// Installs (adds) a \a key_group to be provided by the service. The \a key_group is a NULL-terminated
/// array containing the keys. The callback function \a subscription_changed_cb will be called with the
/// passed user data \a subscription_changed_cb_target when the status of the subscription changes --
/// when the first subscriber appears or the last subscriber disappears. The \a clear_values_on_subscribe
/// when enabled will automatically clear (set to null/undetermined) the group keys on first subscribe.
void context_provider_install_group (char* const * key_group,
                                     int clear_values_on_subscribe,
                                     ContextProviderSubscriptionChangedCallback subscription_changed_cb,
                                     void* subscription_changed_cb_target)
{
    contextDebug() << F_C;

    if (! cService) {
        contextCritical() << "Can't install key group because no service started.";
        return;
    }

    QStringList keys;
    int i = 0;
    while(key_group[i] != NULL) {
        QString key = QString(key_group[i]);

        keys << key;
        i++;
    }

    listeners->append(new GroupListener(*cService, keys,
                                        clear_values_on_subscribe,
                                        subscription_changed_cb, subscription_changed_cb_target));
    cService->restart();
}

/// Sets the \a key to a specified integer \a value.
void context_provider_set_integer (const char* key, int value)
{
    contextDebug() << F_C << key << value;
    if (cService)
        cService->setValue(key, value);
}

/// Sets the \a key to a specified double \a value.
void context_provider_set_double (const char* key, double value)
{
    contextDebug() << F_C << key << value;
    if (cService)
        cService->setValue(key, value);
}

/// Sets the \a key to a specified boolean \a value.
void context_provider_set_boolean (const char* key, int value)
{
    contextDebug() << F_C << key << value;
    if (cService)
        cService->setValue(key, (value != 0));
}

/// Sets the \a key to a specified string \a value.
void context_provider_set_string (const char* key, const char* value)
{
    contextDebug() << F_C << key << value;
    if (cService)
        cService->setValue(key, value);
}

/// Sets the \a key to NULL. In other words - unsets the key.
void context_provider_set_null (const char* key)
{
    contextDebug() << F_C << key;
    if (cService)
        cService->setValue(key, QVariant());
}

/// Sets the value of \a key to the specified \a map.  If \a free_map is TRUE,
/// frees the map, which becomes invalid afterwards.
///
/// \sa context_provider_map_new, context_provider_map_set_*
void context_provider_set_map (const char* key, void* map, int free_map)
{
    QVariantMap *qvm = reinterpret_cast<QVariantMap *>(map);
    cService->setValue(key, QVariant(*qvm));
    if (free_map)
        delete qvm;
}

/// Creates an opaque map for use with the context_provider_map_set_* family
/// of functions.  Free it with context_provider_map_free().
void *context_provider_map_new ()
{
    return new QVariantMap();
}

/// Free the \a map created by context_provider_map_new().
void context_provider_map_free (void* map)
{
    delete reinterpret_cast<QVariantMap *>(map);
}

/// Sets \a key to the integer \a value in \a map.
void context_provider_map_set_integer(void* map, const char* key, int value)
{
    QVariantMap *qvm = reinterpret_cast<QVariantMap *>(map);
    qvm->insert(key, QVariant(value));
}

/// Sets \a key to the double \a value in \a map.
void context_provider_map_set_double (void* map, const char* key, double value)
{
    QVariantMap *qvm = reinterpret_cast<QVariantMap *>(map);
    qvm->insert(key, QVariant(value));
}

/// Sets \a key to the boolean \a value in \a map.
void context_provider_map_set_boolean(void* map, const char* key, int value)
{
    QVariantMap *qvm = reinterpret_cast<QVariantMap *>(map);
    qvm->insert(key, QVariant(value != 0));
}

/// Sets \a key to the string \a value in \a map.
void context_provider_map_set_string (void* map, const char* key, const char* value)
{
    QVariantMap *qvm = reinterpret_cast<QVariantMap *>(map);
    qvm->insert(key, QVariant(value));
}

/// Sets \a key to the map \a value in \a map.  NOTE: \a value is freed, and
/// becomes invalid after this call.
void context_provider_map_set_map (void* map, const char* key, void* value)
{
    QVariantMap *qvm = reinterpret_cast<QVariantMap *>(map);
    QVariantMap *other = reinterpret_cast<QVariantMap *>(value);
    qvm->insert(key, QVariant(*other));
    delete other;
}

/// Sets \a key to the list \a value in \a map.  NOTE: \a value is freed, and
/// becomes invalid after this call.
void context_provider_map_set_list(void* map, const char* key, void* value)
{
    QVariantMap *qvm = reinterpret_cast<QVariantMap *>(map);
    QVariantList *list = reinterpret_cast<QVariantList *>(value);
    qvm->insert(key, QVariant(*list));
    delete list;
}

/// Sets the value of \a key to the specified \a list.  If \a free_list is
/// TRUE, the list is freed.
///
/// \sa context_provider_list_new, context_provider_list_add_*
void context_provider_set_list(const char* key, void* list, int free_list)
{
    QVariantList *qvl = reinterpret_cast<QVariantList *>(list);
    cService->setValue(key, QVariant(*qvl));
    if (free_list)
        delete qvl;
}

/// Creates an opaque list for use with the context_provider_list_add_* family
/// of functions.  Free it with context_provider_list_free().
void *context_provider_list_new()
{
    return new QVariantList();
}

/// Frees the list created by context_provider_list_new().  Use
/// context_provider_list_free() to free it.
void context_provider_list_free(void* list)
{
    delete reinterpret_cast<QVariantList *>(list);
}

/// Appends the integer \a value to \a list.
void context_provider_list_add_integer(void* list, int value)
{
    QVariantList *qvl = reinterpret_cast<QVariantList *>(list);
    qvl->append(QVariant(value));
}

/// Appends the double \a value to \a list.
void context_provider_list_add_double(void* list, double value)
{
    QVariantList *qvl = reinterpret_cast<QVariantList *>(list);
    qvl->append(QVariant(value));
}

/// Appends the boolean \a value to \a list.
void context_provider_list_add_boolean(void* list, int value)
{
    QVariantList *qvl = reinterpret_cast<QVariantList *>(list);
    qvl->append(QVariant(value != 0));
}

/// Appends the string \a value to \a list.
void context_provider_list_add_string(void* list, const char* value)
{
    QVariantList *qvl = reinterpret_cast<QVariantList *>(list);
    qvl->append(QVariant(value));
}

/// Appends the specified map (\a value) to \a list.  NOTE: \a value is freed
/// and becomes invalid after this call.
void context_provider_list_add_map(void* list, void* value)
{
    QVariantList *qvl = reinterpret_cast<QVariantList *>(list);
    QVariantMap *qvm = reinterpret_cast<QVariantMap *>(value);
    qvl->append(QVariant(*qvm));
    delete qvm;
}

/// Appends the specified list \a value to \a list.  NOTE: \a value is freed,
/// and becomes invalid after this call.
void context_provider_list_add_list(void* list, void* value)
{
    QVariantList *qvl = reinterpret_cast<QVariantList *>(list);
    QVariantList *vl = reinterpret_cast<QVariantList *>(value);
    qvl->append(QVariant(*vl));
    delete vl;
}
