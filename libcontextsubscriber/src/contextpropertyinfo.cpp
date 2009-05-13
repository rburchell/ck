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

#include "contextpropertyinfo.h"
#include "infobackend.h"
#include "sconnect.h"

ContextPropertyInfo::ContextPropertyInfo(const QString &key, QObject *parent)
    : QObject(parent)
{
    keyName = key;

    if (key != "") {
        sconnect(InfoBackend::instance(), SIGNAL(keyDataChanged(QString)),
                 this, SLOT(onKeyDataChanged(QString)));
    }
}

ContextPropertyInfo::ContextPropertyInfo(const ContextPropertyInfo &other)
{
    keyName = other.keyName;

    if (keyName != "") {
        sconnect(InfoBackend::instance(), SIGNAL(keyDataChanged(QString)),
                 this, SLOT(onKeyDataChanged(QString)));
    }
}


QString ContextPropertyInfo::key() const
{
    return keyName;
}

QString ContextPropertyInfo::doc() const
{
    return InfoBackend::instance()->docForKey(keyName);
}

QString ContextPropertyInfo::type() const
{
    return InfoBackend::instance()->typeForKey(keyName);
}

QString ContextPropertyInfo::providerDBusName() const
{
    return InfoBackend::instance()->providerForKey(keyName);
}

QDBusConnection::BusType ContextPropertyInfo::providerDBusType() const
{
    // FIXME Stub for now!
    return QDBusConnection::SessionBus;
}

/* Slots */

void ContextPropertyInfo::onKeyDataChanged(QString key)
{
    if (key != keyName)
        return;

    // FIXME Cache the values and actually do SEE if it changed
    emit typeChanged(type());
    emit providerChanged(providerDBusName());
}

