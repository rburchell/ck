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

#ifndef CDBREADER_H
#define CDBREADER_H

#include <QStringList>
#include <QObject>

/*!
  \class CDBReader

  \brief A wrapper class to read data from a tiny-cdb database.

  The reader operates only on strings and can read a string value for a string key
  or a list of string values for a string key. The reader automatically closes the 
  underlying filesystem resource on destruction but can be also closed manually. 

  Reading from a closed reader with return empty strings. 
*/

class CDBReader : public QObject
{
    Q_OBJECT

public:
    explicit CDBReader(const QString &dbpath, QObject *parent = 0);
    virtual ~CDBReader();

    void close();
    void reopen();
    QString valueForKey(const QString &key) const;
    QStringList valuesForKey(const QString &key) const;
    bool isReadable();

private:
    QString path;
    void *cdb;
    int fd;
};

#endif
