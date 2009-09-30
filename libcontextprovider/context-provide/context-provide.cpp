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

#define COMMANDER "org.freedesktop.ContextKit.Commander"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    QStringList args = app.arguments();
    QString busName;
    QDBusConnection::BusType busType = QDBusConnection::SessionBus;
    QTextStream out(stdout);
    QTextStream err(stderr);
    bool silent = false;

    // First, try silently dropping --v2
    if (args.contains("--v2")) {
        args.removeAll("--v2");
    }

    if (args.contains("--help") || args.contains("-h")) {
        // Help? Show it and be gone.
        out << "Usage: context-provide [--silent] [--session | --system] [BUSNAME]\n";
        out << "BUSNAME is " COMMANDER " by default, and bus is session.\n";
        return 0;
    }

    // Silent?
    if (args.contains("--silent") || args.contains("-q")) { // quiet
        silent = true;
        args.removeAll("--silent");
    }

    // session/system
    if (args.contains("--session")) {
        busType = QDBusConnection::SessionBus;
        args.removeAll("--session");
    }

    if (args.contains("--system")) {
        busType = QDBusConnection::SystemBus;
        args.removeAll("--system");
    }

    if (args.count() < 2) {
        // No arguments at all? Use commander by default.
        args.push_back(COMMANDER);
    }

    // Parameter? Extract the session bus and type from it.
    busName = args.at(1);

    Service service(busType, busName);
    service.setAsDefault();

    // FIXME: check the error status of service and exit here, if the
    // registration was unsuccessful
    if (!silent) {
        out << "Service: " << busName << " on " <<
            ((busType == QDBusConnection::SessionBus) ? "session" : "system") << "\n";
    }
    out.flush();

    // 0 -> prog name
    // 1 -> busname
    // 2 -> type of prop1
    // 3 -> name of prop1
    // 4 -> value of prop1
    // 5 -> type of prop2
    // 6 -> name of prop2
    // 7 -> value of prop2
    // ...
    if (args.count() % 3 != 2) {
        err << "Wrong number of properties\n";
        return 1;
    }

    CommandWatcher commandWatcher(busName, busType, STDIN_FILENO, silent, QCoreApplication::instance());

    for (int i=2; i < args.count(); i+=3)
        commandWatcher.addCommand(args.mid(i, 3));

    return app.exec();
}
