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
#include <fcntl.h>
#include "cdbwriter.h"
#include "fileutils.h"

class CDBWriterUnitTest : public QObject
{
    Q_OBJECT

private slots:
    void basicCreation();
    void noPermissions();
    void cleanupTestCase();
    void writingToBad();
    void createWithFileDescriptor();
    void createWithBadFileDescriptor();
};

void CDBWriterUnitTest::basicCreation()
{
    CDBWriter writer(LOCAL_FILE("test.cdb"));
    QCOMPARE(writer.isWritable(), true);
    QVERIFY(writer.fileDescriptor() > 0);

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

void CDBWriterUnitTest::writingToBad()
{
    CDBWriter writer("/usr/test.cdb");
    QCOMPARE(writer.isWritable(), false);
    QVERIFY(writer.fileDescriptor() <= 0);
    
    writer.add("KEYS", "VAL");
    writer.close();
}

void CDBWriterUnitTest::createWithFileDescriptor()
{
    int fd = open("test-fdo.cdb", O_RDWR | O_CREAT, 0644);
    CDBWriter writer(fd);
    
    QCOMPARE(writer.isWritable(), true);
}

void CDBWriterUnitTest::createWithBadFileDescriptor()
{
    int fd = open("/usr/test/something/database.cdb", O_RDWR | O_CREAT, 0644);
    CDBWriter writer(fd);
    
    QCOMPARE(writer.isWritable(), false);
}

void CDBWriterUnitTest::cleanupTestCase()
{
    QFile::remove(LOCAL_FILE("test.cdb"));
    QFile::remove(LOCAL_FILE("test-fdo.cdb"));
}


#include "moc_cdbwriterunittest_cpp.cpp"
QTEST_MAIN(CDBWriterUnitTest);
