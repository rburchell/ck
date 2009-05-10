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
#include <QMutex>

class InfoBackend : public QObject
{
    Q_OBJECT 

public:

    static InfoBackend* instance();
    virtual QString name() const = 0;
	virtual QStringList listKeys() const = 0;
    virtual QList<QString> listKeys(QString providername) const = 0;
    virtual QList<QString> listProviders() const = 0;
    virtual QString typeForKey(QString key) const = 0;
    virtual QString docForKey(QString key) const = 0;
    virtual QString providerForKey(QString key) const = 0;

private:
    InfoBackend(QObject *parent = 0) : QObject(parent) {};
    InfoBackend(const InfoBackend&);

    InfoBackend& operator=(const InfoBackend&);
    
    static InfoBackend* backendInstance;
    
    friend class InfoXmlBackend;
};

#endif // INFOBACKEND_H
