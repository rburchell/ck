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
#include <QStringList>
#include <QtDBus>

/*
 * Proxy class for interface org.freedesktop.ContextKit.Subscriber.
 * Implements the operations Subscribe and Unsubscribe asynchronously.
 */

class SubscriberSignallingInterface;

class SubscriberInterface : public QObject
{
    Q_OBJECT

public:
    SubscriberInterface(const QDBusConnection::BusType busType, const QString& busName, const QString& objectPath, QObject* parent = 0);

    void subscribe(QStringList keys);
    void unsubscribe(QStringList keys);

signals:
    void subscribeFinished(QMap<QString, QVariant> values);
    void unsubscribeFinished();
    void valuesChanged(QMap<QString, QVariant> values);

private slots:
    void onSubscribeFinished(QDBusPendingCallWatcher* watcher);
    void onUnsubscribeFinished(QDBusPendingCallWatcher* watcher);
    void onChanged(const QMap<QString, QVariant> &values, const QStringList &unknownKeys);

private:
    SubscriberSignallingInterface* iface;

    SubscriberInterface(const SubscriberInterface& other);
    SubscriberInterface& operator=(const SubscriberInterface& other);

    QMap<QString, QVariant>& mergeNullsWithMap(QMap<QString, QVariant> &map, QStringList nulls);
};

class SubscriberSignallingInterface: public QDBusAbstractInterface
{
    Q_OBJECT

public:
    SubscriberSignallingInterface(const QString &dBusName, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

    ~SubscriberSignallingInterface();

/*public Q_SLOTS: // METHODS
    inline QDBusReply<DBusVariantMap> Subscribe(const QStringList &keys, QStringList &undeterminable_keys)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(keys);
        QDBusMessage reply = callWithArgumentList(QDBus::Block, QLatin1String("Subscribe"), argumentList);
        if (reply.type() == QDBusMessage::ReplyMessage && reply.arguments().count() == 2) {
            undeterminable_keys = qdbus_cast<QStringList>(reply.arguments().at(1));
        }
        return reply;
    }

    inline QDBusReply<void> Unsubscribe(const QStringList &keys)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(keys);
        return callWithArgumentList(QDBus::Block, QLatin1String("Unsubscribe"), argumentList);
        }*/

Q_SIGNALS: // SIGNALS
    void Changed(const QMap<QString, QVariant> &values, const QStringList &unknownKeys);

private:
    static const QString interfaceName;
};

#endif
