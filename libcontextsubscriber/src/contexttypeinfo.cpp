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

ContextTypeInfo::ContextTypeInfo(const QVariant &variant)
{
    ContextTypeInfo(NanoTree(variant));
}

ContextTypeInfo::ContextTypeInfo(const NanoTree &tree) : nanoTree(tree)
{
    QVariant root = nanoTree.root();

    if (root.type() != QVariant::List &&
        root.toList().at(0).toString() != "type") {
        contextWarning() << "Nanotree does not represent type!";
        return;
    }
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
    return ContextTypeInfo(info);
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
        return ContextTypeInfo::intType();
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

ContextTypeInfo ContextTypeInfo::nullType()
{
    return ContextTypeInfo(QVariant());
}

ContextTypeInfo ContextTypeInfo::int64Type()
{
    QVariantList lst;
    lst << QVariant("type");
    lst << QVariant("int64");
    return ContextTypeInfo(QVariant(lst));
}

ContextTypeInfo ContextTypeInfo::stringType()
{
    QVariantList lst;
    lst << QVariant("type");
    lst << QVariant("int64");
    return ContextTypeInfo(QVariant(lst));
}

ContextTypeInfo ContextTypeInfo::intType()
{
    QVariantList lst;
    lst << QVariant("type");
    lst << QVariant("int64");
    return ContextTypeInfo(QVariant(lst));
}

ContextTypeInfo ContextTypeInfo::boolType()
{
    QVariantList lst;
    lst << QVariant("type");
    lst << QVariant("bool");
    return ContextTypeInfo(QVariant(lst));
}

ContextTypeInfo ContextTypeInfo::typeFromOldType(QString t)
{
    if (t == "TRUTH")
        return ContextTypeInfo::boolType();
    else if (t == "STRING")
        return ContextTypeInfo::stringType();
    else if (t == "INTEGER")
        return ContextTypeInfo::intType();
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

