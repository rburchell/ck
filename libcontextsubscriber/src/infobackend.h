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

class InfoBackend : public QObject
{
    Q_OBJECT 

public:

    static InfoBackend* instance(const QString &backendName = "");
    virtual QString name() const = 0;
    virtual QStringList listKeys() const = 0;
    virtual QStringList listKeys(QString providername) const = 0;
    virtual QStringList listProviders() const = 0;
    virtual QString typeForKey(QString key) const = 0;
    virtual QString docForKey(QString key) const = 0;
    virtual QString providerForKey(QString key) const = 0;
    virtual QString providerDBusTypeForKey(QString key) const = 0;

signals:
    void keysChanged(QStringList currentKeys);
    void keysAdded(QStringList newKeys);
    void keysRemoved(QStringList removedKeys);
    void keyDataChanged(QString key);

private:
    InfoBackend(QObject *parent = 0) : QObject(parent) {};
    InfoBackend(const InfoBackend&);
    void checkAndEmitKeysAdded(QStringList &currentKeys, QStringList &oldKeys);
    void checkAndEmitKeysRemoved(QStringList &currentKeys, QStringList &oldKeys);

    InfoBackend& operator=(const InfoBackend&);
    
    static InfoBackend* backendInstance;
    
    friend class InfoXmlBackend;
    friend class InfoCdbBackend;
};

#endif // INFOBACKEND_H
