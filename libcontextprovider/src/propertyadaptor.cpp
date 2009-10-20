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

#include "propertyadaptor.h"
#include "logging.h"
#include "sconnect.h"
#include "propertyprivate.h"

#include <QDBusConnection>

namespace ContextProvider {

/*!
    \class PropertyAdaptor
    \brief A DBus adaptor for implementing the org.maemo.contextkit.Property
*/

/// Constructor. Creates new adaptor for the given manager with the given
/// dbus connection. The connection \a conn is not retained.
PropertyAdaptor::PropertyAdaptor(PropertyPrivate* property, QDBusConnection *conn)
    : QDBusAbstractAdaptor(property), property(property), connection(conn)
{
    /*sconnect((QObject*) connection->interface(), SIGNAL(serviceOwnerChanged(const QString&, const QString&, const QString&)),
      this, SLOT(OnServiceOwnerChanged(const QString &, const QString&, const QString&)));*/

}

void PropertyAdaptor::Subscribe(const QDBusMessage &msg, QVariantList& value, qlonglong& timestamp)
{
}

void PropertyAdaptor::Unsubscribe()
{
}

/// Dbus interface slot. The PropertyAdaptor listens for dbus bus names changing
/// to notify the managed Property that a bus name is gone. It does it through
/// Property::busNameIsGone function.
/*void PropertyAdaptor::OnServiceOwnerChanged(const QString &name, const QString &oldName, const QString &newName)

{
    if (newName == "")
        manager->busNameIsGone(name);

}*/

} // namespace ContextProvider
