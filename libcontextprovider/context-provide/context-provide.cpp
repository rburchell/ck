/*
 * Copyright (C) 2009 Nokia Corporation.
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
#include <QString>
#include <QStringList>
#include <QMap>
#include <QDebug>
#include <stdlib.h>
#include <service.h>
#include "commandwatcher.h"

using namespace ContextProvider;

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    QStringList args = app.arguments();
    QString busName;
    QDBusConnection::BusType busType;
    QTextStream out(stdout);
    bool silent = false;

    // First, try silently dropping --v2
    if (args.contains("--v2")) {
        args.removeAll("--v2");
    }

    // Silent?
    if (args.contains("--silent")) {
        silent = true;
        args.removeAll("--silent");
    }

    if (args.count() > 1 && args.at(1) == "--v2")
        args.pop_front();

    if (args.count() < 2) {
        // No arguments at all? Use commander session bus.
        busName = "org.freedesktop.ContextKit.Commander";
        busType = QDBusConnection::SessionBus;
    } else {
        args.pop_front();
        if (args.at(0) == "--help" || args.at(0) == "-h") {
            // Help? Show it and be gone.
            out << "Usage: context-provide [BUSNAME][:BUSTYPE]\n";
            out << "BUSTYPE is 'session' or 'system'. Session if not specified.\n";
            return 0;
        } else {
            // Parameter? Extract the session bus and type from it.
            QStringList parts = args.at(0).split(':');
            busName = parts.at(0);

            if (parts.count() > 1)
                busType = (parts.at(1) == "system") ? QDBusConnection::SystemBus : QDBusConnection::SessionBus;
            else
                busType = QDBusConnection::SessionBus;
        }
    }

    if (!silent)
        out << "Using bus:" << busName << ((busType == QDBusConnection::SessionBus) ? "session bus" : "system bus") << "\n";

    Service service(busType, busName);
    service.setAsDefault();
    service.start();

    out.flush();
    CommandWatcher commandWatcher(busName, busType, STDIN_FILENO, silent, QCoreApplication::instance());

    return app.exec();
}
