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

ContextTypeInfo::ContextTypeInfo(QString name)
{
    QVariantList lst;
    lst << QVariant(name);

    tree = NanoTree(lst);
}

ContextTypeInfo::ContextTypeInfo(NanoTree t) : tree(t)
{
}

ContextTypeInfo::ContextTypeInfo()
{
}

ContextTypeInfo::ContextTypeInfo(const ContextTypeInfo& info) : tree(info.tree)
{
}

/// Assigment operator.
ContextTypeInfo ContextTypeInfo::operator=(const ContextTypeInfo& info)
{
    tree = NanoTree(info.tree);
    return *this;
}

QString ContextTypeInfo::name() const
{
    return tree.keyName();
}

NanoTree ContextTypeInfo::definition() const
{
    return ContextTypeRegistryInfo::instance()->typeDefinitionForName(name());
}

QString ContextTypeInfo::parameterDoc(QString p) const
{
    return definition().keyValue("params", p, "doc").toString();
}

QVariant ContextTypeInfo::parameterValue(QString p) const
{
    return tree.keyValue(p);
}

void ContextTypeInfo::setParameterValue(QString p, QVariant v)
{
    tree = tree.addKeyValue(p, v);
}

QString ContextTypeInfo::doc() const
{
    return definition().keyValue("doc").toString();
}

ContextTypeInfo ContextTypeInfo::base() const
{
    return ContextTypeInfo(definition().keyValue("base").toString());
}

QVariantList ContextTypeInfo::parameters() const
{
    // FIXME What the hell here?
    return QVariantList();
}

