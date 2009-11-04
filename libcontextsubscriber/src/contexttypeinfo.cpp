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

QString ContextTypeInfo::name() const
{
    return typeName;
}

NanoTree ContextTypeInfo::definition() const
{
    return ContextTypeRegistryInfo::instance()->typeDefinitionForName(name);
}

QString ContextTypeInfo::parameterDoc(QString p) const
{
    return definition().valueForKey("params", p, "doc");
}

QVariant parameterValue(QString p) const
{
    typeInfo.keyValue(p);
}

void ContextTypeInfo::setParameterValue(QString p, QVatiant v)
{
    typeInfo = typeInfo.addKeyValue(p, v);
}

QString ContextTypeInfo::doc() const
{
    return definition().valueForKey("doc");
}

ContextTypeInfo ContextTypeInfo::base() const
{
    QString baseName = definition().keyValue("base");
    return ContextTypeInfo(baseName);
}

QVariantList ContextTypeInfo::parameters() const
{
    // FIXME What the hell here?
    return QVariantList();
}

