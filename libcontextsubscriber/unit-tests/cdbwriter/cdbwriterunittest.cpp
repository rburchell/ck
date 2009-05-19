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

#include <QtTest/QtTest>
#include <QtCore>
#include "cdbwriter.h"

class CDBWriterUnitTest : public QObject
{
    Q_OBJECT

private slots:
    void basicCreation();
    void noPermissions();
    void cleanupTestCase();
};

void CDBWriterUnitTest::basicCreation()
{
    CDBWriter writer("test.cdb");
    QCOMPARE(writer.isWritable(), true);

    writer.add("KEYS", "KEYSValue1");
    writer.add("KEYS", "KEYSValue2");
    writer.add("KEYS", "KEYSValue3");

    writer.add("KEY1", "KEY1Value");
    writer.close();
}

void CDBWriterUnitTest::noPermissions()
{
    CDBWriter writer("/usr/test.cdb");
    QCOMPARE(writer.isWritable(), false);
    writer.close();
}

void CDBWriterUnitTest::cleanupTestCase()
{
    QFile::remove("test.cdb");
}

#include "moc_cdbwriterunittest_cpp.cpp"
QTEST_MAIN(CDBWriterUnitTest);
