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

// This is a mock implementation

#ifndef CONTEXTREGISTRYINFO_H
#define CONTEXTREGISTRYINFO_H

#include <QStringList>
#include <QString>
#include <QObject>

class ContextRegistryInfo : public QObject
{
    Q_OBJECT

public:
    static ContextRegistryInfo* instance(const QString &backendName = "");

signals:
    void keysChanged(QStringList currentKeys);
    void keysAdded(QStringList newKeys);
    void keysRemoved(QStringList removedKeys);
};

#endif
