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

#ifndef CONTEXT_H
#define CONTEXT_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QDBusConnection>
#include <QHash>
#include <QVariant>

class Manager;

class Context : public QObject
{
    Q_OBJECT

public:
    explicit Context(const QString &key, QObject *parent = 0);
    QString getKey() const;
    
    void set(bool v);
    void set(int v);
    void set(double v);
    void set(const QString &v);
    void set(const QVariant &v);
    QVariant get();
    void unset();

    bool isValid() const;
    bool isSet() const;

    static bool initService(QDBusConnection::BusType busType, const QString &busName, const QStringList &keys);

private:
    QString key;
};

#endif
