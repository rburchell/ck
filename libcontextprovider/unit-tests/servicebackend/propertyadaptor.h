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

// This is a mock implementation

#ifndef PROPERTYADAPTOR_H
#define PROPERTYADAPTOR_H

#include <QObject>
#include <QDBusConnection>
#include <QSet>
#include <QString>

namespace ContextProvider {

class PropertyPrivate;

class PropertyAdaptor : public QObject
{
    Q_OBJECT

public:
    PropertyAdaptor(PropertyPrivate* property, QDBusConnection *connection);
    QString objectPath() const;
    void forgetClients();
};

} // namespace ContextProvider

#endif
