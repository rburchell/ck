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
    return InfoBackend::instance()->typeForKey(keyName);
}

/// Returns the dbus name of the provider supplying this property/key.
QString ContextPropertyInfo::providerDBusName() const
{
    return InfoBackend::instance()->providerForKey(keyName);
}

/// Returns the bus type of the provider supplying this property/key.
/// Ie. if it's a session bus or a system bus. 
QDBusConnection::BusType ContextPropertyInfo::providerDBusType() const
{
    QString busTypeName = InfoBackend::instance()->providerDBusTypeForKey(keyName);
    
    if (busTypeName == "session")
        return QDBusConnection::SessionBus;
    else if (busTypeName == "system")
        return QDBusConnection::SystemBus;
    else
        return QDBusConnection::SessionBus;
}

/* Slots */

void ContextPropertyInfo::onKeyDataChanged(QString key)
{
    if (key != keyName)
        return;

    // FIXME Cache the values and actually do SEE if it changed
    emit typeChanged(type());
    emit providerChanged(providerDBusName());
}

