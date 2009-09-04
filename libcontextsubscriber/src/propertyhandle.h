/*
 * Copyright (C) 2008, 2009 Nokia Corporation.
 *
 * Contact: Marius Vollmer <marius.vollmer@nokia.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef PROPERTYHANDLE_H
#define PROPERTYHANDLE_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QSet>
#include <QReadWriteLock>
#include <QMutex>

class ContextPropertyInfo;

namespace ContextSubscriber {

class Provider;
class DBusNameListener;

class PropertyHandle : public QObject
{
    Q_OBJECT

public:
    void subscribe();
    void unsubscribe();

    QString key() const;
    QVariant value() const;
    bool isSubscribePending() const;
    const ContextPropertyInfo* info() const;

    static PropertyHandle* instance(const QString& key);

    void setValue(QVariant newValue);
    static void ignoreCommander();
    static void setTypeCheck(bool typeCheck);

signals:
    void valueChanged();

private slots:
    void onSubscribeFinished(QSet<QString> keys);
    void updateProvider();

private:
    PropertyHandle(const QString& key);

    Provider *myProvider; ///< Provider of this property
    ContextPropertyInfo *myInfo; ///< Metadata for this property
    unsigned int subscribeCount; ///< Number of subscribed ContextProperty objects subscribed to this property
    QMutex subscribeCountLock;
    bool subscribePending; ///< True when the subscription has been started, but hasn't been finished yet
                           ///  (used by the waitForSubscription() feature)
    QString myKey; ///< Key of this property
    mutable QReadWriteLock valueLock;
    QVariant myValue; ///< Current value of this property
    static DBusNameListener *commanderListener; ///< Listener for ContextCommander's (dis)appearance
    static bool commandingEnabled; ///< Whether the properties can be directed to ContextCommander
    static bool typeCheckEnabled; ///< Whether we check the type of the value received from the provider
};

} // end namespace

#endif
