/*
 * Copyright (C) 2008, 2009 Nokia Corporation.
 *
 * Contact: Marius Vollmer <marius.vollmer@nokia.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef MANAGERADAPTOR_H
#define MANAGERADAPTOR_H

#include <QObject>
#include <QDBusAbstractAdaptor>
#include <QDBusMessage>
#include <QDBusConnection>
#include <QDBusObjectPath>
#include "manager.h"

namespace ContextProvider {

class ManagerAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.ContextKit.Manager")

public:
    ManagerAdaptor (Manager* manager, QDBusConnection *connection);

public slots:
    QDBusObjectPath GetSubscriber(const QDBusMessage &msg);
    void OnServiceOwnerChanged(const QString&, const QString&, const QString&);

private:
    Manager *manager; ///< The managed object.
    QDBusConnection *connection; ///< The connection to operate on.
};

} // namespace ContextProvider

#endif
