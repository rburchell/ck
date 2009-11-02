/*
 * Copyright (C) 2009 Nokia Corporation.
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

#ifndef SERVICEBACKEND_H
#define SERVICEBACKEND_H

#include <QObject>
#include <QString>
#include <QDBusConnection>
#include <QVariant>

namespace ContextProvider {


class ServiceBackend : QObject
{
    Q_OBJECT

public:
    explicit ServiceBackend(QDBusConnection connection, const QString &busName = "");
    static ServiceBackend *defaultServiceBackend;

    void setAsDefault();

    void ref();
    void unref();
    bool sharedConnection();
    bool start();
    void stop();
    static ServiceBackend* instance(QDBusConnection connection);
    static ServiceBackend* instance(QDBusConnection::BusType busType,
                                    const QString &busName, bool autoStart = true);

    void setValue(const QString &key, const QVariant &val);
    QDBusConnection connection;
};

} // end namespace

#endif
