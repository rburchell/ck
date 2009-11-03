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

#include "contexttyperegistryinfo.h"
#include "logging.h"
#include "loggingfeatures.h"
#include <QMutex>
#include <QMutexLocker>
#include <QCoreApplication>

ContextTypeRegistryInfo* ContextTypeRegistryInfo::registryInstance = NULL;

/* Public */

ContextTypeRegistryInfo* ContextTypeRegistryInfo::instance()
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    if (! registryInstance) {
        contextDebug() << F_TYPES << "Creating ContextTypeRegistryInfo instance";
        registryInstance = new ContextTypeRegistryInfo;

        // Move the backend to the main thread
        registryInstance->moveToThread(QCoreApplication::instance()->thread());
    }

    return registryInstance;
}

/// Returns the full path to the registry directory. Takes the
/// \c CONTEXT_TYPES env variable into account.
QString ContextTypeRegistryInfo::registryPath()
{
    const char *regpath = getenv("CONTEXT_TYPES");
    if (! regpath)
        regpath = DEFAULT_CONTEXT_TYPES;

    return QString(regpath);
}

