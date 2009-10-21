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

#ifndef PROPERTYPRIVATE_H
#define PROPERTYPRIVATE_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QPair>

namespace ContextProvider {

class ServiceBackend;

class PropertyPrivate : public QObject
{
    Q_OBJECT

public:
    explicit PropertyPrivate(ServiceBackend* serviceBackend, const QString &key, QObject *parent = 0);

    void setValue(const QVariant& v);

signals:
    void valueChanged(const QVariantList& values, const qlonglong& timestamp);
    void firstSubscriberAppeared(const QString& key);
    void lastSubscriberDisappeared(const QString& key);

private:
    static qlonglong currentTimestamp();

    ServiceBackend* serviceBackend; ///< Pointer to the serviceBackend taking care of D-Bus related things
    QString key; ///< Key of this property
    QVariant value; ///< Current value of the property, QVariant() if null
    qlonglong timestamp; ///< Time when the value was set
    static QHash<QPair<ServiceBackend*, QString>, PropertyPrivate*> propertyPrivateMap;
    friend class Property;
    friend class PropertyAdaptor;
};

} // end namespace

#endif