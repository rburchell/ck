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

/*!
  \class SubscriberInterface

  \brief Proxy class for using the DBus interface
  org.freedesktop.ContextKit.Subscriber asynchronously.

  Implements methods for constructing the interface objects (given the DBus
  type, session or system, and bus name), calling the functions Subscribe and
  Unsubscribe asynchronously, and listening to the Changed signal.
*/

class SubscriberSignallingInterface;

class SubscriberInterface : public QObject
{
    Q_OBJECT

public:
    SubscriberInterface(const QDBusConnection::BusType busType, const QString& busName,
                        const QString& objectPath, QObject* parent = 0);

    void subscribe(QStringList keys);
    void unsubscribe(QStringList keys);

signals:
    void valuesChanged(QMap<QString, QVariant> values, bool processingSubscription);

private slots:
    void onSubscribeFinished(QDBusPendingCallWatcher* watcher);
    void onChanged(const QMap<QString, QVariant> &values, const QStringList &unknownKeys);

private:
    SubscriberSignallingInterface* iface;

    SubscriberInterface(const SubscriberInterface& other);
    SubscriberInterface& operator=(const SubscriberInterface& other);

    QMap<QString, QVariant>& mergeNullsWithMap(QMap<QString, QVariant> &map, QStringList nulls) const;
};

/*!
  \class SubscriberSignallingInterface

  \brief Proxy class for the DBus interface
  org.freedesktop.ContextKit.Subscriber which connects automatically to the
  Changed signal over DBus.

*/

class SubscriberSignallingInterface: public QDBusAbstractInterface
{
    Q_OBJECT

public:
    SubscriberSignallingInterface(const QString &dBusName, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

    ~SubscriberSignallingInterface();

Q_SIGNALS: // SIGNALS
    void Changed(const QMap<QString, QVariant> &values, const QStringList &unknownKeys);

private:
    static const QString interfaceName;
};

#endif
