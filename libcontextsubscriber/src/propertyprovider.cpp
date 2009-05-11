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

#include "propertyprovider.h"

#include "propertyhandle.h"

#include <QTimer>

/*!
   \class PropertyProvider

   \brief Keeps the DBUS connection with a specific provider and
   handles subscriptions with the properties of that provider.
*/

void PropertyProvider::getSubscriberFinished(QString objectPath)
{
/*
         dBusSubscriber = new QDBusInterface(busName, path.path(), *connection, this);


         // we can keep this connected all the time
         connect(dBusSubscriber,
                 SIGNAL(Changed(DBusVariantMap, const QStringList &)),
                 this,
                 SLOT(changeValues(const DBusVariantMap &, const QStringList &)));
     }
     
     }*/
}

    
/// Constructs a new instance. ContextProperty handles providers for
/// you, no need to (and can't) call this constructor.
PropertyProvider::PropertyProvider(QDBusConnection::BusType busType, const QString& busName)
    : managerInterface(busType, busName, this)
{
    qDBusRegisterMetaType<DBusVariantMap>();

    // Setup idle timer
    idleTimer.setSingleShot(true);
    idleTimer.setInterval(0);


    // Call GetSubscriber asynchronously
    

}

/// Returns the dbus name and bus type of the provider
QString PropertyProvider::getName() const
{
    return "";
}

/// Subscribes to contextd DBUS notifications for property \a prop.
void PropertyProvider::subscribe(PropertyHandle* prop)
{
    /*QStringList unknowns;
    QStringList keys;
    keys.append(prop->key);

    if (subscriber == NULL) {
        qCritical() << "This provider does not really exist!";
        return;
    }

    QDBusReply<DBusVariantMap> reply = subscriber->Subscribe(keys, unknowns);
    if (!reply.isValid()) {
        qCritical() << "subscribe: bad reply from provider " << dbusServiceString
                    << ": " << reply.error();
        prop->value.clear(); // FIXME: Should this be changed?
        //emit prop->handleChanged();
    }

    qDebug() << "subscribed to " << prop->key << " via provider " << dbusServiceString;

    if (prop->value.type() != prop->type)
        prop->value = QVariant(prop->type);
        changeValues(reply.value(), unknowns, true);*/
}

/// Unsubscribes from contextd DBUS notifications for property \a prop.
void PropertyProvider::unsubscribe(PropertyHandle* prop)
{
    /*QStringList keys;
    keys.append(prop->key);

    if (subscriber == NULL) {
        qCritical() << "This provider does not really exist!";
        return;
    }

    QDBusReply<void> reply = subscriber->Unsubscribe(keys);

    if (!reply.isValid())
        qWarning() << "Could not unsubscribe! This should not happen!" << reply.error() <<
        " Provider: " << dbusServiceString;

        qDebug() << "unsubscribed from " << prop->key << " via provider: " << dbusServiceString;*/
}


/// Slot, handling changed values coming from contextd over DBUS.
/*void PropertyProvider::changeValues(const DBusVariantMap& values,
                                    const QStringList& unknowns,
                                    const bool processingSubscription)
{
    const QHash<QString, PropertyHandle*> &properties =
        PropertyManager::instance()->properties;

    for (QMap<QString, QDBusVariant>::const_iterator i = values.constBegin();
         i != values.constEnd(); ++i) {
        QString key = i.key();
        PropertyHandle *h = properties.value(key, 0);
        if (h == 0) {
            qWarning() << "Received property not in registry:" << key;
            continue;
        }
        if (h->provider != this) {
            qWarning() << "Received property not handled by this provider:" << key;
            continue;
        }

        QVariant v = i.value().variant();
        if (v.type() == h->type) {
            // The type of the received value matches the known type of the property

            if (h->value == v && h->value.isNull() == v.isNull()) {
                // The DBus message contains the current value of the property
                // Value shouldn't be changed
                if (!processingSubscription) qWarning() << "PROVIDER ERROR: Received unnecessary DBUS signal for property" << key;
            }
            else {
                h->value = v;
                emit h->valueChanged();
            }
        }
        else {
            qWarning() << "Type mismatch for:" << key << v.type() << "!=" << h->type;
            h->value.clear();
            emit h->valueChanged();
        }
    }

    // clear the undeterminable ones
    for (QStringList::const_iterator i = unknowns.begin(); i != unknowns.end(); ++i) {
        QString key = *i;
        PropertyHandle *h = properties.value(key, 0);
        if (h == 0) {
            qWarning() << "Received property not in registry: " << key;
            continue;
        }
        if (h->provider != this) {
            qWarning() << "Received property not handled by this provider: " << key;
            continue;
        }

        if (h->value == QVariant(h->type) && h->value.isNull() == true) {
            if (!processingSubscription) qWarning() << "PROVIDER ERROR: Received unnecessary DBUS signal for property" << key;
        }
        else {
            h->value = QVariant(h->type); // Note: Null but typed QVariant
            emit h->valueChanged();
        }
    }
    }*/

PropertyProvider::~PropertyProvider()
{
    if (subscriber != 0)
        delete subscriber;

}
