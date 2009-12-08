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
    ContextTypeInfo(const AssocTree &tree);
    ContextTypeInfo(const QVariant &tree);
    ContextTypeInfo();

    ContextTypeInfo ensureNewTypes();
    bool typeCheck(const QVariant &value) const;

    bool hasBase(QString wanted, int depth = 10) const;
    QVariantList parameters() const;
    QVariant parameterValue(QString p) const;
    AssocTree parameterNode(QString p) const;

    AssocTree definition() const;
    QString parameterDoc(QString p) const;
    QString doc() const;
    ContextTypeInfo base() const;
};

class ContextStringEnumInfo : public ContextTypeInfo
{
public:
    ContextStringEnumInfo(const ContextTypeInfo &info);

    QStringList choices() const;
    QString choiceDoc(const QString &choice) const;
};

class ContextListInfo : public ContextTypeInfo
{
public:
    ContextListInfo(const ContextTypeInfo &info);

    ContextTypeInfo type();
};

#endif
