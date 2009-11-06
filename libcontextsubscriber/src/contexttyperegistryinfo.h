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

#ifndef CONTEXTTYPEREGISTRYINFO_H
#define CONTEXTTYPEREGISTRYINFO_H

#include <QVariant>
#include <QStringList>
#include <QObject>
#include "nanotree.h"

class ContextTypeRegistryInfo : public QObject
{
    Q_OBJECT

public:
    static ContextTypeRegistryInfo* instance();

    QString registryPath();
    QString coreTypesPath();
    NanoTree typeDefinitionForName(QString name);

    NanoTree int64Type();
    NanoTree int32Type();
    NanoTree stringType();
    NanoTree boolType();
    NanoTree doubleType();

private:
    ContextTypeRegistryInfo(); ///< Private constructor. Do not use.
    ContextTypeRegistryInfo(const ContextTypeRegistryInfo&); ///< Private constructor. Do not use.
    ContextTypeRegistryInfo& operator=(const ContextTypeRegistryInfo&); ///< Private operator. Do not use.

    /// Holds the actual pointer to the singelton instance.
    /// Mutex protected during creation.
    static ContextTypeRegistryInfo* registryInstance;

    NanoTree coreTree; ///< The core.types as NanoTree. Read on construction.
    QHash <QString, NanoTree> typeCache; //< Cache of QString -> type definition.

    friend class ContextTypeRegistryInfoUnitTest;
};

#endif
