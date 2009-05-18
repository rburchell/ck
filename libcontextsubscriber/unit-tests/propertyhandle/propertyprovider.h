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

// This is a mock implementation

#ifndef PROPERTYPROVIDER_H
#define PROPERTYPROVIDER_H

#include <QObject>
#include <QDBusConnection>
#include <QSet>
#include <QString>

class PropertyProvider : public QObject
{
    Q_OBJECT

public:
    void subscribe(const QString &key);
    void unsubscribe(const QString &key);
    static PropertyProvider* instance(const QDBusConnection::BusType busType, const QString& busName);
    PropertyProvider(); // public only in tests

signals:
    void subscribeFinished(QSet<QString> keys);
    void valueChanged(QString key, QVariant value, bool processingSubscription);
};

#endif
