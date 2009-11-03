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
#include "nanoxml.h"

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

/// Returns the full path to the core property declaration file. Takes
/// the \c CONTEXT_CORE_TYPES env variable into account.
QString ContextTypeRegistryInfo::coreTypesPath()
{
    const char *corepath = getenv("CONTEXT_CORE_TYPES");
    if (! corepath)
        corepath = DEFAULT_CONTEXT_CORE_TYPES;

    return QString(corepath);
}

ContextTypeInfo ContextTypeRegistryInfo::typeInfoForName(QString name)
{
    // Support for the old types
    if (name == "TRUTH")
        return boolType();
    else if (name == "STRING")
        return stringType();
    else if (name == "INT")
        return int32Type();
    else if (name == "INTEGER")
        return int32Type();
    else if (name == "DOUBLE")
        return doubleType();

    // Try using the cache first
    if (typeCache.contains(name))
        return typeCache.value(name);

    // No type in cache? Find it in the nano tree and put in cache.
    foreach (NanoTree typeTree, coreTree.keyValues("type")) {
        if (typeTree.keyValue("name") == name) {
            ContextTypeInfo typeInfo(typeTree);
            typeCache.insert(name, typeInfo);
            return typeInfo;
        }
    }

    // Not found. Return blank null type.
    return ContextTypeInfo();
}

/// Returns in instance of the null type info.
ContextTypeInfo ContextTypeRegistryInfo::nullType()
{
    return ContextTypeInfo(QVariant());
}

/// Returns in instance of the int64 type info.
ContextTypeInfo ContextTypeRegistryInfo::int64Type()
{
    return typeInfoForName("int64");
}

/// Returns in instance of the string type info.
ContextTypeInfo ContextTypeRegistryInfo::stringType()
{
    return typeInfoForName("string");
}

/// Returns in instance of the double type info.
ContextTypeInfo ContextTypeRegistryInfo::doubleType()
{
    return typeInfoForName("double");
}

/// Returns in instance of the bool type info.
ContextTypeInfo ContextTypeRegistryInfo::boolType()
{
    return typeInfoForName("bool");
}

/// Returns in instance of the int32 type info.
ContextTypeInfo ContextTypeRegistryInfo::int32Type()
{
    return typeInfoForName("int32");
}

/* Private */

ContextTypeRegistryInfo::ContextTypeRegistryInfo()
{
    contextDebug() << F_TYPES << "Reading core types from:" << ContextTypeRegistryInfo::coreTypesPath();
    NanoXml parser(ContextTypeRegistryInfo::coreTypesPath());
    if (parser.didFail())
        contextWarning() << F_TYPES << "Reading core types from" << ContextTypeRegistryInfo::coreTypesPath() << "failed, parsing error";
    else
        coreTree = parser.result();
}
