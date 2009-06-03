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

#include "contextregistryinfo.h"
#include "infobackend.h"
#include "sconnect.h"
#include <QMutex>

/*!
    \class ContextRegistryInfo

    \brief A class to introspect the registry contents.

    This is a singelton class used to obtain information about the keys (properties)
    in the registry database. The information can be provided either from xml files 
    or from a cdb database. It's possible to list all the keys in the registry and 
    also list all keys belonging to a one particular provider. 
*/

ContextRegistryInfo* ContextRegistryInfo::registryInstance = NULL;

/* Public */

/// Returns the singleton instance of the ContextRegistryInfo. The object
/// is constructed automaticall on first access. 
/// \param backendName the optional name of the backend to use (force). 
ContextRegistryInfo* ContextRegistryInfo::instance(const QString &backendName)
{
    static QMutex mutex;
    if (!registryInstance)
    {
        mutex.lock();
 
        if (! registryInstance) {
            InfoBackend::instance(backendName);
            registryInstance = new ContextRegistryInfo;
            
            sconnect(InfoBackend::instance(), SIGNAL(keysChanged(QStringList)), 
                     registryInstance, SLOT(onKeysChanged(QStringList)));

            sconnect(InfoBackend::instance(), SIGNAL(keysAdded(QStringList)), 
                     registryInstance, SLOT(onKeysAdded(QStringList)));
            
            sconnect(InfoBackend::instance(), SIGNAL(keysRemoved(QStringList)), 
                     registryInstance, SLOT(onKeysRemoved(QStringList)));
        }
 
        mutex.unlock();
    }
 
    return registryInstance;
}

/// Returns the list of all the keys currently availible in the registry.
QStringList ContextRegistryInfo::listKeys() const
{
    return InfoBackend::instance()->listKeys();
}

/// Returns the list of all the keys associated with the given provider.
QStringList ContextRegistryInfo::listKeys(QString providername) const
{
    return InfoBackend::instance()->listKeys(providername);
}

/// Returns the list of all unique providers in the registry. 
/// The lists consists of strings with dbus names of the providers.
QStringList ContextRegistryInfo::listProviders() const
{
    return InfoBackend::instance()->listProviders();
}

/// Returns the name of the currently used registry backend. Ie. "cdb" or "xml".
QString ContextRegistryInfo::backendName() const
{
    return InfoBackend::instance()->name();
}

/* Slots */

/// This is connected to the \a onKeysChanged of the actual info backend instance.
void ContextRegistryInfo::onKeysChanged(const QStringList& currentKeys)
{
    emit(keysChanged(currentKeys));
}

/// This is connected to the \a onKeysAdded of the actual info backend instance.
void ContextRegistryInfo::onKeysAdded(const QStringList& newKeys)
{
    emit(keysAdded(newKeys));
}

/// This is connected to the \a onKeysRemoved of the actual info backend instance.
void ContextRegistryInfo::onKeysRemoved(const QStringList& removedKeys)
{
    emit(keysRemoved(removedKeys));
}

