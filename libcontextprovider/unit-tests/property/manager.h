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

#include <QDBusObjectPath>
#include <QDBusMessage>
#include <QHash>
#include <QObject>
#include <QDBusConnection>
#include <QStringList>

namespace ContextProvider {

class Subscriber;

class Manager : public QObject
{
    Q_OBJECT

public:
    Manager();
    void addKey(const QString &key);
    bool keyIsValid(const QString &key) const;
    void increaseSubscriptionCount(const QString &key);
    void decreaseSubscriptionCount(const QString &key);
    const QVariant keyValue(const QString &key);
    void setKeyValue(const QString &key, const QVariant &v);
    QVariant getKeyValue(const QString &key);
    void fakeFirst(const QString &key);
    void fakeLast(const QString &key);
    QStringList getKeys();

Q_SIGNALS:
    void firstSubscriberAppeared(const QString &key);
    void lastSubscriberDisappeared(const QString &key);

private:
    QStringList keys;
};

} // namespace ContextProvider

#endif
