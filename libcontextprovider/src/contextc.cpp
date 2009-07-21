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

QHash<QString, QStringList*> busNameToKeyList;
QHash<QString, QDBusConnection::BusType> busNameToBusType;

/// Initializes the service for the given \a name. \a is_system 
/// (1 or 0) specifies if this is a system service (system bus or session bus). The \a keys
/// is a pointer to an array of pointers to NULL-terminated strings with key names. 
/// \a key_count specifies the number of items in the \a keys array. 
/// Returns 1 on success, 0 otherwise. This function internally calls Context::initService.
int context_init_service (int is_system, const char *name, const char** keys, int key_count)
{
    // Build the key string list
    QStringList keyList;
    int i;
    for (i = 0; i < key_count; i++)
        keyList.append(QString(keys[i]));

    QDBusConnection::BusType busType = (is_system) ? QDBusConnection::SystemBus : QDBusConnection::SessionBus;
    return Context::initService(busType, QString(name), keyList);
}

/// Creates a new ContextPtr handle that can be used to set/get values. \a name
/// is a NULL-terminated key name. Returns the new allocated ContextPtr. Will never
/// return NULL. You're supposed to destroy the ContextPtr with context_free. 
ContextPtr* context_new (const char *name)
{
    Context *c = new Context(name);
    return (ContextPtr*) c;
}

/// Frees the provided \a ptr. Note that destroying the ContextPtr will not destroy
/// the key itself. It's just the handle that's being destroyed.
void context_free (ContextPtr *ptr)
{
    delete ((Context*) ptr);
}

/// Returns the key that this context property \a ptr represents. 
/// The caller is responsible for freeing the returned string when 
/// it's not needed anymore.
char* context_get_key (ContextPtr *ptr)
{
    QString key = ((Context*) ptr)->getKey();
    return qstrdup(key.toUtf8().constData());
}

/// Returns 1 if the context property referenced by \a ptr is valid.
int context_is_valid (ContextPtr *ptr)
{
    return ((Context*) ptr)->isValid();
}

/// Returns 1 if the context property referenced by \a ptr is set (it's not undetermined).
int context_is_set (ContextPtr *ptr)
{
    return ((Context*) ptr)->isSet();
}

/// Unsets the context property referenced by \a ptr. The property value
/// becomes undeterined.
void context_unset (ContextPtr *ptr)
{
    ((Context*) ptr)->unset();
}

/// Sets the context property referenced by \a ptr to a new integer value \a v.
/// Calls Context::set internally. 
void context_set_int(ContextPtr *ptr, int v)
{
    ((Context*) ptr)->set(QVariant(v));
}

/// Sets the context property referenced by \a ptr to a new bool value \a v.
/// Calls Context::set internally. 
void context_set_bool(ContextPtr *ptr, int v)
{
    ((Context*) ptr)->set(QVariant((bool) v));
}

/// Sets the context property referenced by \a ptr to a new double value \a v.
/// Calls Context::set internally. 
void context_set_double(ContextPtr *ptr, double v)
{
    ((Context*) ptr)->set(QVariant(v));
}

/// Sets the context property referenced by \a ptr to a new string value \a v.
/// Calls Context::set internally. 
void context_set_string(ContextPtr *ptr, const char *v)
{
    ((Context*) ptr)->set(QVariant(QString(v)));
}

/// Obtains the integer value of the context property described by the \a ptr. 
/// Returns 1 if the property retrieval was succesful. 0 when property 
/// value is \b Undetermined. When retrieval was succesful, \a v is set 
/// to a new integer value.
int context_get_int (ContextPtr *ptr, int *v)
{
    QVariant var = ((Context*) ptr)->get();
    if (var.isValid()) {
        *v = var.toInt();
        return 1;
    } else {
        *v = 0;
        return 0;
    }
}

/// Obtains the string value of the context property described by the \a ptr. 
/// Returns 1 if the property retrieval was succesful. 0 when property 
/// value is \b Undetermined. When retrieval was succesful, \a v is set 
/// to a point to a new NULL-terminated string. You have to \b destroy this string
/// when done dealing with it.
int context_get_string (ContextPtr *ptr, char **v)
{
    QVariant var = ((Context*) ptr)->get();
    if (var.isValid()) {
        QString str = var.toString();
        *v = qstrdup(str.toUtf8().constData());
    return 1;
    } else {
        *v = 0;
        return 0;
    }
}

/// Obtains the bool value of the context property described by the \a ptr. 
/// Returns 1 if the property retrieval was succesful. 0 when property 
/// value is \b Undetermined. When retrieval was succesful, \a v is set 
/// to a new bool (1, 0) value.
int context_get_bool (ContextPtr *ptr, int *v)
{
    QVariant var = ((Context*) ptr)->get();
    if (var.isValid()) {
        *v = var.toBool();
        return 1;
    } else {
        *v = 0;
        return 0;
    }
}

/// Obtains the double value of the context property described by the \a ptr. 
/// Returns 1 if the property retrieval was succesful. 0 when property 
/// value is \b Undetermined. When retrieval was succesful, \a v is set 
/// to a new double value.
int context_get_double (ContextPtr *ptr, double *v)
{
    QVariant var = ((Context*) ptr)->get();
    if (var.isValid()) {
        *v = var.toDouble();
        return 1;
    } else {
        *v = 0.0;
        return 0;
    }    
}

static int reinitialize_service(const QString &busName)
{
    QStringList *keys = busNameToKeyList.value(busName);
    QDBusConnection::BusType busType = busNameToBusType.value(busName);

    if (keys->length() > 0) 
        Context::stopService(busName);

    return Context::initService(busType, busName, *keys);
}

int context_provider_init (DBusBusType bus_type, const char* bus_name)
{
    if (busNameToKeyList.contains(QString(bus_name))) {
        contextCritical() << "Service with name" << bus_name << "already initialized!";
        return 0;
    }

    QDBusConnection::BusType busType = (bus_type == DBUS_BUS_SESSION) ? 
                                        QDBusConnection::SessionBus   :
                                        QDBusConnection::SystemBus;
    QString busName = QString(bus_name);

    busNameToKeyList.insert(busName, new QStringList());
    busNameToBusType.insert(busName, busType);
    return reinitialize_service(busName);
}

void context_provider_stop (void)
{
    contextDebug() << "Stopping all services";

    foreach(QString key, busNameToKeyList.keys()) {
        Context::stopService(key);
    }

    busNameToKeyList.clear();
    busNameToBusType.clear();
}

