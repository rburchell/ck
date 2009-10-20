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

class ServiceUnitTest;

namespace ContextProvider {

class Property;
class ServiceBackend;

class Service : public QObject
{
    Q_OBJECT

public:
    explicit Service(QDBusConnection connection, QObject *parent = 0);
    Service(QDBusConnection::BusType busType, const QString &busName, QObject *parent = 0);
    Service(QDBusConnection::BusType busType, const QString &busName, bool autoStart, QObject *parent = 0);
    virtual ~Service();

    bool start();
    void stop();
    void restart();

    void setAsDefault();

    void setValue(const QString &key, const QVariant &val);
    void setConnection(const QDBusConnection &connection);

private:
    ServiceBackend *backend;

    friend class Property;
    friend class ::ServiceUnitTest;
};

} // end namespace

#endif
