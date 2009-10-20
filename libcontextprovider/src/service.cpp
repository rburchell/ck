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
#include "servicebackend.h"
#include "property.h"
#include "logging.h"
#include "sconnect.h"
#include "loggingfeatures.h"
#include <QDBusConnection>
#include <QDebug>

namespace ContextProvider {

/*!

  \mainpage Providing values for Context properties

  This library implements the provider side of the ContextKit's D-Bus
  protocol.  It has both a C++ and a C interface, so you can choose
  which you prefer. For the documentation of the C API, see \ref CApi.

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

  void main(int argc, char **argv)
  {
      QApplication app(argc, argv);

      Service my_service(QDBusConnection::SessionBus, "com.example.simple");
      Property my_property(my_service, "Example.Simple");

      // set initial value of property
      my_property.set(100);

      app.exec();
  }
  \endcode

  If you need to know when someone actually subscribes to one of
  your values, you can connect to the firstSubscriberAppeared and
  lastSubscriberDisappeared signals of the Property instances.  You
  can also use a Group if you are only interested in whether at least
  one of a set of Property objects is subscribed to.
*/

/*!
    \class Service

    \brief A Service object is a proxy representing a service on D-Bus
    that implements the ContextKit interface. The lifespan of the
    Service object is not tied to the real service lifespan. This way
    a service can be accessed and controlled from different parts of
    the code.

    When you first create a Service object with given bus type / bus
    name parameters the real D-Bus service instance is actually
    created and started.  Future references to this bus type / bus
    name will give you a Service instance that points to the very same
    service as the first Service initialization that started it.  In
    other words, we can say that the Service class represents a proxy
    interface to the real D-Bus service.

    When the last instance of Service is destroyed, the real service
    is automatically terminated and destroyed (there is a simple ref
    counting mechanism involved to guarantee that).

    Consider the following examples:

    \code
    Service *s1 = new Service(QDBusConnection::SessionBus, "com.example.simple");
    Service *s2 = new Service(QDBusConnection::SessionBus, "com.example.simple");
    // s1 and s2 represent the same service

    s2->stop();
    // Both s1 and s2 are now stopped

    s1->start();
    // Both s1 and s2 are now started

    delete s1; // s2 is still valid, service is present
    delete s2; // the "com.example.simple" just disappeared from D-Bus
    \endcode

    Every Property object must be associated with a Service object.

    A Service can be running or stopped.  When it is running, it is
    visible via D-Bus and clients can subscribe to its properties.

    It is undefined what happens when you add a Property object to a
    running service.  Likewise, it is undefined what happens when you
    add a Property to a Group while the Service of that Property
    object is running.

    Thus, it is best to create a Service object, add all Property
    objects to it immediately, create the Group objects for them, and
    then enter the main loop.

    Libcontextprovider can share the same QDBusConnection that is used
    in the provider program. In that case, the Service must be created
    by passing the QDBusConnection. The Service will not register any
    service name on the connection. It is advisable to register the
    service name as late as possible, after initializing all Services
    and Properties and after registering other objects the provider
    declares on D-Bus.

    \code
    int main(int argc, char** argv) {
        QCoreApplication app(argc, argv);
        QDBusConnection systemBus = QDBusConnection::SystemBus();

        // D-Bus registrations can happen before or after initializing
        // the Services and Properties
        connection.registerObject(...);

        // Now myService will share the connection and won't register
        // any service name
        Service *myService = new Service(systemBus);
        Property* property = new Property(myService, "My.Property");

        // Important: registering the service name should be done as
        // late as possible, to prevent clients from connecting to us
        // before all objects have been registered.
        connection.registerService("my.service.name");

        return app.exec();
    }
    \endcode

*/

/// Creates a Service proxy object which shares the \a connection with
/// the provider program. The Service will not register any service
/// name on the conneciton. If the service is accessed for the first
/// time it'll be created and set up. If the service with the given
/// parameters already exists the created object represents a
/// controller to a previously-created service.  A new Service will be
/// started when it is constructed.
Service::Service(QDBusConnection connection, QObject *parent)
{
    // this singleton accessor method always autostart
    backend = ServiceBackend::instance(connection);
    backend->ref();
}


/// Creates a Service proxy object for \a busName on the bus indicated
/// by \a busType. The Service will register the given \a busName on
/// D-Bus. If the service is accessed for the first time it'll be
/// created and set up. If the service with the given parameters
/// already exists the created object represents a controller to a
/// previously-created service.  A new Service will be started when it
/// is constructed if \a autoStart is true (which is the default).
Service::Service(QDBusConnection::BusType busType, const QString &busName, bool autoStart, QObject* parent)
    : QObject(parent)
{
    backend = ServiceBackend::instance(busType, busName, autoStart);
    backend->ref();
}

/// A convenient constructor, where autoStart is always true.
Service::Service(QDBusConnection::BusType busType, const QString &busName, QObject* parent)
    : QObject(parent)
{
    backend = ServiceBackend::instance(busType, busName, true);
    backend->ref();
}

/// Destroys this Service instance. The actual service on D-Bus is
/// destroyed and stopped if this object is a last instance pointing
/// at the actual service with the given constructor parameters
/// (QDBusConnection or bus type and bus name).
Service::~Service()
{
    contextDebug() << F_SERVICE << F_DESTROY << "Destroying Service";
    backend->unref();
}

/// Sets the Service object as the default one to use when
/// constructing Property objects.
void Service::setAsDefault()
{
    backend->setAsDefault();
}

/// Set the value of \a key to \a val.  A property named \a key must
/// have been registered already, by creating a Property object for
/// it.
void Service::setValue(const QString &key, const QVariant &val)
{
    backend->setValue(key, val);
}

/// Set (override) the QDBusConnection used by the
/// Service. Deprecated; use constructor with QDBusConnection
/// parameter instead.
void Service::setConnection(const QDBusConnection &connection)
{
    qCritical("[ContextProvider] Service::setConnection() is deprecated; use Service(QDBusConnection).");
    backend->stop();
    backend->connection = connection;
    backend->start();
}

/// Start the Service again after it has been stopped. In the case of
/// shared connection, the objects will be registered to D-Bus. In the
/// case of non-shared connection, also the service name will be
/// registered on D-Bus. Returns true on success, false otherwise.
bool Service::start()
{
    return backend->start();
}

/// Stop the service. In the case of shared connection, this will
/// cause the related objects to be unregistered, but the bus name
/// will still be on D-Bus. In the case of non-shared connection, this
/// will cause the service to disappear from D-Bus completely.
void Service::stop()
{
    backend->stop();
}

/// Stop and start the service. This function cannot be used when the
/// Service shares a QDBusConnection with the provider program. When
/// that is the case, you must stop the service, unregister the D-Bus
/// bus name, register it again (to force clients of context
/// properties to resubscribe), and finally start the service again.
void Service::restart()
{
    if (backend->sharedConnection())
        qFatal("[ContextProvider] Service::restart() is not compatible with shared connections");

    stop();
    start();
}

} // end namespace
