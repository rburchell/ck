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

#include <QCoreApplication>
#include <QDir>
#include <QDebug>
#include <stdlib.h>
#include "contextregistryinfo.h"
#include "contextpropertyinfo.h"

void fail()
{
    QFile::remove("cache.cdb");
    exit(128);
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    QFile::remove("cache.cdb");
    if (! QFile::copy("cache1.cdb", "cache.cdb")) {
        qDebug() << "Can't copy cache1.cdb!";
        fail();
    }

    sleep(1);

    if (! ContextRegistryInfo::instance()->listKeys().contains("Battery.OnBattery")) {
        qDebug() << "Battery.OnBattery not found in cache1.cdb!";
        fail();
    }

    ContextPropertyInfo info1("Battery.OnBattery");
    if (info1.provided() != true) {
        qDebug() << "Battery.OnBattery should be provided!";
        fail();
    }

    QFile::remove("cache.cdb");
    if (! QFile::copy("cache2.cdb", "cache.cdb")) {
        qDebug() << "Can't copy cache2.cdb!";
        fail();
    }

    for (int i = 0; i < 1000; i++)
        app.processEvents();

    ContextPropertyInfo info2("Battery.OnBattery");
    if (info2.provided() == true) {
        qDebug() << "Battery.OnBattery should not be provided!";
        fail();
    }

    QFile::remove("cache.cdb");
    return 0;
}

