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

void ContextRegistryInfoUnitTest::initTestCase()
{
    context = ContextRegistryInfo::instance();
    QVERIFY(context != NULL);
}

void ContextRegistryInfoUnitTest::basicChange()
{
    // Setup state
    QFile::remove("xml/providers.xml");
    QFile::copy("xml/providers1v1.xml.src", "xml/providers.xml");

    QSignalSpy spy(context, SIGNAL(keysChanged(QStringList)));

    QFile::remove("xml/providers.xml");
    QFile::copy("xml/providers1v2.xml.src", "xml/providers.xml");
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
    QFile::remove("xml/providers.xml");
}
    
#include "moc_contextregistryinfounittest_cpp.cpp"
