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
#include "cdbreader.h"
#include "fileutils.h"

class CDBUnitTest : public QObject
{
    Q_OBJECT

private slots:
    void creation();
    void cleanupTestCase();
    void writingToBad();
    void createWithFileDescriptor();
    void createWithBadFileDescriptor();
    void reading();
    void readingFromBad();
};

void CDBUnitTest::creation()
{
    CDBWriter writer("test.cdb");
    QCOMPARE(writer.isWritable(), true);
    QVERIFY(writer.fileDescriptor() > 0);

    writer.add("KEYS", "KEYSValue1");
    writer.add("KEYS", "KEYSValue2");
    writer.add("KEYS", "KEYSValue3");
    writer.insert("KEYS", "doesn't get into");

    writer.add("KEY1", "KEY1Value wrong");
    writer.replace("KEY1", "KEY1Value");

    QVariantList complex;
    QMap<QString, QVariant> map;
    map["fortytwo"] = 42;
    map["fortytwo two times"] = 4242;
    QList<QVariant> list;
    list << 24.24;
    list << 42.42;
    complex << QVariant("test") << QVariant(map) << QVariant(list);

    writer.add("COMPLEX", complex);
    writer.close();
}

void CDBUnitTest::writingToBad()
{
    CDBWriter writer("/proc/test.cdb");
    QCOMPARE(writer.isWritable(), false);
    QVERIFY(writer.fileDescriptor() <= 0);

    writer.add("KEYS", "VAL");
    writer.close();
}

void CDBUnitTest::createWithFileDescriptor()
{
    int fd = open("test-fdo.cdb", O_RDWR | O_CREAT, 0644);
    CDBWriter writer(fd);

    QCOMPARE(writer.isWritable(), true);
}

void CDBUnitTest::createWithBadFileDescriptor()
{
    int fd = open("/usr/test/something/database.cdb", O_RDWR | O_CREAT, 0644);
    CDBWriter writer(fd);

    QCOMPARE(writer.isWritable(), false);
}

void CDBUnitTest::readingFromBad()
{
    CDBReader reader("/proc/test.cdb");
    QCOMPARE(reader.isReadable(), false);
    QVERIFY(reader.fileDescriptor() <= 0);
    QVariant v = reader.valueForKey("SOMETHING");
    QCOMPARE(v, QVariant());
}

void CDBUnitTest::reading()
{
    CDBReader reader("test.cdb");
    QCOMPARE(reader.isReadable(), true);
    QVERIFY(reader.fileDescriptor() > 0);

    QCOMPARE(reader.valueForKey("KEY1"), QVariant("KEY1Value"));

    QVariantList reslist = reader.valuesForKey("KEYS");
    QCOMPARE(reslist.size(), 3);

    QCOMPARE(reslist.at(0), QVariant("KEYSValue1"));
    QCOMPARE(reslist.at(1), QVariant("KEYSValue2"));
    QCOMPARE(reslist.at(2), QVariant("KEYSValue3"));

    QCOMPARE(QVariantList() << "KEY1Value", reader.valuesForKey("KEY1"));

    QVariantList complex;
    QMap<QString, QVariant> map;
    map["fortytwo"] = 42;
    map["fortytwo two times"] = 4242;
    QList<QVariant> list;
    list << 24.24;
    list << 42.42;
    complex << QVariant("test") << QVariant(map) << QVariant(list);

    QCOMPARE(QVariant(complex), reader.valuesForKey("COMPLEX")[0]);
    QCOMPARE(42, reader.valueForKey("COMPLEX").toList().at(1).toMap().value("fortytwo").toInt());
}

void CDBUnitTest::cleanupTestCase()
{
    QFile::remove("test.cdb");
    QFile::remove("test-fdo.cdb");
}

#include "cdbunittest.moc"
QTEST_MAIN(CDBUnitTest);
