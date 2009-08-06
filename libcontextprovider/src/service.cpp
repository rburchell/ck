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

#include <QTimer>

#include "service.h"
#include "property.h"
#include "logging.h"
#include "manager.h"
#include "manageradaptor.h"
#include "sconnect.h"
#include "loggingfeatures.h"

namespace ContextProvider {

/*!

  \mainpage Providing values for Context properties

  This library implements the provider side of the Context Framework.
  It has both a C++ and a C interface, so you can choose which you
  prefer.

  The C++ interface consists mainly of the three classes Service,
  Property, and Group in the namespace ContextProvider.  They are
  declared in the ContextProvider header file.

  Thus, you would typically gain access to the classes like this
  
  \code
  #include <ContextProvider>

  using namespace ContextProvider;

  Service my_service (...);
  \endcode

  If you prefer not to have generic names like "Service" in your code,
  you can of course skip the "using" declaration and refer to the
  classes as "ContextProvider::Service", etc.  If that is too long,
  consider a namespace alias like this:

  \code
  #include <ContextProvider>

  namespace CP = ContextProvider;

  CP::Service my_service (...);
  \endcode
  
  
  The basic pattern to use this library is to create a Service
  instance to represent you on D-Bus and then to add Property
  instances to it for the keys that you want to provide.  Once this is
  done, you can call Property::set() at any time to change the value
  of the property.

  Communication with clients happens asynchronously and this library
  needs a running event loop for that.

  Thus, a simple provider might look like this:

  \code
  #include <ContextProvider>

  using namespace ContextProvider;

  void main (int argc, char **argv)
  {
      QApplication app (argc, argv);

      Service my_service (Service::session_bus, "com.example.simple");
      Property my_property (my_service, "Example.Simple");

      // set initial value of property
      my_property.set (100);

      app.exec();
  }
  \endcode

  If you need to know when someone is actually subscribes to one of
  your values, you can connect to the firstSubscriberAppeared and
  lastSubscriberDisappeared signals of the Property instances.  You
  can also use a Group if you are only interested in whether at least
  one of a set of Property objects is subscribed to.

*/

/*!
    \class Service

    \brief A Service object represents a service name on D-Bus that
    implements the Context Framework interface.

    Every Property object must be associated with a Service object.

    A Service can be running or stopped.  When it is running, it is
    visible via D-Bus and clients can subscribe to its properties.

    It is undefined what happens when you add a Property object to a
    running service after a client has already tried to subscribe to
    it.  The client might be notified about changes to that property,
    or it might not.  (XXX - we remove this undefinedness.)

    Thus, it is best to create a Service object, add all Property
    objects to it immediately, and then enter the main loop, which
    will cause the Service to start.
*/

struct ServicePrivate {
    QDBusConnection::BusType busType;
    QString busName;
    Manager *manager;
    QDBusConnection *connection;
};

Service *Service::defaultService;

/// Creates a Service object for \a busName on the bus indicated by \a
/// busType.  The service will be initially stopped and will
/// automatically start itself when the main loop is entered.
Service::Service(QDBusConnection::BusType busType, const QString &busName, QObject* parent)
    : QObject(parent)
{
    contextDebug() << F_SERVICE << "Creating new Service for" << busName;

    priv = new ServicePrivate;
    priv->busType = busType;
    priv->busName = busName;
    priv->manager = new Manager();
    priv->connection = NULL;

    QTimer::singleShot(0, this, SLOT(start()));
}

Service::~Service()
{
    contextDebug() << F_SERVICE << F_DESTROY << "Destroying Service";
    stop();
    delete priv;
}

Manager *Service::manager()
{
    return priv->manager;
}

/// Sets the Service object as the default one to use when
/// constructing Property objects.  You can only set the default
/// Service once and the Service object that is the default must never
/// be deallocated.
void Service::setAsDefault()
{
    if (defaultService) {
        contextCritical() << F_SERVICE << "Default service already set.";
        return;
    }

    defaultService = this;
}

/// Set the value of \a key to \a val.  A property named \a key must
/// have been registered already, by creating a Property object for
/// it.
void Service::setValue(const QString &key, const QVariant &val)
{
    priv->manager->setKeyValue(key, val);
}

/// Start the Service again after it has been stopped.  All clients
/// will resubscribe to its properties.
void Service::start()
{
    if (priv->connection)
        return;

    priv->connection = new QDBusConnection(QDBusConnection::connectToBus(priv->busType, priv->busName));
    ManagerAdaptor *managerAdaptor = new ManagerAdaptor(priv->manager, priv->connection);

    // Register service
    if (!priv->connection->registerService(priv->busName)) {
        contextCritical() << F_SERVICE << "Failed to register service with name" << priv->busName;
        stop();
        return;
    }

    // Register object
    if (managerAdaptor && !priv->connection->registerObject("/org/freedesktop/ContextKit/Manager", priv->manager)) {
        contextCritical() << F_SERVICE << "Failed to register the Manager object for" << priv->busName;
        stop();
        return;
    }
}

/// Stop the service.  This will cause it to disappear from D-Bus.
void Service::stop()
{
    if (priv->connection == NULL)
        return;

    contextDebug() << F_SERVICE << "Stopping service for bus:" << priv->busName;

    // Unregister
    priv->connection->unregisterObject("/org/freedesktop/ContextKit/Manager");
    priv->connection->unregisterService(priv->busName);

    // Dealloc
    delete priv->connection;
    priv->connection = NULL;
}

/// If the service is running, stop and start it.
void Service::restart()
{
    if (priv->connection) {
        stop();
        start();
    }
}

} // end namespace
