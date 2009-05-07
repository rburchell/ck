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

#include <QStringList>
#include <QHash>
#include <QString>
#include <QDebug>
#include <fcntl.h>
#include <string>

#include "propertyhandle.h"
#include "manageriface_p.h"

/*!
   \class PropertyHandle

   \brief A common handle for a context property.

   Only one handle exists at a time for a context property, no matter
   how much ContextProperty objects are created for it.
*/

/// Constructs a new instance. ContextProperty creates the handles, you
/// don't have to (and can't) call this constructor ever.
PropertyHandle::PropertyHandle(const QString& key)
    : KeyHandle(key), provider(NULL), type(QVariant::Invalid), subscribeCount(0)
{
    update_provider();
}

/// Increase the subscribeCount of this context property and subscribe to DBUS if neccessary.
void PropertyHandle::subscribe()
{
    ++subscribeCount;
    if (subscribeCount == 1 && provider)
        provider->subscribe(this);
}

/// Decrease the subscribeCount of this context property and unsubscribe from DBUS if neccessary.
void PropertyHandle::unsubscribe()
{
    --subscribeCount;
    if (subscribeCount == 0 && provider)
        provider->unsubscribe(this);
}

/// True if number of active subscriptions to this node is greater
/// than zero and we have received the reply to the Subscribe call on
/// D-Bus.
bool PropertyHandle::subscribed() const
{
    /* XXX - subscription is synchronous right now, so we don't need to
       check for that.
     */
    return (subscribeCount > 0);
}

/// Check the registry for the provider providing this property (named \c key) and
/// set up subscription and the \c provider pointer accordingly.
void PropertyHandle::update_provider()
{
    qDebug() << "PropertyHandle::update_provider";
    PropertyManager *manager = PropertyManager::instance();
    int provider_index;

    QVariant::Type new_type = QVariant::Invalid;
    PropertyProvider *new_provider = 0;

    if (manager->lookupProperty(key, provider_index, new_type, typeName, description))
        new_provider = manager->getProvider (provider_index);

    if (new_provider != provider) {
        qDebug() << "New provider:" << key;
        if (subscribeCount > 0) {
            if (provider)
                provider->unsubscribe(this);
            provider = new_provider;
            if (provider)
                provider->subscribe(this);
            emit providerChanged();
        } else
            provider = new_provider;
    } else
        qDebug() << "Same provider:" << key;

    // XXX - what do we do when the type changes?

    if (new_type != type && type != QVariant::Invalid)
        qWarning() << "Type of" << key << "changed!";

    type = new_type;
}

/// Return the DBus name and bus type of the provider providing this property.
QString PropertyHandle::providerName() const
{
    if (provider) {
        return provider->getName();
    }
    return "";
}

/*!
   \class PropertyProvider

   \brief Keeps the DBUS connection with a specific provider and
   handles subscriptions with the properties of that provider.
*/

/// Constructs a new instance. ContextProperty handles providers for
/// you, no need to (and can't) call this constructor.
PropertyProvider::PropertyProvider (int bus_kind, const QString& bus_name)
{
    qDBusRegisterMetaType<ValueSetMap>();

    subscriber = NULL;

    QDBusConnection *connection;
    dbusServiceString = bus_name;
    if (bus_kind == 0) {
        dbusServiceString += " (session)";
        connection = new QDBusConnection(QDBusConnection::sessionBus());
    } else if (bus_kind == 1) {
        dbusServiceString += " (system)";
        connection = new QDBusConnection(QDBusConnection::systemBus());
    } else {
        qCritical() << "Invalid bus kind: " << bus_kind;
        return;
    }

    if (!connection->isConnected()) {
        qCritical() << "Couldn't connect to DBUS: " << dbusServiceString;
        delete connection;
        return;
    }

    ManagerInterface *manager =
        new ManagerInterface(bus_name, "/org/freedesktop/ContextKit/Manager", *connection, this);

    QDBusReply<QDBusObjectPath> subscriptionObject = manager->GetSubscriber();
    delete manager;

    if (subscriptionObject.isValid()) {
        subscriber = new SubscriberInterface(bus_name, subscriptionObject.value().path(), *connection, this);
        delete connection;
    } else {
        qWarning() << "There was no provider started for:" << dbusServiceString << ", error: "
                   << subscriptionObject.error();
        delete connection;
        return;
    }

    if (!subscriber->isValid()) {
        qWarning() << "Subscriber can't connect to " << dbusServiceString;
        delete subscriber;
        subscriber = 0;
        return;
    }

    // we can keep this connected all the time
    connect(subscriber,
            SIGNAL(Changed(ValueSetMap, const QStringList &)),
            this,
            SLOT(changeValues(const ValueSetMap &, const QStringList &)));
}

/// Returns the dbus name and bus type of the provider
QString PropertyProvider::getName() const
{
    return dbusServiceString;
}

/// Subscribes to contextd DBUS notifications for property \a prop.
void PropertyProvider::subscribe(PropertyHandle* prop)
{
    QStringList unknowns;
    QStringList keys;
    keys.append(prop->key);

    if (subscriber == NULL) {
        qCritical() << "This provider does not really exist!";
        return;
    }

    QDBusReply<ValueSetMap> reply = subscriber->Subscribe(keys, unknowns);
    if (!reply.isValid()) {
        qCritical() << "subscribe: bad reply from provider " << dbusServiceString
                    << ": " << reply.error();
        prop->value.clear();
        emit prop->handleChanged();
    }

    qDebug() << "subscribed to " << prop->key << " via provider " << dbusServiceString;

    if (prop->value.type() != prop->type)
        prop->value = QVariant(prop->type);
    changeValues(reply.value(), unknowns);
}

/// Unsubscribes from contextd DBUS notifications for property \a prop.
void PropertyProvider::unsubscribe(PropertyHandle* prop)
{
    QStringList keys;
    keys.append(prop->key);

    if (subscriber == NULL) {
        qCritical() << "This provider does not really exist!";
        return;
    }

    QDBusReply<void> reply = subscriber->Unsubscribe(keys);

    if (!reply.isValid())
        qWarning() << "Could not unsubscribe! This should not happen!" << reply.error() <<
        " Provider: " << dbusServiceString;

    qDebug() << "unsubscribed from " << prop->key << " via provider: " << dbusServiceString;
}

/// Slot, handling changed values coming from contextd over DBUS.
void PropertyProvider::changeValues(const ValueSetMap& values,
                                    const QStringList& unknowns)
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
                qWarning() << "PROVIDER ERROR: Received unnecessary DBUS signal for property" << key;
            }
            else {
                h->value = v;
                emit h->handleChanged();
            }
        }
        else {
            qWarning() << "Type mismatch for:" << key << v.type() << "!=" << h->type;
            h->value.clear();
            emit h->handleChanged();
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
            qWarning() << "PROVIDER ERROR: Received unnecessary DBUS signal for property" << key;
        }
        else {
            h->value = QVariant(h->type); // Note: Null but typed QVariant
            emit h->handleChanged();
        }
    }
}

PropertyProvider::~PropertyProvider()
{
    if (subscriber != 0)
        delete subscriber;
}

/*!
   \class PropertyManager

   \brief Handles the creation of PropertyProvider and PropertyHandle
   objects according to the registry.
*/
PropertyManager::PropertyManager()
  : registry_fd(-1), fsWatcher(0), commandingEnabled(true), commander(0), commanded(false)
{
    update_registry();

    // Start listening to Commander
    QDBusConnection sessionBusConnection = QDBusConnection::sessionBus();

    connect(sessionBusConnection.interface(),
            SIGNAL(serviceOwnerChanged(const QString &, const QString &, const QString &)),
            this,
            SLOT(on_nameOwnerChanged(const QString &, const QString &, const QString &)));

    // Check if Commander is already present
    if (sessionBusConnection.interface()->isServiceRegistered("org.freedesktop.ContextKit.Commander")) {
        qDebug() << "Commander already there";
        commanderAppeared();
    }

    // Note: First we start listening to the NameOwnerChanged signal, and after that we check if
    // the Commander is already present. This way, we might call commanderAppeared twice. If we do
    // these two things the other way, we might not notice the Commander starting after the check but
    // before we start listening.
}

PropertyManager::~PropertyManager()
{
    for (int i = 0; i < 2; i++) {
        foreach (PropertyProvider *p, providers[i])
            delete p;
        providers[i].clear();
    }

    foreach (PropertyHandle *h, properties)
        delete h;
    properties.clear();

    if (fsWatcher)
        delete fsWatcher;

    if (registry_fd >= 0) {
        cdb_free(&registry);
        close(registry_fd);
    }

    delete commander;
}

#define REGISTRY_NAME PREFIX "/share/context-providers/context-providers.cdb"

/// Opens the CDB database pointed to by environment variable
/// CONTEXT_PROVIDERS (default:
/// $prefix/share/context-providers/context-providers.db), and populates
/// the registry based on that.
void PropertyManager::update_registry ()
{
    const char *registry_name;

    registry_name = getenv ("CONTEXT_PROVIDERS");
    if (registry_name == NULL)
        registry_name = REGISTRY_NAME;

    qDebug() << "Opening" << registry_name;

    if (fsWatcher)
        delete fsWatcher;
    if (registry_fd >= 0) {
        cdb_free(&registry);
        close (registry_fd);
    }

    registry_fd = open (registry_name, O_RDONLY);
    if (registry_fd < 0) {
        qWarning() << "Can't open registry file:" << registry_name;
    } else if (cdb_init(&registry, registry_fd) < 0) {
        qWarning() << "Can't cdb_init" << registry_name;
        close (registry_fd);
        registry_fd = -1;
    }

    foreach (PropertyHandle *h, properties)
        h->update_provider();

    fsWatcher = new QFileSystemWatcher(this);
    fsWatcher->addPath(registry_name);
    connect(fsWatcher, SIGNAL(fileChanged(const QString &)),
            this, SLOT(update_registry()));
}

/// Looks up a provider's bus kind (session (0) or system (1)) and bus
/// name based on its index in the registry.
bool PropertyManager::lookupProvider(int provider_index,
                                     int &bus_kind,
                                     QString &bus_name)
{
    QByteArray key = QByteArray::number(provider_index);

    if (registry_fd < 0)
        return false;

    if (cdb_find(&registry, key.data(), key.size()) > 0) {
        unsigned vpos = cdb_datapos(&registry);
        unsigned vlen = cdb_datalen(&registry);
        QByteArray entry(vlen, 0);
        char *entry_kind, *entry_name;

        cdb_read (&registry, entry.data(), vlen, vpos);
        entry_kind = entry.data();
        entry_name = entry_kind + strlen(entry_kind)+1;

        qDebug() << "Found:" << key << "->" << entry_kind << entry_name;

        bus_name = QString(entry_name);
        if (strcmp(entry_kind, "session") == 0)
            bus_kind = 0;
        else if (strcmp(entry_kind, "system") == 0)
            bus_kind = 1;
        else {
            qCritical() << "Unrecognized bus kind:" << entry;
            return false;
        }

        return true;
    } else
        return false;
}

/// Looks up a property's provider and type based on its name in the registry.
bool PropertyManager::lookupProperty(const QString &key, int &provider_index, QVariant::Type &type,
        QString &typeName, QString &description)
{
    struct registry_property {
        short provider_index;
        char type[1];
    };

    std::string std_key = key.toStdString();

    if (registry_fd < 0)
        return false;

    if (cdb_find(&registry, std_key.c_str(), std_key.size()) > 0) {
        unsigned vpos = cdb_datapos(&registry);
        unsigned vlen = cdb_datalen(&registry);
        unsigned char entry[vlen];
        registry_property *prop = (registry_property *)&entry;

        cdb_read (&registry, entry, vlen, vpos);
        qDebug() << "Found:" << key << "->"
                 << prop->provider_index << "with type" << prop->type;

        typeName = prop->type;
        description = (char*)(&prop->type + strlen(prop->type) + 1);

        provider_index = prop->provider_index;
        if (strcmp(prop->type, "INT") == 0)
            type = QVariant::Int;
        else if (strcmp(prop->type, "DOUBLE") == 0)
            type = QVariant::Double;
        else if (strcmp(prop->type, "STRING") == 0)
            type = QVariant::String;
        else if (strcmp(prop->type, "TRUTH") == 0)
            type = QVariant::Bool;
        else if (strcmp(prop->type, "ENUM") == 0)
            type = QVariant::Int;
        else {
            qWarning() << "Unknown type:" << prop->type;
            type = QVariant::Invalid;
            typeName = "Unknown";
        }

        return true;
    } else
        return false;
}

/// Returns a list of all the properties currently available in the registry.
QList<QString> PropertyManager::listProperties ()
{
    QList<QString> result;
    unsigned cptr;
    unsigned vsize = 256;
    char* val;

    if (registry_fd < 0)
        return result;

    val =  (char*)malloc(vsize);

    cdb_seqinit(&cptr, &registry);
    while (cdb_seqnext(&cptr, &registry) > 0) {
        unsigned keylen = registry.cdb_klen;
        if (keylen + 1 > vsize)
            val = (char*)realloc(val, vsize = keylen + 32);
        cdb_readkey(&registry, val);
        val[keylen] = '\0';
        if (keylen > 6 || index(val, '.') != NULL)
            result.append(val);
    }
    free(val);
    return result;
}

/// Constructs a new provider
PropertyProvider* PropertyManager::getProvider(int provider_index)
{
    if (commanded) {
        qDebug() << "Returning commander from getProvider";
        return commander;
    }

    int bus_kind;
    QString bus_name;

    if (lookupProvider(provider_index, bus_kind, bus_name)){
        PropertyProvider *provider = providers[bus_kind][bus_name];
        if (!provider){
            provider = new PropertyProvider(bus_kind, bus_name);
            providers[bus_kind][bus_name] = provider;
        }
        return provider;
    }

    qCritical() << "Provider" << provider_index << "not in registry";
    return NULL;
}

/// Checks the \c properties hash for the context property named \c
/// key and returns it.  If it is not in the hash, then constructs it
/// and puts it in the hash before returning it.
PropertyHandle* PropertyManager::getHandle(const QString& key)
{
    PropertyHandle *handle = properties.value(key, 0);

    if (!handle){
        handle = new PropertyHandle (key);
        properties.insert(key, handle);
    }

    return handle;
}

/// Returns the singleton instance.
PropertyManager* PropertyManager::instance()
{
    static PropertyManager *myInstance = 0;

    if (!myInstance)
        myInstance = new PropertyManager();

    return myInstance;
}

/// Listens to DBus to notice Context Commander
/// appearing / disappearing.
void PropertyManager::on_nameOwnerChanged(const QString & name,
    const QString & oldName, const QString & newName)
{
    if (name == "org.freedesktop.ContextKit.Commander") {
        if (oldName == "") {
            qDebug() << "PropertyManager: Commander appeared";
            commanderAppeared();
        }
        else if (newName == "") {
            qDebug() << "PropertyManager: Commander disappeared";
            commanderDisappeared();
        }
    }
}

/// Modifies the behaviour of ContextProperty instances so that all connections
/// are redirected to Context Commander.
void PropertyManager::commanderAppeared()
{
    if (commandingEnabled == false)
    {
        return;
    }

    // Redirect all handles so that they connect to the Commander.
    commanded = true;

    if (commander == 0) {
        commander = new PropertyProvider(0, "org.freedesktop.ContextKit.Commander");
    }

    foreach (PropertyHandle* handle, properties.values()) {
        handle->update_provider();
    }
}

/// Modifies the behaviour of ContextProperty instances so that all connections
/// are redirected to real providers.
void PropertyManager::commanderDisappeared()
{
    if (commandingEnabled == false)
    {
        return;
    }

    // Cancel the redirection so that all handles connect to their
    // original providers.
    commanded = false;

    foreach (PropertyHandle* handle, properties.values()) {
        handle->update_provider();
    }

    delete commander;
    commander = 0;
}

/// Disables the commanding behaviour, so that connections are never
/// redicrected to Context Commander.
void PropertyManager::disableCommanding()
{
    commandingEnabled = false;
}
