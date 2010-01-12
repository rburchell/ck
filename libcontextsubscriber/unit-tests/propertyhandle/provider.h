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

#ifndef PROVIDER_H
#define PROVIDER_H

#include "contextproviderinfo.h"
#include "timedvalue.h"

#include <QObject>
#include <QDBusConnection>
#include <QSet>
#include <QString>

namespace ContextSubscriber {

class Provider : public QObject
{
    Q_OBJECT

public:
    static Provider* instance(const ContextProviderInfo& providerInfo);
    bool subscribe(const QString &key);
    void unsubscribe(const QString &key);
    TimedValue get(const QString &key) const;
    void clearValues();

Q_SIGNALS:
    void subscribeFinished(QString key);
    void valueChanged(QString key);

public:
    // Logging
    static int instanceCount;
    static QStringList instancePluginNames;
    static QStringList instancePluginConstructionStrings;
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
    static TimedValue cachedValue; // setValue sets, get gets

    // For tests
    Provider(QString name); // public only in tests
    static void setValue(const QString &key, const QVariant &value);
    static void resetLogs();
    QString myName;

    friend class PropertyHandleUnitTests;
};

} // end namespace

#endif
