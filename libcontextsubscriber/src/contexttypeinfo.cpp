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

ContextTypeInfo::ContextTypeInfo(const QVariant &root) : nanoTree(NanoTree(root))
{
}

ContextTypeInfo::ContextTypeInfo(const NanoTree &tree) : nanoTree(tree)
{
    QVariant root = nanoTree.root();
}

ContextTypeInfo::ContextTypeInfo() : nanoTree(QVariant())
{
}

ContextTypeInfo::ContextTypeInfo(const ContextTypeInfo &ti) : nanoTree(ti.nanoTree)
{
}

bool ContextTypeInfo::operator==(const ContextTypeInfo &other) const
{
    return (name() == other.name());
}

bool ContextTypeInfo::operator!=(const ContextTypeInfo &other) const
{
    return (name() != other.name());
}

ContextTypeInfo ContextTypeInfo::operator=(const ContextTypeInfo& info)
{
    nanoTree = NanoTree(info.nanoTree);
    return *this;
}

QString ContextTypeInfo::name() const
{
    return nanoTree.keyValue("name").toString();
}

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

QList<NanoTree> ContextTypeInfo::parameters() const
{
    QList<NanoTree> lst;
    foreach (QVariant v, nanoTree.keySub("params").toList()) {
        lst.append(NanoTree(v));
    }

    return lst;
}

NanoTree ContextTypeInfo::parameter(QString p) const
{
    return NanoTree(nanoTree.keyValue("params", p));
}

QString ContextTypeInfo::parameterDoc(QString p) const
{
    return nanoTree.keyValue(QString("params"), p, QString("doc")).toString();
}

QString ContextTypeInfo::parameterStringValue(QString p) const
{
    NanoTree params = nanoTree.keySub("params");
    NanoTree key = params.keySub(p);
    return key.stringValue();
}

QString ContextTypeInfo::doc()
{
    return nanoTree.keyValue("doc").toString();
}

int ContextTypeInfo::parameterIntValue(QString p) const
{
    NanoTree params = nanoTree.keySub("params");
    NanoTree key = params.keySub(p);
    return key.intValue();
}

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

ContextTypeInfo ContextTypeInfo::nullType()
{
    return ContextTypeInfo(QVariant());
}

ContextTypeInfo ContextTypeInfo::int64Type()
{
    return buildPrimitiveType("int64");
}

ContextTypeInfo ContextTypeInfo::stringType()
{
    return buildPrimitiveType("string");
}

ContextTypeInfo ContextTypeInfo::doubleType()
{
    return buildPrimitiveType("double");
}

ContextTypeInfo ContextTypeInfo::boolType()
{
    return buildPrimitiveType("bool");
}

ContextTypeInfo ContextTypeInfo::int32Type()
{
    return buildPrimitiveType("int32");
}

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

/* ContextMapTypeInfo */

QString ContextMapTypeInfo::keyDoc(QString key)
{
    return nanoTree.keyValue(QString("params"), QString("keys"), key, QString("doc")).toString();
}

ContextTypeInfo ContextMapTypeInfo::keyType(QString key)
{
    return ContextTypeInfo(nanoTree.keyValue(QString("params"), QString("keys"), key, QString("type")));
}

/* ContextUniformListTypeInfo */

ContextTypeInfo ContextUniformListTypeInfo::elementType()
{
    NanoTree params = nanoTree.keySub("params");
    NanoTree key = params.keySub("type");
    return ContextTypeInfo(key);
}

NanoTree ContextTypeInfo::tree()
{
    return nanoTree;
}

