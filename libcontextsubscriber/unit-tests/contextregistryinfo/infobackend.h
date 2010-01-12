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

#ifndef INFOBACKEND_H
#define INFOBACKEND_H

#include <QVariant>
#include <QStringList>
#include <QObject>
#include "contextproviderinfo.h"

class InfoBackend : public QObject
{
    Q_OBJECT

public:
    static InfoBackend* instance(const QString &backendName = "");

    QString name() const;
    QStringList listKeys() const;
    const QList<ContextProviderInfo> providersForKey(QString key);

    void fireKeysChanged(const QStringList& keys);
    void fireKeysAdded(const QStringList& keys);
    void fireKeysRemoved(const QStringList& keys);
    void fireListChanged();

Q_SIGNALS:
    void keysChanged(const QStringList& currentKeys);
    void keysAdded(const QStringList& newKeys);
    void keysRemoved(const QStringList& removedKeys);
    void listChanged();
};

#endif // INFOBACKEND_H
