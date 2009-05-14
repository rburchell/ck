#include "dbusnamelistener.h"
#include "sconnect.h"

#include <QDBusConnectionInterface>

DBusNameListener::DBusNameListener(const QDBusConnection::BusType busType, const QString &busName) :
    busName(busName)
{
    QDBusConnection connection("");

    // Create DBus connection
    if (busType == QDBusConnection::SessionBus) {
        connection = QDBusConnection::sessionBus();
    } else if (busType == QDBusConnection::SystemBus) {
        connection = QDBusConnection::systemBus();
    } else {
        qCritical() << "Invalid bus type: " << busType;
        return;
    }

    if (!connection.isConnected()) {
        qCritical() << "Couldn't connect to DBUS: ";
        return;
    }

    sconnect(connection.interface(),
             SIGNAL(serviceOwnerChanged(const QString&, const QString&, const QString&)),
             this,
             SLOT(onServiceOwnerChanged(const QString&, const QString&, const QString&)));
}

void DBusNameListener::onServiceOwnerChanged(const QString &name, const QString &oldOwner, const QString &newOwner)
{
    if (name == busName) {
        if (oldOwner == "")
            emit nameAppeared();
        else if (newOwner == "")
            emit nameDisappeared();
    }
}
