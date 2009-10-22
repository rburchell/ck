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

#ifndef PROPERTYADAPTOR_H
#define PROPERTYADAPTOR_H

#include <QObject>
#include <QDBusAbstractAdaptor>
#include <QDBusMessage>
#include <QDBusConnection>
#include <QSet>
#include <QString>
#define DBUS_INTERFACE "org.maemo.contextkit.Property"

namespace ContextProvider {

class PropertyPrivate;

class PropertyAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.maemo.contextkit.Property")

public:
    PropertyAdaptor(PropertyPrivate* property, QDBusConnection *connection);
    QString objectPath() const;

public slots:
    void Subscribe(const QDBusMessage& msg, QVariantList& values, quint64& timestamp);
    void Unsubscribe(const QDBusMessage& msg);
    void Get(QVariantList& values, quint64& timestamp);

signals:
    void ValueChanged(const QVariantList &values, const quint64& timestamp);

private slots:
    void OnServiceOwnerChanged(const QString&, const QString&, const QString&);
    void onValueChanged(QVariantList values, quint64 timestamp);

private:
    PropertyPrivate *propertyPrivate; ///< The managed object.
    QDBusConnection *connection; ///< The connection to operate on.
    QSet<QString> clientServiceNames; ///< How many times each client (recognized by D-Bus service name) has subscribed

};

} // namespace ContextProvider

#endif
