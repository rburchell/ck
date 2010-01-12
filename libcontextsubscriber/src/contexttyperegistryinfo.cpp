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

/*!
    \class ContextTypeRegistryInfo

    \brief A class to access the type registry.

    This is a singelton class used to obtain information about the core types defined
    in the type registry. Information is provided as type definitions returned as
    AssocTree instances. Each type definition is a QVariant tree wrapped in AssocTree
    for easy helper key accessors.

    \section Usage

    To obtain a type definition for a given type:

    \code
    AssocTree typeDefinition = ContextTypeRegistryInfo::instance()->typeDefinitionForName("string-enum");
    \endcode

    Unless you're building a dedicated type-introspection application, you don't
    want to deal with ContextTypeRegistryInfo directly. Instead, you can use the ContextTypeInfo
    class to fetch concrete types and use the easy accessors provided there.
*/

ContextTypeRegistryInfo* ContextTypeRegistryInfo::registryInstance = NULL;

/* Public */

/// Returns the singleton instance of the ContextTypeRegistryInfo. The object
/// is constructed automaticall on first access.
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

/// Returns a type definition for the type with the given name. The type
/// is being fetched from the registry.
AssocTree ContextTypeRegistryInfo::typeDefinitionForName(QString name)
{
    // Support for the old types
    if (name == "TRUTH")
        name = "bool";
    else if (name == "STRING")
        name = "string";
    else if (name == "INT")
        name = "integer";
    else if (name == "INTEGER")
        name = "integer";
    else if (name == "DOUBLE")
        name = "double";

    // Try using the cache first
    if (typeCache.contains(name))
        return typeCache.value(name);

    // No type in cache? Find it in the assoc tree and put in cache.
    Q_FOREACH (AssocTree typeTree, coreTree.nodes()) {
        if (typeTree.name() == "type" && typeTree.value("name") == name) {
            typeCache.insert(name, typeTree);
            return typeTree;
        }
    }

    // Not found. Return blank null type.
    return AssocTree();
}

/* Private */

/// Private constructor. Do not use.
ContextTypeRegistryInfo::ContextTypeRegistryInfo()
{
    if (QFile(ContextTypeRegistryInfo::coreTypesPath()).exists()) {
        contextDebug() << F_TYPES << "Reading core types from:" << ContextTypeRegistryInfo::coreTypesPath();

        NanoXml parser(ContextTypeRegistryInfo::coreTypesPath());
        if (parser.didFail())
            contextWarning() << F_TYPES << "Reading core types from" << ContextTypeRegistryInfo::coreTypesPath() << "failed, parsing error";
        else
            coreTree = parser.result();
    } else
        contextDebug() << F_TYPES << "Core types at" << ContextTypeRegistryInfo::coreTypesPath() << "missing.";
}
