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

#ifndef CDBWRITER_H
#define CDBWRITER_H

#include <QObject>
#include <QString>
#include <QVariant>

class CDBWriter : public QObject
{
    Q_OBJECT

public:
    explicit CDBWriter(const QString &path, QObject *parent = 0);
    explicit CDBWriter(int fd, QObject *parent = 0);
    virtual ~CDBWriter();

    void add(const QString &key, const QVariant &val);
    void insert(const QString &key, const QVariant &val);
    void replace(const QString &key, const QVariant &val);
    void close();
    bool isWritable();
    int fileDescriptor() const;

private:
    void *cdbm; ///< A cdb library structure used to read data.
    int fd; ///< A file descriptor pointing to the database.

    void put(const QString &key, const QVariant &val, int flag);
};

#endif
