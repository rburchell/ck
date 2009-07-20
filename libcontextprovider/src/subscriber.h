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

#ifndef SUBSCRIBER_H
#define SUBSCRIBER_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QVariant>
#include "queuedinvoker.h"

class Manager;

class Subscriber : public QueuedInvoker
{
    Q_OBJECT

public:
    Subscriber(const QString &dbusPath, Manager *manager);
    ~Subscriber();

    QMap<QString, QVariant> subscribe(const QStringList &keys, QStringList &undetermined);
    void unsubscribe(const QStringList &keys);
    QString dbusPath() const;
    bool isSubscribedToKey(const QString &key);

private:
    QString path; ///< Dbus object path.
    Manager *manager; ///< Parent Manager (service).
    QStringList subscribedKeys; ///< List of keys we're currently subscribed to.
    QStringList changedKeys; ///< List of keys that got changed and need to be processed in Subscriber::handleChanged.
    
    void buildMapAndUndetermined(const QStringList &keys, QMap<QString, QVariant> &map, QStringList &undetermined);
    
private slots:
    void onManagerKeyValueChanged(const QString &key);
    Q_INVOKABLE void handleChanged();
    
signals:
    /// Emitted when any of the subscribed keys changes it's value. The \a values map contains
    /// a mapping of keys -> new values. The \a undetermined list contains the names of keys
    /// that could not be determined. The \a values and \a undetermined contains only the keys
    /// that actually changed value.
    void changed(const QMap<QString, QVariant> &values, const QStringList &undetermined);
};

#endif
