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
#include <QDebug>
#include <QFile>

CDBReader::CDBReader(const QString &path, QObject *parent) 
    : QObject(parent)
{
    cdb = NULL;
    fd = 0;

    if (! QFile::exists(path))
        return;

    fd = open(path.toUtf8().constData(), O_RDONLY);

    if (fd != 0) {
        cdb = calloc(1, sizeof(struct cdb));
        if (cdb_init((struct cdb*) cdb, fd) != 0) {
            free(cdb);
            cdb = NULL;
        }
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

QString CDBReader::valueForKey(const QString &key) const
{
    if (! cdb)
        return "";

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

QStringList CDBReader::valuesForKey(const QString &key) const
{
    QStringList list;

    if (! cdb)
       return list;

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
