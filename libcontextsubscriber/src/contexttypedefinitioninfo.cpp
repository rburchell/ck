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

#include "contexttypedefinitioninfo.h"
#include "logging.h"
#include "contexttyperegistryinfo.h"

/* Public */

/// Creates a new type from the given QVariant tree.
ContextTypeDefinitionInfo::ContextTypeInfo(const QVariant &root) : nanoTree(NanoTree(root))
{
}

/// Creates a new type from the given NanoDom tree.
ContextTypeDefinitionInfo::ContextTypeInfo(const NanoTree &tree) : nanoTree(tree)
{
}

/// Creates an empty type info representing the null type.
ContextTypeDefinitionInfo::ContextTypeInfo() : nanoTree(QVariant())
{
}

/// Creates a new ContextTypeDefinitionInfo being a copy of the pass \a ti info.
ContextTypeDefinitionInfo::ContextTypeInfo(const ContextTypeInfo &ti) : nanoTree(ti.nanoTree)
{
}

/// Assigment operator.
ContextTypeDefinitionInfo ContextTypeInfo::operator=(const ContextTypeInfo& info)
{
    nanoTree = NanoTree(info.nanoTree);
    return *this;
}

/// Returns the name of the type. Ie. "double" for double type.
QString ContextTypeDefinitionInfo::name() const
{
    return nanoTree.keyValue("name").toString();
}

/// Returns the base type for this type.
ContextTypeDefinitionInfo ContextTypeInfo::base() const
{
    ContextTypeRegistryInfo *typeRegistry = ContextTypeRegistryInfo::instance();
    QString n = name();
    if (n == "duration")
        return typeRegistry->int64Type();
    else if (n == "time")
        return typeRegistry->int64Type();
    else if (n == "string-enum")
        return typeRegistry->stringType();
    else if (n == "percentage")
        return typeRegistry->int32Type();
    else
        return typeRegistry->typeInfoForName(nanoTree.keyValue("base").toString());
}

/// Returns a list of parameter names for this type.
QStringList ContextTypeDefinitionInfo::parameters() const
{
    return nanoTree.keyNode("params").keys();
}

/// Returns the NanoDom tree for the specified paramemeter \a p.
/// Returns and empty tree if the parameter does not exists.
NanoTree ContextTypeDefinitionInfo::parameterTree(QString p) const
{
    return nanoTree.keyValue("params", p);
}

/// Returns documentation as string for the given parameter.
/// Returns an empty string if the parameter does not exists.
QString ContextTypeDefinitionInfo::parameterDoc(QString p) const
{
    return nanoTree.keyValue(QString("params"), p, QString("doc")).toString();
}

/// Returns the value (if set) for the given parameter as string.
QString ContextTypeDefinitionInfo::parameterValue(QString p) const
{
    return nanoTree.keySub("params").keySub(p).stringValue();
}

/// Sets the \a value (as string) for the parameter \a p.
void ContextTypeDefinitionInfo::setParameterValue(QString p, QString value)
{
    // FIXME Use chainers now...
    NanoTree params = NanoTree(nanoTree.keyNode("params"));
    NanoTree parameterTree = NanoTree(params.keyNode(p));

    parameterTree = parameterTree.addStringValue(value);
    params = params.replaceKey(p, parameterTree);
    nanoTree = nanoTree.replaceKey("params", params);
}

/// Returns documentation for this ContextTypeDefinitionInfo.
QString ContextTypeDefinitionInfo::doc()
{
    // FIXME: Temporary. Gone when registry comes.
    return nanoTree.keyValue("doc").toString();
}

/// Returns the NanoDom tree representing this type.
NanoTree ContextTypeDefinitionInfo::tree()
{
    return nanoTree;
}

