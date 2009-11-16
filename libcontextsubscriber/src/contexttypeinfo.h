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

#ifndef CONTEXTTYPEINFO_H
#define CONTEXTTYPEINFO_H

#include <QVariant>
#include <QStringList>
#include <QObject>
#include "assoctree.h"

class ContextTypeInfo : public AssocTree
{
public:
    ContextTypeInfo (const AssocTree &tree) : AssocTree (tree) { }
    ContextTypeInfo (const QVariant &tree) : AssocTree (tree) { }
    ContextTypeInfo () { }

    ContextTypeInfo ensureNewTypes();
    bool typeCheck(const QVariant &value) const;

    QString name() const { return AssocTree::name(); }
    QVariantList parameters() const { return nodes(); }
    QVariant parameterValue(QString p) const { return value(p); }
    AssocTree parameterNode(QString p) const { return node(p); }

    AssocTree definition() const;
    QString parameterDoc(QString p) const { return definition().value("params", p, "doc").toString(); }
    QString doc() const { return definition().value("doc").toString(); }
    ContextTypeInfo base() const { return ContextTypeInfo(definition().value("base")); }
};

class ContextStringEnumInfo : public ContextTypeInfo
{
public:
    ContextStringEnumInfo (const ContextTypeInfo &info) : ContextTypeInfo (info) { }

    QStringList choices () const;
    QString choiceDoc (const QString &choice) const { return value (choice, "doc").toString(); }
};

class ContextListInfo : public ContextTypeInfo
{
public:
    ContextListInfo (const ContextTypeInfo &info) : ContextTypeInfo (info) { }

    ContextTypeInfo type() { return ContextTypeInfo(parameterValue("type")); }
};

#endif
