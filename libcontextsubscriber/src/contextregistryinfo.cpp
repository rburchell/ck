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

ContextRegistryInfo* ContextRegistryInfo::registryInstance = NULL;

/* Public */

ContextRegistryInfo* ContextRegistryInfo::instance()
{
    static QMutex mutex;
    if (!registryInstance)
    {
        mutex.lock();
 
        if (! registryInstance) {
            registryInstance = new ContextRegistryInfo;
            connect(InfoBackend::instance(), SIGNAL(keysChanged(QStringList)), 
                    registryInstance, SLOT(onKeysChanged(QStringList)));
        }
 
        mutex.unlock();
    }
 
    return registryInstance;
}

QStringList ContextRegistryInfo::listKeys() const
{
    return InfoBackend::instance()->listKeys();
}

QStringList ContextRegistryInfo::listKeys(QString providername) const
{
    return InfoBackend::instance()->listKeys(providername);
}

QStringList ContextRegistryInfo::listProviders() const
{
    return InfoBackend::instance()->listProviders();
}

/* Slots */

void ContextRegistryInfo::onKeysChanged(QStringList currentKeys)
{
    emit(keysChanged(currentKeys));
}

