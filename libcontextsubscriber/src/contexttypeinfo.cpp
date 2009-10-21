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

ContextTypeInfo::ContextTypeInfo(const ContextTypeInfo &ti) : nanoTree(ti.nanoTree)
{
}

QString ContextTypeInfo::name() const
{
    QVariant root = nanoTree.root();

    if (root.toList().at(1).type() == QVariant::String)
        return root.toList().at(1).toString();
    else if (root.toList().at(1).type() == QVariant::List)
        return root.toList().at(1).toList().at(0).toString();
    else
        return "";
}

QList<QVariant> ContextTypeInfo::parameters() const
{
    return nanoTree.keyValues("params");
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
