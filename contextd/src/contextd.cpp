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

#include <QCoreApplication>
#include "halprovider.h"
#include "lowmemprovider.h"
#include "context.h"

using namespace ContextD;

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    QList<Provider*> providerList;
    providerList << new HalProvider();
    providerList << new LowMemProvider();
    QStringList keys;

    // First collect all keys
    foreach (Provider *provider, providerList) {
        keys << provider->keys();
    }

    // Now init service
    Context::initService(QDBusConnection::SessionBus, QString("org.freedesktop.ContextKit.contextd"), keys);

    // Now init all providers
    foreach (Provider *provider, providerList) {
        provider->initialize();
    }
    
    return app.exec();
}

