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
#include "nanotree.h"

class ContextTypeInfo : public QObject
{
    Q_OBJECT

public:
    explicit ContextTypeInfo(const NanoTree &tree);
    explicit ContextTypeInfo(const QVariant &variant);
    ContextTypeInfo(const ContextTypeInfo &ti);

    QString name() const;
    QList<NanoTree> parameters() const;
    NanoTree parameter(QString p) const;
    QString parameterDoc(QString p) const;
    QString parameterStringValue(QString p) const;
    int parameterIntValue(QString p) const;
    ContextTypeInfo base() const;

    static ContextTypeInfo int64Type();
    static ContextTypeInfo intType();
    static ContextTypeInfo stringType();

protected:
    NanoTree nanoTree;
};

class ContextMapTypeInfo : public ContextTypeInfo
{
    Q_OBJECT

public:
    QString keyDoc(QString key);
    ContextTypeInfo keyType(QString key);
};

class ContextUniformListTypeInfo : public ContextTypeInfo
{
    Q_OBJECT

public:
    ContextTypeInfo elementType();
};

#endif
