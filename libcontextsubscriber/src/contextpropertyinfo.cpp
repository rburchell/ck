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

#include "contextpropertyinfo.h"
#include "infobackend.h"
#include "sconnect.h"

/// Constructs a new ContextPropertyInfo for \a key with the given \a parent.
/// The object can be used to perform introspection on the given \a key.
/// \param key The full name of the key.
ContextPropertyInfo::ContextPropertyInfo(const QString &key, QObject *parent)
    : QObject(parent)
{
    keyName = key;

    if (key != "") {
        sconnect(InfoBackend::instance(), SIGNAL(keyDataChanged(QString)),
                 this, SLOT(onKeyDataChanged(QString)));

        cachedType = InfoBackend::instance()->typeForKey(keyName);
        cachedProvider = InfoBackend::instance()->providerForKey(keyName);
        cachedProviderDBusType = InfoBackend::instance()->providerDBusTypeForKey(keyName);
    }
}

/// Returns the full name of the introspected key.
QString ContextPropertyInfo::key() const
{
    return keyName;
}

/// Returns the doc (documentation) for the introspected key.
QString ContextPropertyInfo::doc() const
{
    return InfoBackend::instance()->docForKey(keyName);
}

/// Returns the type name for the introspected key.
QString ContextPropertyInfo::type() const
{
    return cachedType;
}

/// Returns true if the key exists in the registry.
bool ContextPropertyInfo::exists() const
{
    // A key is assumed to exist if it's type != "".
    if (cachedType != "")
        return true;
    else
        return false;
}

/// Returns the dbus name of the provider supplying this property/key.
QString ContextPropertyInfo::providerDBusName() const
{
    return cachedProvider;
}

/// Returns the bus type of the provider supplying this property/key.
/// Ie. if it's a session bus or a system bus. 
QDBusConnection::BusType ContextPropertyInfo::providerDBusType() const
{
    if (cachedProviderDBusType == "session")
        return QDBusConnection::SessionBus;
    else if (cachedProviderDBusType == "system")
        return QDBusConnection::SystemBus;
    else
        return QDBusConnection::SessionBus;
}

/* Slots */

void ContextPropertyInfo::onKeyDataChanged(const QString& key)
{
    if (key != keyName)
        return;

    QString newType = InfoBackend::instance()->typeForKey(keyName);
    if (cachedType != newType) {
    
        if (cachedType == "")
            emit existsChanged(true);
        if (newType == "")
            emit existsChanged(false);
            
        cachedType = newType;
        emit typeChanged(cachedType);
    }

    QString newProvider = InfoBackend::instance()->providerForKey(keyName);
    if (cachedProvider != newProvider) {
        cachedProvider = newProvider;
        emit providerChanged(cachedProvider);
    }

    QString newProviderDBusType = InfoBackend::instance()->providerDBusTypeForKey(keyName);
    if (cachedProviderDBusType != newProviderDBusType) {
        cachedProviderDBusType = newProviderDBusType;
        emit providerDBusTypeChanged(providerDBusType());
    }
}

