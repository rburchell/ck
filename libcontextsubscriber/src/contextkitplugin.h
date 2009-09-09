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

#include <QString>
#include <QDBusConnection>
#include <QSet>
#include <QVariant>
#include <QMap>
#include "dbusnamelistener.h"
#include "provider.h"

extern "C" {
    ContextSubscriber::IProviderPlugin* contextKitPluginFactory(QString constructionString);
}

namespace ContextSubscriber {
class ContextKitPlugin : public IProviderPlugin
{
    Q_OBJECT

public:
    ContextKitPlugin(const QDBusConnection bus, const QString& busName);
    void subscribe(QSet<QString> keys);
    void unsubscribe(QSet<QString> keys);

signals:
    void ready(); ///< Emitted when the GetSubscriber call returns successfully.
    void failed(QString error); ///< Emitted when the GetSubscriber call fails or provider not on D-Bus at all.
    void subscribeFinished(QString key); ///< Emitted when Subscribe call succeeds on D-Bus.
    void subscribeFailed(QString failedKey, QString error); ///< Emitted when Subscribe call fails on D-Bus.
    void valueChanged(QString key, QVariant value); ///< Emitted when ValueChanged signal comes on D-Bus

private slots:
    void onDBusValuesChanged(QMap<QString, QVariant> values);
    void onDBusGetSubscriberFinished(QString objectPath);
    void onDBusSubscribeFinished(QList<QString> keys);
    void onDBusSubscribeFailed(QList<QString> keys, QString error);
    void onProviderAppeared();
    void onProviderDisappeared();

private:
    DBusNameListener *providerListener; ///< Listens to provider's (dis)appearance over DBus
    SubscriberInterface *subscriberInterface; ///< The D-Bus interface for the Subscriber object
    ManagerInterface *managerInterface; ///< The D-Bus interface for the Manager object

    QDBusConnection *connection; ///< The connection to DBus
    QString busName; ///< The D-Bus service name of the ContextKit provider connected to
};


}

#endif
