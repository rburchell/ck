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

#include <cdb.h>
#include <fcntl.h>
#include <stdlib.h>
#include "cdbwriter.h"

/// Constructs a new CDBWriter to write to a cdb database at \a dbpath
/// \param dbpath Path to the database.
CDBWriter::CDBWriter(const QString &path, QObject *parent) 
    : QObject(parent)
{
    cdbm = NULL;
    fd = 0;

    fd = open(path.toUtf8().constData(), O_RDWR | O_CREAT, 0644);

    if (fd > 0) {
        cdbm = calloc(1, sizeof(struct cdb_make));
        cdb_make_start((struct cdb_make *) cdbm, fd);
    }
}

/// Constructs a new CDBWriter to write to a cdb database at a file
/// descriptor \a fd. You should not manipulate the \a fd after calling
/// this function.
/// \param fd An open file descriptor. 
CDBWriter::CDBWriter(int fdd, QObject *parent) 
    : QObject(parent)
{
    cdbm = NULL;
    fd = fdd;

    if (fd > 0) {
        cdbm = calloc(1, sizeof(struct cdb_make));
        cdb_make_start((struct cdb_make *) cdbm, fd);
    }
}

/// Destroys the object closing the file beforehand.
CDBWriter::~CDBWriter()
{
    close();
}

/// Add a new \a key with \a val to the database. If a value
/// for this key already exists, another one is added.
/// \param key Key name as string.
/// \param val Value as string.
void CDBWriter::add(const QString &key, const QString &val)
{
    put(key, val, CDB_PUT_ADD);
}

/// Insert a new \a key with \a val to the database. If a value
/// for this key already exists, nothing is done.
/// \param key Key name as string.
/// \param val Value as string.
void CDBWriter::insert(const QString &key, const QString &val)
{
    put(key, val, CDB_PUT_INSERT);
}

/// Insert a new \a key with \a val to the database. If a value
/// for this key already exists, the old value is replaced with the
/// new one.
/// \param key Key name as string.
/// \param val Value as string.
void CDBWriter::replace(const QString &key, const QString &val)
{
    put(key, val, CDB_PUT_REPLACE);
}

/// Closes the writer and the underlying filesystem resource for writing.
void CDBWriter::close()
{
    if (cdbm) {
        cdb_make_finish((struct cdb_make *) cdbm);
        free(cdbm);
        cdbm = NULL;
    }

    if (fd > 0) {
        ::close(fd);
        fd = 0;
    }
}

/// Returns the file descriptor used by the writer. Returns 0
/// when the writer is closed.
int CDBWriter::fileDescriptor() const
{
    return fd;
}

/// Returns \a true if the writer is writable. The writer is not writable
/// after it has been closed or the target path is not accessible.
bool CDBWriter::isWritable()
{
    return (fd > 0);
}

/* Private */

void CDBWriter::put(const QString &key, const QString &val, int flag)
{
    if (! cdbm)
        return;

    QByteArray keyUtf8Data = key.toUtf8();
    QByteArray valUtf8Data = val.toUtf8();

    int klen = keyUtf8Data.size();
    const char *kval = keyUtf8Data.constData();
    int vlen = valUtf8Data.size();
    const char *vval = valUtf8Data.constData();

    cdb_make_put((struct cdb_make *) cdbm, 
                 kval, klen, 
                 vval, vlen, 
                 (cdb_put_mode) flag);
}

