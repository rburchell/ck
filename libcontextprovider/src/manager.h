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

#ifndef MANAGER_H
#define MANAGER_H

#include <QHash>
#include <QObject>
#include <QStringList>
#include <QVariant>

class Subscriber;

class Manager : public QObject
{
    Q_OBJECT

public:
    Manager(const QStringList &keys);
    ~Manager();

    Subscriber* getSubscriber(const QString &busName);
    bool keyIsValid(const QString &key) const;
    void increaseSubscriptionCount(const QString &key);
    void decreaseSubscriptionCount(const QString &key);
    void setKeyValue(const QString &key, const QVariant &v);
    QVariant getKeyValue(const QString &key);
    int getSubscriptionCount(const QString &key) const;
    void busNameIsGone(const QString &key);
    bool hasSubscriberWithBusName(const QString &name) const;
    QStringList getKeys();

signals:
    /// Emitted when the first subscriber appears for the specified \a key.
    void firstSubscriberAppeared(const QString &key);

    /// Emitted when the last subscriber disappears from the specified \a key.
    void lastSubscriberDisappeared(const QString &key);
    
    /// Emitted when a \a key managed by the Manager changes to value \a newValue.
    void keyValueChanged(const QString &key, const QVariant& newValue);

private:
    QHash<QString, Subscriber*> busNamesToSubscribers; ///< List of subscribers. BusName to Subscriber mapping.
    QHash<QString, QVariant> keysToValues; ///< Values for keys. Key name to QVariant mapping.
    QHash<QString, int> keysToSubscriptionCount; ///< Subscription count. Key name to integer mapping.
    int subscriberCounter; ///< A simple counter (iterator) to name the subscribers uniquely.
};

#endif
