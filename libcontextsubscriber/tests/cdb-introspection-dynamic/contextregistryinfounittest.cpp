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
#include "contextregistryinfo.h"
#include "contextregistryinfounittest.h"
#include <fcntl.h>

void ContextRegistryInfoUnitTest::initTestCase()
{
    QFile::copy("context-providers1v1.cdb", "context-providers.cdb");
    context = ContextRegistryInfo::instance("cdb");
    QVERIFY(context != NULL);
}

void ContextRegistryInfoUnitTest::basicChange()
{
    // Setup state
    QFile::copy("context-providers1v1.cdb", "temp.cdb");
    rename("temp.cdb", "context-providers.cdb");
    QTest::qWait(200);

    QSignalSpy spy(context, SIGNAL(keysChanged(QStringList)));

    QFile::copy("context-providers1v2.cdb", "temp.cdb");
    rename("temp.cdb", "context-providers.cdb");
    QTest::qWait(500);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();
    QCOMPARE(args.at(0).toList().size(), 3);

    // Now just make sure that the new key is availible
    QStringList newKeys = context->listKeys();
    QVERIFY(newKeys.contains("Battery.AboutToExplode"));
}

void ContextRegistryInfoUnitTest::cleanupTestCase()
{
    QFile::remove("context-providers.cdb");
}
    
#include "moc_contextregistryinfounittest_cpp.cpp"
