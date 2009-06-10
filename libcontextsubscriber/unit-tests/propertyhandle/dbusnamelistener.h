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

// This is a mock implementation.

#ifndef DBUSNAMELISTENER_H
#define DBUSNAMELISTENER_H

#include <QObject>
#include <QDBusConnection>
#include <QString>

template<typename P1, typename P2> class QPair;
template<typename K, typename V> class QMap;

namespace ContextSubscriber {

class DBusNameListener : public QObject
{
    Q_OBJECT
public:
    DBusNameListener(const QDBusConnection::BusType busType,
                     const QString &busName,
                     bool initialCheck = true,
                     QObject* parent = 0);

    bool isServicePresent() const;

signals:
    void nameAppeared();
    void nameDisappeared();

public:
    // For the test program
    bool servicePresent;

    friend class PropertyHandleUnitTests;

};

} // end namespace

#endif
