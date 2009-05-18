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

#include "infobackend.h"
#include "infoxmlbackend.h"
#include "infocdbbackend.h"
#include <QMutex>

InfoBackend* InfoBackend::backendInstance = NULL;

InfoBackend* InfoBackend::instance(const QString &backendName)
{
    static QMutex mutex;
    if (!backendInstance)
    {
        mutex.lock();

        if (! backendInstance) {
            if (backendName == "xml")
                backendInstance = new InfoXmlBackend;
            else if (backendName == "cdb")
                backendInstance = new InfoCdbBackend;
            else {
                if (InfoCdbBackend::databaseExists())
                    backendInstance = new InfoCdbBackend;
                else
                    backendInstance = new InfoXmlBackend;
            }
        }
 
        mutex.unlock();
    }
 
    return backendInstance;
}

void InfoBackend::checkAndEmitKeysAdded(QStringList &currentKeys, QStringList &oldKeys)
{
    QStringList addedKeys;
    foreach (QString key, currentKeys) {
        if (! oldKeys.contains(key))
            addedKeys << key;
    }

    if (addedKeys.size() > 0)
        emit keysAdded(addedKeys);
}

void InfoBackend::checkAndEmitKeysRemoved(QStringList &currentKeys, QStringList &oldKeys)
{
    QStringList removedKeys;
    foreach (QString key, oldKeys) {
        if (! currentKeys.contains(key))
            removedKeys << key;
    }

    if (removedKeys.size() > 0)
        emit keysRemoved(removedKeys);
}
