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

#include "contexttypeinfo.h"
#include "logging.h"
#include "contexttyperegistryinfo.h"

/* Public */

ContextTypeInfo::ContextTypeInfo(QString n) : typeName(n), typeInfo()
{
}

ContextTypeInfo::ContextTypeInfo() : typeName(""), typeInfo()
{
}

ContextTypeInfo::ContextTypeInfo(const ContextTypeInfo& info) : typeName(info.typeName), typeInfo(info.typeInfo)
{
}

QString ContextTypeInfo::name() const
{
    return typeName;
}

NanoTree ContextTypeInfo::definition() const
{
    return ContextTypeRegistryInfo::instance()->typeDefinitionForName(typeName);
}

QString ContextTypeInfo::parameterDoc(QString p) const
{
    return definition().keyValue("params", p, "doc").toString();
}

QVariant ContextTypeInfo::parameterValue(QString p) const
{
    return typeInfo.keyValue(p);
}

void ContextTypeInfo::setParameterValue(QString p, QVariant v)
{
    typeInfo = typeInfo.addKeyValue(p, v);
}

QString ContextTypeInfo::doc() const
{
    return definition().keyValue("doc").toString();
}

ContextTypeInfo ContextTypeInfo::base() const
{
    QString baseName = definition().keyValue("base").toString();
    return ContextTypeInfo(baseName);
}

QVariantList ContextTypeInfo::parameters() const
{
    // FIXME What the hell here?
    return QVariantList();
}

