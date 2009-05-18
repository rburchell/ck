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
#include "cdbreaderunittest.h"

void CDBReaderUnitTest::basicCreation()
{
    CDBReader reader("test.cdb");
    QCOMPARE(reader.isReadable(), true);

    QCOMPARE(reader.valueForKey("KEY1"), QString("KEY1Value"));

    QStringList list = reader.valuesForKey("KEYS");
    QCOMPARE(list.size(), 3);

    QCOMPARE(list.at(0), QString("KEYSValue1"));
    QCOMPARE(list.at(1), QString("KEYSValue2"));
    QCOMPARE(list.at(2), QString("KEYSValue3"));

    reader.close();
}

void CDBReaderUnitTest::doesNotExist()
{
    CDBReader reader("/usr/test.cdb");
    QCOMPARE(reader.isReadable(), false);
    reader.close();
}

void CDBReaderUnitTest::cleanupTestCase()
{
    QFile::remove("test.cdb");
}


#include "moc_cdbreaderunittest_cpp.cpp"
