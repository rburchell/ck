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

#ifndef PROPERTYPROVIDER_H
#define PROPERTYPROVIDER_H

#include "managerinterface.h"

#include <QObject>
#include <QDBusConnection>
#include <QSet>
#include <QTimer>

class PropertyHandle;
class SubscriberInterface;

class PropertyProvider : public QObject
{
    Q_OBJECT

public:
    void subscribe(PropertyHandle* handle);
    void unsubscribe(PropertyHandle* handle);

    QString getName() const;

private:
    PropertyProvider (QDBusConnection::BusType busType, const QString& busName);
    ~PropertyProvider();

    SubscriberInterface *subscriber; ///< The DBUS interface for the provider

    QTimer idleTimer; ///< For scheduling subscriptions / unsubscriptions as idle processing
    ManagerInterface managerInterface;

    QSet<PropertyHandle*> toSubscribe;
    QSet<PropertyHandle*> toUnsubscribe;

    QString busName;

private slots:
    /*void changeValues(const DBusVariantMap& values, const QStringList& unknowns,
      const bool processingSubscription = false);*/
    void getSubscriberFinished(QString objectPath);

    friend class PropertyManager; // FIXME: check if this can be removed
};

#endif
