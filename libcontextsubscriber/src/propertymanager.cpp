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

#include "propertymanager.h"

#include "propertyprovider.h"
#include "propertyhandle.h"


#include <fcntl.h>
#include <string>


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
            provider = new PropertyProvider((bus_kind == 1 ? QDBusConnection::SystemBus : QDBusConnection::SessionBus), bus_name);
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
        commander = new PropertyProvider(QDBusConnection::SessionBus, "org.freedesktop.ContextKit.Commander");
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
