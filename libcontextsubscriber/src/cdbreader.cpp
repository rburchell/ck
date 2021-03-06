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

#include <stdlib.h>
#include <cdb.h>
#include <fcntl.h>
#include <QDebug>
#include <QByteArray>
#include <QFile>
#include <QDataStream>
#include "cdbreader.h"
#include "logging.h"
#include "loggingfeatures.h"


/*!
    \class CDBReader

    \brief A wrapper class to read data from a tiny-cdb database.

    This class is not a part of the public API.  The reader operates
    on string-qvariant pairs: can read a qvariant value for a string
    key or a list of qvariant values for a string key. The reader
    automatically closes the underlying filesystem resource on
    destruction but can be also closed manually.

    Reading from a closed reader will return empty strings.
*/

/// Constructs a new CDBReader reading from cdb database at \a dbpath
/// \param dbpath Path to the database.
CDBReader::CDBReader(const QString &dbpath, QObject *parent)
    : QObject(parent), path(dbpath)
{
    contextDebug() << F_CDB << "cdb reader created for:" << dbpath;
    cdb = NULL;
    fd = 0;

    reopen();
}

/// Destroys the object automatically closing the database and file.
CDBReader::~CDBReader()
{
    close();
}

/// Closes the reader and the underlying file on the filesystem.
void CDBReader::close()
{
    if (cdb) {
        free(cdb);
        cdb = NULL;
    }

    if (fd > 0) {
        ::close(fd);
        fd = 0;
    }
}

/// Reopens the reader for reading.
/// It will first close the current reader if it's open.
void CDBReader::reopen()
{
    close();

    if (! QFile::exists(path))
        return;

    fd = open(path.toLocal8Bit().constData(), O_RDONLY);

    if (fd > 0) {
        cdb = calloc(1, sizeof(struct cdb));
        if (cdb_init((struct cdb*) cdb, fd) != 0) {
            free(cdb);
            cdb = NULL;
        }
    }
}

/// Returns all values associated with the given key from the database.
/// \param key The key name in the database.
QVariantList CDBReader::valuesForKey(const QString &key) const
{
    if (! cdb) {
        contextDebug() << F_CDB << "Trying to read values for key:" << key << "from database that is closed!";
        return QVariantList();
    }

    QVariantList list;
    QByteArray utf8Data = key.toUtf8();
    unsigned int klen = utf8Data.size();
    const char *kval = utf8Data.constData();

    struct cdb_find cdbf;
    cdb_findinit(&cdbf, (struct cdb*) cdb, kval, klen);

    QVariant valVariant;
    char* val;
    QByteArray valArray;
    QDataStream ds;
    QBuffer buffer;
    ds.setDevice(&buffer);

    while(cdb_findnext(&cdbf) > 0) {
        unsigned int vpos = cdb_datapos((struct cdb*) cdb);
        unsigned int vlen = cdb_datalen((struct cdb*) cdb);
        val = (char*) cdb_get((struct cdb*) cdb, vlen, vpos);
        buffer.close();
        valArray.setRawData(val, vlen);
        buffer.setBuffer(&valArray);
        buffer.open(QBuffer::ReadOnly);
        ds >> valVariant;
        list << valVariant;
    }

    return list;
}

/// Returns a value for the given \a key.
/// First value is returned if there are many values for one key.
/// \param key The key name in the database.
QVariant CDBReader::valueForKey(const QString &key) const
{
    QVariantList ret = valuesForKey(key);
    if (ret.size() == 0) return QVariant();
    return ret[0];
}

/// Returns the current state of the reader. Reader is not readable if
/// it was created with a path that doesn't exist or if it was closed.
bool CDBReader::isReadable()
{
    return (fd > 0);
}

/// Returns the file descriptor used by the reader. Returns 0
/// when the reader is closed.
int CDBReader::fileDescriptor() const
{
    return fd;
}


