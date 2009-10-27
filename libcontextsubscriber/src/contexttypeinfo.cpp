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
    QVariant root = nanoTree.root();
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
        return ContextTypeInfo(QVariant());
}

/// Returns a list of NanoDom trees representing the parameters for this type.
QList<NanoTree> ContextTypeInfo::parameters() const
{
    QList<NanoTree> lst;
    foreach (QVariant v, nanoTree.keySub("params").toList()) {
        lst.append(NanoTree(v));
    }

    return lst;
}

/// Returns the NanoDom tree for the specified paramemeter \a p.
/// Returns and empty tree if the parameter does not exists.
NanoTree ContextTypeInfo::parameter(QString p) const
{
    return NanoTree(nanoTree.keyValue("params", p));
}

/// Returns documentation as string for the given parameter.
/// Returns an empty string if the parameter does not exists.
QString ContextTypeInfo::parameterDoc(QString p) const
{
    return nanoTree.keyValue(QString("params"), p, QString("doc")).toString();
}

/// Returns the value (if set) for the given parameter as string.
QString ContextTypeInfo::parameterStringValue(QString p) const
{
    NanoTree params = nanoTree.keySub("params");
    NanoTree key = params.keySub(p);
    return key.stringValue();
}

/// Returns the value (if set) for the given parameter as int.
int ContextTypeInfo::parameterIntValue(QString p) const
{
    NanoTree params = nanoTree.keySub("params");
    NanoTree key = params.keySub(p);
    return key.intValue();
}

/// Returns documentation for this ContextTypeInfo.
QString ContextTypeInfo::doc()
{
    return nanoTree.keyValue("doc").toString();
}

/// Returns in instance of the null type info.
ContextTypeInfo ContextTypeInfo::nullType()
{
    return ContextTypeInfo(QVariant());
}

/// Returns in instance of the int64 type info.
ContextTypeInfo ContextTypeInfo::int64Type()
{
    return buildPrimitiveType("int64");
}

/// Returns in instance of the string type info.
ContextTypeInfo ContextTypeInfo::stringType()
{
    return buildPrimitiveType("string");
}

/// Returns in instance of the double type info.
ContextTypeInfo ContextTypeInfo::doubleType()
{
    return buildPrimitiveType("double");
}

/// Returns in instance of the bool type info.
ContextTypeInfo ContextTypeInfo::boolType()
{
    return buildPrimitiveType("bool");
}

/// Returns in instance of the int32 type info.
ContextTypeInfo ContextTypeInfo::int32Type()
{
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
    QVariantList type;
    QVariantList tree;
    type << QVariant("name");
    type << QVariant(name);
    tree << QVariant("type");
    tree << QVariant(type);
    return ContextTypeInfo(QVariant(tree));
}

/* ContextMapTypeInfo */

/// Returns the documentation for the given \a key in the map type. This is
/// equivalent of accessing params -> keys -> (key) -> doc .
QString ContextMapTypeInfo::keyDoc(QString key)
{
    return nanoTree.keyValue(QString("params"), QString("keys"), key, QString("doc")).toString();
}

/// Returns the type information for the given \a key in the map type. This is
/// equivalent of accessing params -> keys -> (key) -> type .
ContextTypeInfo ContextMapTypeInfo::keyType(QString key)
{
    return resolveTypeName(nanoTree.keyValue(QString("params"), QString("keys"), key, QString("type")).toString());
}

/* ContextUniformListTypeInfo */

/// Returns the type information for the elements in this list.
ContextTypeInfo ContextUniformListTypeInfo::elementType()
{
    QString typeName = parameterStringValue("type");
    return resolveTypeName(typeName);
}


