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

#include "cdbwriter.h"
#include <cdb.h>
#include <fcntl.h>

CDBWriter::CDBWriter(const QString &path, QObject *parent) 
    : QObject(parent)
{
    cdbm = NULL;
    fd = 0;

    fd = open(path.toUtf8().constData(), O_RDWR | O_CREAT, 0644);

    if (fd != 0) {
        cdbm = malloc(sizeof(cdb_make));
        cdb_make_start((cdb_make *) cdbm, fd);
    }
}

CDBWriter::~CDBWriter()
{
    close();
}

void CDBWriter::add(const QString &key, const QString &val)
{
    put(key, val, CDB_PUT_ADD);
}

void CDBWriter::insert(const QString &key, const QString &val)
{
    put(key, val, CDB_PUT_INSERT);
}

void CDBWriter::replace(const QString &key, const QString &val)
{
    put(key, val, CDB_PUT_REPLACE);
}

void CDBWriter::close()
{
    if (cdbm) {
        cdb_make_finish((cdb_make *) cdbm);
        free(cdbm);
        cdbm = NULL;
    }

    if (fd != 0) {
        ::close(fd);
        fd = 0;
    }
}

/* Private */

void CDBWriter::put(const QString &key, const QString &val, int flag)
{
    if (! cdbm)
        return;

    int klen = key.toUtf8().size();
    int vlen = val.toUtf8().size();
    cdb_make_put((cdb_make *) cdbm, 
                 key.toUtf8().constData(), klen, 
                 val.toUtf8().constData(), vlen, 
                 (cdb_put_mode) flag);
}

