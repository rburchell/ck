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

#ifndef CONTEXTKITPLUGIN_H
#define CONTEXTKITPLUGIN_H

#include "dbusnamelistener.h"
#include "subscriberinterface.h"
#include "provider.h"
#include "iproviderplugin.h"
#include "asyncdbusinterface.h"
#include "timedvalue.h"
#include <QString>
#include <QDBusConnection>
#include <QDBusPendingCallWatcher>
#include <QDBusObjectPath>
#include <QSet>
#include <QVariant>
#include <QMap>

extern "C" {
    ContextSubscriber::IProviderPlugin* contextKitPluginFactory(QString constructionString);
}

namespace ContextSubscriber {
class PendingSubscribeWatcher : public QDBusPendingCallWatcher
{
    Q_OBJECT;

public:
    PendingSubscribeWatcher(const QDBusPendingCall &call,
                            const QString &key,
                            QObject * parent = 0);
private slots:
    void onFinished();

signals:
    void subscribeFailed(QString, QString);
    void valueChanged(QString, TimedValue);
    void subscribeFinished(QString);

private:
    QString key;
};

class ContextKitPlugin : public IProviderPlugin
{
    Q_OBJECT

public:
    ContextKitPlugin(const QDBusConnection bus, const QString& busName);
    void subscribe(QSet<QString> keys);
    void unsubscribe(QSet<QString> keys);
    void setDefaultNewProtocol(bool s);

signals:
#ifdef DOXYGEN_ONLY
    void ready(); ///< Emitted when the GetSubscriber call returns successfully.
    void failed(QString error); ///< Emitted when the GetSubscriber call fails or provider not on D-Bus at all.
    void subscribeFinished(QString key); ///< Emitted when Subscribe call succeeds on D-Bus.
    void subscribeFailed(QString failedKey, QString error); ///< Emitted when Subscribe call fails on D-Bus.
    void valueChanged(QString key, QVariant value); ///< Emitted when ValueChanged signal comes on D-Bus
#endif

private slots:
    void onNewValueChanged(QList<QVariant> value,
                           quint64 timestamp,
                           QDBusMessage message);
    void onDBusValuesChanged(QMap<QString, QVariant> values);
    void onDBusGetSubscriberFinished(QDBusObjectPath objectPath);
    void onDBusGetSubscriberFailed(QDBusError err);
    void onDBusSubscribeFinished(QList<QString> keys);
    void onDBusSubscribeFailed(QList<QString> keys, QString error);
    void onProviderAppeared();
    void onProviderDisappeared();
    void newSubscribe(const QString& key);

private:
    static QString keyToPath(QString key);
    static QString pathToKey(QString key);

    void reset();
    void useNewProtocol();

    QMap<QString, QVariant>& mergeNullsWithMap(QMap<QString, QVariant> &map, QStringList nulls) const;

    DBusNameListener *providerListener; ///< Listens to provider's (dis)appearance over DBus
    SubscriberInterface *subscriberInterface; ///< The D-Bus interface for the Subscriber object
    QDBusAbstractInterface *managerInterface; ///< The D-Bus interface for the Manager object

    QDBusConnection *connection; ///< The connection to DBus
    QString busName; ///< The D-Bus service name of the ContextKit provider connected to

    bool newProtocol; ///< The current provider on D-Bus speaks the new protocol only.
    bool defaultNewProtocol; ///< Let's only try the new protocol to talk with the provider.

    static const QString managerIName; ///< org.freedesktop.ContextKit.Manager
    static const QString subscriberIName; ///< org.freedesktop.ContextKit.Subscriber
    static const QString managerPath; ///< /org/freedesktop/ContextKit/Manager
    static const QString propertyIName; ///< org.maemo.contextkit.Property
    static const QString corePrefix; ///< /org/maemo/contextkit/
};

QVariant demarshallValue(const QVariant &v);

}

#endif
