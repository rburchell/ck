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
class Property;

class Service : public QObject
{
    Q_OBJECT

public:
    explicit Service(QDBusConnection::BusType busType, const QString &busName, QObject *parent = 0);
    virtual ~Service();

    void start();
    void stop();
    void restart();

    void setAsDefault();

private:
    QList<Property *> props;
    QDBusConnection::BusType busType;
    const QString busName;
    Manager *manager;
    QDBusConnection *connection;

    void add(Property *prop);

    friend class Property;
};

class Property : public QObject
{
    Q_OBJECT

public:
    explicit Property(Service &service, const QString &key, QObject *parent = 0);
    explicit Property(const QString &key, QObject *parent = 0);
    virtual ~Property();

    QString getKey() const;
    bool isValid() const;
    bool isSet() const;
    
    void set(const QVariant &v);
    void unset();
    
    QVariant get();
   
private:
    void setManager(Manager *manager);
    bool keyCheck() const;

    Manager *manager;
    QString key;

    friend class Service;

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
