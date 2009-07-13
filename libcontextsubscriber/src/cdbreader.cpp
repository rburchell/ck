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
#include <QFile>
#include "cdbreader.h"
#include "logging.h"
#include "loggingfeatures.h"


/*!
    \class CDBReader

    \brief A wrapper class to read data from a tiny-cdb database.

    This class is not a part of the public API.
    The reader operates only on strings and can read a string value for a string key
    or a list of string values for a string key. The reader automatically closes the
    underlying filesystem resource on destruction but can be also closed manually.

    Reading from a closed reader with return empty strings.
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
    contextDebug() << F_CDB << F_DESTROY << "CDBReader::~CDBReader, dbpath:" << path;
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

    fd = open(path.toUtf8().constData(), O_RDONLY);

    if (fd > 0) {
        cdb = calloc(1, sizeof(struct cdb));
        if (cdb_init((struct cdb*) cdb, fd) != 0) {
            free(cdb);
            cdb = NULL;
        }
    }
}

/// Returns a string value for the given \a key.
/// First value is returned if there are many values for one key.
/// \param key The key name in the database.
QString CDBReader::valueForKey(const QString &key) const
{
    if (! cdb) {
        contextDebug() << F_CDB << "Trying to read value key:" << key << "from database that is closed!";
        return "";
    }

    QByteArray utf8Data = key.toUtf8();
    unsigned int klen = utf8Data.size();
    const char *kval = utf8Data.constData();

    if (cdb_find((struct cdb*) cdb, kval, klen)) {
        unsigned int vpos = cdb_datapos((struct cdb*) cdb);
        unsigned int vlen = cdb_datalen((struct cdb*) cdb);
        char *val = (char *) malloc(vlen + 1);
        cdb_read((struct cdb*) cdb, val, vlen, vpos);
        val[vlen] = 0;

        QString str(val);
        free(val);
        return str;
    } else
        return "";
}

/// Returns all values associated with the given key from the database.
/// \param key The key name in the database.
QStringList CDBReader::valuesForKey(const QString &key) const
{
    QStringList list;

    if (! cdb) {
        contextDebug() << F_CDB << "Trying to read values for key:" << key << "from database that is closed!";
        return list;
    }

    QByteArray utf8Data = key.toUtf8();
    unsigned int klen = utf8Data.size();
    const char *kval = utf8Data.constData();

    struct cdb_find cdbf;
    cdb_findinit(&cdbf, (struct cdb*) cdb, kval, klen);

    while(cdb_findnext(&cdbf) > 0) {
        unsigned int vpos = cdb_datapos((struct cdb*) cdb);
        unsigned int vlen = cdb_datalen((struct cdb*) cdb);
        char *val = (char *) malloc(vlen + 1);
        cdb_read((struct cdb*) cdb, val, vlen, vpos);
        val[vlen] = 0;

        QString str(val);
        free(val);
        list << str;
    }

    return list;
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


