/*
 * Copyright (C) 2008 Nokia Corporation.
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

#ifndef CONTEXT_H
#define CONTEXT_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QDBusConnection>
#include <QHash>
#include <QVariant>

namespace ContextProvider {

class Manager;

class Property : public QObject
{
    Q_OBJECT

public:
    explicit Property(const QString &key, QObject *parent = 0);
    virtual ~Property();
    QString getKey() const;
    bool isValid() const;
    bool isSet() const;
    
    void set(const QVariant &v);
    void unset();
    
    QVariant get();

    static bool initService(QDBusConnection::BusType busType, const QString &busName, const QStringList &keys);
    static void stopService(const QString &busName);
   
private:
    static QHash<QString, Manager*> busesToManagers;
    static QHash<QString, QDBusConnection*> busesToConnections;
    static QHash<QString, Manager*> keysToManagers;
    bool keyCheck() const;

    ContextProvider::Manager *manager; ///< This property Manager (service).
    QString key; ///< The key name for this property.

private slots:
    void onManagerFirstSubscriberAppeared(const QString &key);
    void onManagerLastSubscriberDisappeared(const QString &key);

signals:
    /// This is emitted when the first subscriber appears for this Property.
    /// It can be used ie. to start the actual process of harvesting the 
    /// data needed for this Property.
    void firstSubscriberAppeared(const QString &key); 
    
    /// This is emitted when the last subscriber disappears for this Property.
    /// It can be used ie. to stop the process of harvesting the 
    /// data needed for this Property (and save resources).
    void lastSubscriberDisappeared(const QString &key);
};

} // end namespace

#endif
