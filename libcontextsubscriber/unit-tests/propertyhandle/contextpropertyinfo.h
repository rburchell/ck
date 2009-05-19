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

// This is a mock implementation.

#ifndef CONTEXTPROPERTYINFO_H
#define CONTEXTPROPERTYINFO_H

#include <QVariant>
#include <QStringList>
#include <QObject>
#include <QDBusConnection>

class ContextPropertyInfo : public QObject
{
    Q_OBJECT

public:
    explicit ContextPropertyInfo(const QString &key, QObject *parent = 0);

    QString type() const;

    QString providerDBusName() const;
    QDBusConnection::BusType providerDBusType() const;

signals:
    void providerChanged(QString newProvider);
    void providerDBusTypeChanged(QDBusConnection::BusType newBusType);
    void typeChanged(QString newType);
    void existsChanged(bool exists);
};

#endif // CONTEXTPROPERTYINFO_H
