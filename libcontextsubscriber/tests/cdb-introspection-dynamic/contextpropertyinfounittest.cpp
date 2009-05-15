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
#include "contextpropertyinfo.h"
#include "contextpropertyinfounittest.h"
#include <fcntl.h>

void ContextPropertyInfoUnitTest::checkKeyTypeChanging()
{
    // Create initial state
    QFile::copy("context-providers2v1.cdb", "temp.cdb");
    rename("temp.cdb", "context-providers.cdb");
    QTest::qWait(200);

    ContextPropertyInfo prop("Battery.LowBattery");
    QCOMPARE(prop.type(), QString("TRUTH"));
    
    QSignalSpy spy(&prop, SIGNAL(typeChanged(QString)));

    QFile::copy("context-providers2v2.cdb", "temp.cdb");
    rename("temp.cdb", "context-providers.cdb");
    QTest::qWait(500);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();
    QCOMPARE(args.at(0).toString(), QString("INT"));

    // Now just make sure that the new val is ok too
    QCOMPARE(prop.type(), QString("INT"));
}

void ContextPropertyInfoUnitTest::checkKeyRemoval()
{
    // Create initial state
    QFile::copy("context-providers3v1.cdb", "temp.cdb");
    rename("temp.cdb", "context-providers.cdb");
    QTest::qWait(200);

    ContextPropertyInfo prop("Battery.LowBattery");
    QVERIFY(prop.type() != "");
    QVERIFY(prop.doc() != "");
    
    QFile::copy("context-providers3v2.cdb", "temp.cdb");
    rename("temp.cdb", "context-providers.cdb");
    QTest::qWait(500);

    QVERIFY(prop.type() == "");
    QVERIFY(prop.doc() == "");
}

void ContextPropertyInfoUnitTest::cleanupTestCase()
{
    QFile::remove("context-providers.cdb");
}

#include "moc_contextpropertyinfounittest_cpp.cpp"
