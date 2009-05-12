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

#include "cdbreader.h"
#include <cdb.h>
#include <fcntl.h>

CDBReader::CDBReader(const QString &path, QObject *parent) 
    : QObject(parent)
{
    cdb = NULL;
    fd = 0;

    fd = open(path.toUtf8().constData(), O_RDONLY);

    if (fd != 0) {
        cdb = malloc(sizeof(cdb));
        cdb_init((struct cdb*) cdb, fd);
    }
}

CDBReader::~CDBReader()
{
    close();
}

void CDBReader::close()
{
    if (cdb) {
        free(cdb);
        cdb = NULL;
    }

    if (fd != 0) {
        ::close(fd);
        fd = 0;
    }
}

QString CDBReader::valueForKey(const QString &key)
{
    if (! cdb)
        return "";

    unsigned int klen = key.toUtf8().size();
    if (cdb_find((struct cdb*) cdb, key.toUtf8().constData(), klen)) {
        unsigned int vpos = cdb_datapos((struct cdb*) cdb);
        unsigned int vlen = cdb_datalen((struct cdb*) cdb);
        char *val = (char *) malloc(vlen);
        cdb_read((struct cdb*) cdb, val, vlen, vpos);
        
        QString str(val);
        free(val);
        return str;
    } else
        return "";
}

QStringList CDBReader::valuesForKey(const QString &key)
{
    QStringList list;

    if (! cdb)
       return list;

    unsigned int klen = key.toUtf8().size();
    struct cdb_find cdbf;
    cdb_findinit(&cdbf, (struct cdb*) cdb, key.toUtf8().constData(), klen);

    while(cdb_findnext(&cdbf) > 0) {
        unsigned int vpos = cdb_datapos((struct cdb*) cdb);
        unsigned int vlen = cdb_datalen((struct cdb*) cdb);
        char *val = (char *) malloc(vlen);
        cdb_read((struct cdb*) cdb, val, vlen, vpos);

        QString str(val);
        free(val);
        list << str;
    }

    return list;
}

