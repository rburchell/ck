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

#include "contextproperty.h"
#include "propertylistener.h"
#include "commandwatcher.h"
#include <QCoreApplication>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QDebug>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if (getenv("CONTEXT_CLI_DISABLE_TYPE_CHECK"))
        ContextProperty::setTypeCheck(false);
    else
        ContextProperty::setTypeCheck(true);

    if (getenv("CONTEXT_CLI_IGNORE_COMMANDER"))
        ContextProperty::ignoreCommander();

    QCoreApplication app(argc, argv);

    QStringList args = app.arguments();
    if (args.count() <= 1)
        qWarning() << "Started without properties, if you want properties at startup, pass them as arguments";

    QMap<QString, ContextProperty*> properties;

    args.pop_front();
    foreach (QString key, args) {
        if (properties[key] == 0) {
            properties[key] = new ContextProperty(key, QCoreApplication::instance());
            new PropertyListener(properties[key]);
        }
    }

    new CommandWatcher(STDIN_FILENO, &properties, QCoreApplication::instance());

    return app.exec();
}
