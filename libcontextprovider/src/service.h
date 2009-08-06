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

#ifndef SERVICE_H
#define SERVICE_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QDBusConnection>
#include <QHash>
#include <QVariant>

namespace ContextProvider {

class Manager;
class Property;
class ServicePrivate;

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

    void setValue(const QString &key, const QVariant &val);

private:
    class ServicePrivate *priv;

    Manager *manager();

    static Service *defaultService;

    friend class Property;
    friend class ServicePrivate;
};

} // end namespace

#endif
