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

#ifndef CONTEXTTYPEDEFINITIONINFO_H
#define CONTEXTTYPEDEFINITIONINFO_H

#include <QVariant>
#include <QStringList>
#include <QObject>
#include "nanotree.h"

class ContextTypeDefinitionInfo : public QObject
{
    Q_OBJECT

public:
    explicit ContextTypeDefinitionInfo(const NanoTree &tree);
    explicit ContextTypeDefinitionInfo(const QVariant &variant);
    ContextTypeDefinitionInfo();
    ContextTypeDefinitionInfo(const ContextTypeInfo &ti);
    ContextTypeDefinitionInfo operator=(const ContextTypeInfo& info);

    QString name() const;
    QStringList parameters() const;
    NanoTree parameterTree(QString p) const;
    QString parameterDoc(QString p) const;
    QString parameterValue(QString p) const;
    void setParameterValue(QString p, QString value);
    ContextTypeDefinitionInfo base() const;
    QString doc();
    NanoTree tree();

protected:
    NanoTree nanoTree; ///< The root nano tree representing this type.
};

#endif
