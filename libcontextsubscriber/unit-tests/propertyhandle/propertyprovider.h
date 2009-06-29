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

namespace ContextSubscriber {

class PropertyProvider : public QObject
{
    Q_OBJECT

public:
    bool subscribe(const QString &key);
    void unsubscribe(const QString &key);
    static PropertyProvider* instance(const QDBusConnection::BusType busType, const QString& busName);

signals:
    void subscribeFinished(QSet<QString> keys);
    void valueChanged(QString key, QVariant value, bool processingSubscription);
public:
    // Logging
    static int instanceCount;
    static QList<QDBusConnection::BusType> instanceDBusTypes;
    static QStringList instanceDBusNames;
    // Log the subscribe calls
    static int subscribeCount; // count
    static QStringList subscribeKeys; // parameters
    static QStringList subscribeProviderNames; // provider name of the object
    // on which it was called
    // Log the unsubscribe calls
    static int unsubscribeCount;
    static QStringList unsubscribeKeys;
    static QStringList unsubscribeProviderNames; // provider name of the object
    // on which it was called

    // For tests
    PropertyProvider(QString name); // public only in tests
    static void resetLogs();
    QString myName;

    friend class PropertyHandleUnitTests;
};

} // end namespace

#endif
