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

#include "infocdbbackend.h"
#include "infoxmlbackendconfig.h" // FIXME: Should be moved into a universal config
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QFile>
#include <stdlib.h>
#include "sconnect.h"

InfoCdbBackend::InfoCdbBackend(QObject *parent)
    : InfoBackend(parent), reader(InfoCdbBackend::databasePath())
{
    //reader = CDBReader(databasePath());
    // FIXME: Check if database present!
}

QString InfoCdbBackend::name() const
{
    return QString("cdb backend");
}

QStringList InfoCdbBackend::listKeys() const
{
    return reader.valuesForKey("KEYS");
}

QStringList InfoCdbBackend::listKeys(QString providername) const
{
    // FIXME: Not present yet in cdb
    return reader.valuesForKey(providername + ":KEYS");
}

QStringList InfoCdbBackend::listProviders() const
{
    return reader.valuesForKey("PROVIDERS");
}

QString InfoCdbBackend::typeForKey(QString key) const
{
    return reader.valueForKey(key + ":KEYTYPE");
}

QString InfoCdbBackend::docForKey(QString key) const
{
    return reader.valueForKey(key + ":KEYDOC");
}

QString InfoCdbBackend::providerForKey(QString key) const
{
    return reader.valueForKey(key + ":PROVIDER");
}

/* Private */

QString InfoCdbBackend::databasePath()
{
    const char *regpath = getenv("CONTEXT_PROVIDERS");
    if (! regpath)
        regpath = DEFAULT_REGISTRY_PATH;

    return QString(regpath) + "context-providers.cdb";
}

