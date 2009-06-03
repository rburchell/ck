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

#ifndef SUBSCRIBERINTERFACE_H
#define SUBSCRIBERINTERFACE_H

#include <QObject>
#include <QString>
#include <QtDBus>

// This is a mock implementation

class SubscriberInterface : public QObject
{
    Q_OBJECT

public:
    SubscriberInterface(const QDBusConnection::BusType busType, const QString& busName,
                        const QString& objectPath, QObject* parent = 0);

    void subscribe(QSet<QString> keys);
    void unsubscribe(QSet<QString> keys);

signals:
    void valuesChanged(QMap<QString, QVariant> values, bool processingSubscription);
    void subscribeFinished(QSet<QString> keys);

public:
    // Logging
    static int creationCount;
    static QList<QDBusConnection::BusType> creationBusTypes;
    static QStringList creationBusNames;
    static QStringList creationObjectPaths;

    static int subscribeCount;
    static int unsubscribeCount;
    static QList<QSet<QString> > subscribeKeys;
    static QList<QSet<QString> > unsubscribeKeys;

    static void resetLogs();

    friend class PropertyProviderUnitTests;

};


#endif
