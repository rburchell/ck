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

/* Public */

/// Creates a new type from the given QVariant tree.
ContextTypeInfo::ContextTypeInfo(const QVariant &root) : nanoTree(NanoTree(root))
{
}

/// Creates a new type from the given NanoDom tree.
ContextTypeInfo::ContextTypeInfo(const NanoTree &tree) : nanoTree(tree)
{
}

/// Creates an empty type info representing the null type.
ContextTypeInfo::ContextTypeInfo() : nanoTree(QVariant())
{
}

/// Creates a new ContextTypeInfo being a copy of the pass \a ti info.
ContextTypeInfo::ContextTypeInfo(const ContextTypeInfo &ti) : nanoTree(ti.nanoTree)
{
}

/// Assigment operator.
ContextTypeInfo ContextTypeInfo::operator=(const ContextTypeInfo& info)
{
    nanoTree = NanoTree(info.nanoTree);
    return *this;
}

/// Returns the name of the type. Ie. "double" for double type.
QString ContextTypeInfo::name() const
{
    return nanoTree.keyValue("name").toString();
}

/// Returns the base type for this type.
ContextTypeInfo ContextTypeInfo::base() const
{
    QString n = name();
    if (n == "duration")
        return ContextTypeInfo::int64Type();
    else if (n == "time")
        return ContextTypeInfo::int64Type();
    else if (n == "string-enum")
        return ContextTypeInfo::stringType();
    else if (n == "percentage")
        return ContextTypeInfo::int32Type();
    else
        return resolveTypeName(nanoTree.keyValue("base").toString());
}

/// Returns a list of parameter names for this type.
QStringList ContextTypeInfo::parameters() const
{
    return nanoTree.keyNode("params").keys();
}

/// Returns the NanoDom tree for the specified paramemeter \a p.
/// Returns and empty tree if the parameter does not exists.
NanoTree ContextTypeInfo::parameterTree(QString p) const
{
    return nanoTree.keyValue("params", p);
}

/// Returns documentation as string for the given parameter.
/// Returns an empty string if the parameter does not exists.
QString ContextTypeInfo::parameterDoc(QString p) const
{
    return nanoTree.keyValue(QString("params"), p, QString("doc")).toString();
}

/// Returns the value (if set) for the given parameter as string.
QString ContextTypeInfo::parameterValue(QString p) const
{
    return nanoTree.keySub("params").keySub(p).stringValue();
}

/// Sets the \a value (as string) for the parameter \a p.
void ContextTypeInfo::setParameterValue(QString p, QString value)
{
    // FIXME Use chainers now...
    NanoTree params = NanoTree(nanoTree.keyNode("params"));
    NanoTree parameterTree = NanoTree(params.keyNode(p));

    parameterTree = parameterTree.addStringValue(value);
    params = params.replaceKey(p, parameterTree);
    nanoTree = nanoTree.replaceKey("params", params);
}

/// Returns documentation for this ContextTypeInfo.
QString ContextTypeInfo::doc()
{
    // FIXME: Temporary. Gone when registry comes.
    return nanoTree.keyValue("doc").toString();
}

/// Returns in instance of the null type info.
ContextTypeInfo ContextTypeInfo::nullType()
{
    // FIXME: Temporary. Gone when registry comes.
    return ContextTypeInfo(QVariant());
}

/// Returns in instance of the int64 type info.
ContextTypeInfo ContextTypeInfo::int64Type()
{
    // FIXME: Temporary. Gone when registry comes.
    return buildPrimitiveType("int64");
}

/// Returns in instance of the string type info.
ContextTypeInfo ContextTypeInfo::stringType()
{
    // FIXME: Temporary. Gone when registry comes.
    return buildPrimitiveType("string");
}

/// Returns in instance of the double type info.
ContextTypeInfo ContextTypeInfo::doubleType()
{
    // FIXME: This is temporary, a full manual/static specification
    // of the double type. It'll be gone when we have the registry
    // implementation.

    QVariantList type;
    QVariantList doc;
    QVariantList tree;
    QVariantList params;
    QVariantList minParam;
    QVariantList minParamDoc;
    QVariantList maxParam;
    QVariantList maxParamDoc;

    type << QVariant("name");
    type << QVariant("double");
    doc << QVariant("doc");
    doc << QVariant("A double value within the given limits.");
    params << QVariant("params");
    minParam << QVariant("min");
    minParamDoc << QVariant("doc");
    minParamDoc << QVariant("Minimum value.");
    minParam << QVariant(minParamDoc);
    params << QVariant(minParam);

    maxParam << QVariant("max");
    maxParamDoc << QVariant("doc");
    maxParamDoc << QVariant("Maximum value.");
    maxParam << QVariant(maxParamDoc);
    params << QVariant(maxParam);

    tree << QVariant("type");
    tree << QVariant(type);
    tree << QVariant(doc);
    tree << QVariant(params);

    return ContextTypeInfo(NanoTree(tree));
}

/// Returns in instance of the bool type info.
ContextTypeInfo ContextTypeInfo::boolType()
{
    // FIXME: Temporary. Gone when registry comes.
    return buildPrimitiveType("bool");
}

/// Returns in instance of the int32 type info.
ContextTypeInfo ContextTypeInfo::int32Type()
{
    // FIXME: Temporary. Gone when registry comes.
    return buildPrimitiveType("int32");
}

/// Given a type name (ie. "double", "int32", "bool") returns the ContextTypeInfo
/// representing this type.
ContextTypeInfo ContextTypeInfo::resolveTypeName(QString t)
{
    // Support for the old types
    if (t == "TRUTH")
        return ContextTypeInfo::boolType();
    else if (t == "STRING")
        return ContextTypeInfo::stringType();
    else if (t == "INT")
        return ContextTypeInfo::int32Type();
    else if (t == "INTEGER")
        return ContextTypeInfo::int32Type();
    else if (t == "DOUBLE")
        return ContextTypeInfo::doubleType();
    // New types
    else if (t == "string")
        return ContextTypeInfo::stringType();
    else if (t == "double")
        return ContextTypeInfo::doubleType();
    else if (t == "int32")
        return ContextTypeInfo::int32Type();
    else if (t == "int64")
        return ContextTypeInfo::int64Type();
    else if (t == "bool")
        return ContextTypeInfo::boolType();
    else if (t == "map")
        return buildPrimitiveType("map");
    else if (t == "list")
        return buildPrimitiveType("list");
    else
        return ContextTypeInfo(QVariant());
}

/// Returns the NanoDom tree representing this type.
NanoTree ContextTypeInfo::tree()
{
    return nanoTree;
}

/* Protected */

/// A helper funtion to build a primitive type with a given name.
/// Will be removed in future when registry comes in place.
ContextTypeInfo ContextTypeInfo::buildPrimitiveType(QString name)
{
    // FIXME: Temporary. Gone when registry comes.
    QVariantList type;
    QVariantList tree;
    type << QVariant("name");
    type << QVariant(name);
    tree << QVariant("type");
    tree << QVariant(type);
    return ContextTypeInfo(NanoTree(tree));
}

/* ContextMapTypeInfo */

/// Returns the documentation for the given \a key in the map type. This is
/// equivalent of accessing params -> keys -> (key) -> doc .
QString ContextMapTypeInfo::keyDoc(QString key)
{
    return nanoTree.keyValue("params", "keys", key, "doc").toString();
}

/// Returns the type information for the given \a key in the map type. This is
/// equivalent of accessing params -> keys -> (key) -> type .
ContextTypeInfo ContextMapTypeInfo::keyType(QString key)
{
    return resolveTypeName(nanoTree.keyValue("params", "keys", key, "type").toString());
}

/* ContextUniformListTypeInfo */

/// Returns the type information for the elements in this list.
ContextTypeInfo ContextUniformListTypeInfo::elementType()
{
    return resolveTypeName(parameterValue("type"));
}

