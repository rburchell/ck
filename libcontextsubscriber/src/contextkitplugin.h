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
#include "propertyprovider.h"
namespace ContextSubscriber {

extern "C" {
    IProviderPlugin* contextKitPluginFactory(const QString& constructionString);
}

class ContextKitPlugin : public IProviderPlugin
{
    Q_OBJECT

public:
    ContextKitPlugin(const QDBusConnection bus, const QString& busName);
    void subscribe(QSet<QString> keys);
    void unsubscribe(QSet<QString> keys);

signals:
    void ready();
    void failed(QString error);
    void subscribeFinished(QString key);
    void subscribeFailed(QString failedKey, QString error);
    void valueChanged(QString key, QVariant value);

private slots:
    void onDBusValuesChanged(QMap<QString, QVariant> values);
    void onDBusGetSubscriberFinished(QString objectPath);
    void onDBusSubscribeFinished(QList<QString> keys);
    void onDBusSubscribeFailed(QList<QString> keys, QString error);
    void onProviderAppeared();
    void onProviderDisappeared();

private:
    DBusNameListener *providerListener; ///< Listens to provider's (dis)appearance over DBus
    SubscriberInterface *subscriberInterface; ///< The DBus interface for the Subscriber object
    ManagerInterface *managerInterface; ///< The DBus interface for the Manager object

    QDBusConnection *connection; ///< The connection to DBus
    QString busName; ///< The bus name of the DBus provider connected to
};


}

#endif
