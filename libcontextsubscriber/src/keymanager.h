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

#ifndef KEYMANAGER_H
#define KEYMANAGER_H

#include <QStringList>
#include <QHash>
#include <QString>
#include <QVariant>

/* The key/value pairs (context properties) are gathered from one ore
 * more backends.  Backends need to hand out handles that implement
 * the KeyHandle interface.
 *
 * Dispatching to backends is done based on the first component of the
 * key name.  It is currently hardcoded inside the KeyManager
 * singleton.
 */

class KeyHandle : public QObject {
    Q_OBJECT

protected:
    KeyHandle(const QString& key) : key(key), typeName("Unknown") { }
    virtual ~KeyHandle() { };

public:
    virtual void subscribe() = 0;
    virtual void unsubscribe() = 0;
    virtual bool subscribed() const = 0;

    QVariant value; ///< Cached (current) value
    QString key; ///< Name of the key
    QString typeName; ///< Type name of this property.
    QString description; ///< Documentation describing this property.

    virtual void setValue(const QVariant &val);

    virtual QString providerName() const = 0;

signals:
    void handleChanged();
    void providerChanged(); ///< Emitted whenever the provider is changed (e.g. in the registry in the case of context).
};

struct KeyManager {
    static QList<QString> listKeys ();
    static KeyHandle* getHandle(const QString &key);
};

#endif
