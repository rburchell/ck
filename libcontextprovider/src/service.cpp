/*
 * Copyright (C) 2008, 2009 Nokia Corporation.
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

#include "service.h"
#include "property.h"
#include "logging.h"
#include "manager.h"
#include "manageradaptor.h"
#include "sconnect.h"
#include "loggingfeatures.h"

namespace ContextProvider {

/*!
    \class Service

*/

static Service *defaultService;

Service::Service(QDBusConnection::BusType busType, const QString &busName, QObject* parent)
    : QObject(parent), busType(busType), busName(busName), manager(NULL), connection(NULL)
{
    contextDebug() << F_SERVICE << "Creating new Service for" << busName;
}

Service::~Service()
{
    contextDebug() << F_SERVICE << F_DESTROY << "Destroying Service";
    stop();
}

void Service::add(Property *prop)
{
    props << prop;
    prop->setManager(manager);
}

void Service::setAsDefault()
{
    if (defaultService) {
        contextCritical() << F_SERVICE << "Default service already set.";
        return;
    }

    defaultService = this;
}

void Service::start()
{
    if (manager)
        return;

    QStringList keys;
    foreach (Property *p, props)
        keys << p->getKey();

    connection = new QDBusConnection(QDBusConnection::connectToBus(busType, busName));
    manager = new Manager(keys);

    ManagerAdaptor *managerAdaptor = new ManagerAdaptor(manager, connection);

    // Register service
    if (! connection->registerService(busName)) {
        contextCritical() << F_SERVICE << "Failed to register service with name" << busName;
        stop();
        return;
    }

    // Register object
    if (managerAdaptor && !connection->registerObject("/org/freedesktop/ContextKit/Manager", manager)) {
        contextCritical() << F_SERVICE << "Failed to register the Manager object for" << busName;
        stop();
        return;
    }

    foreach(Property *p, props)
        p->setManager(manager);
}

void Service::stop()
{
    if (manager == NULL)
        return;

    contextDebug() << F_SERVICE << "Stopping service for bus:" << busName;

    foreach(Property *p, props)
        p->setManager(NULL);

    // Unregister
    connection->unregisterObject("/org/freedesktop/ContextKit/Manager");
    connection->unregisterService(busName);

    // Dealloc
    delete manager;
    delete connection;
    
    manager = NULL;
    connection = NULL;
}

void Service::restart()
{
    if (manager) {
        stop();
        start();
    }
}

} // end namespace
