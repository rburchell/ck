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
#include "fcntl.h"

class ContextRegistryInfoUnitTest : public QObject
{
    Q_OBJECT

private:
    ContextRegistryInfo *context;

private slots:
    void initTestCase();
    void basicChange();
    void changeWithRemove();
    void cleanupTestCase();
};

void ContextRegistryInfoUnitTest::initTestCase()
{
    setenv("CONTEXT_PROVIDERS", "./", 0);

    // Setup state
    // QFile::remove("providers.xml");
    QFile::copy("providers1v1.xml.src", "providers.xml");
    QTest::qWait(200);
       
    context = ContextRegistryInfo::instance("xml");
}

void ContextRegistryInfoUnitTest::basicChange()
{
    QSignalSpy spy1(context, SIGNAL(keysChanged(QStringList)));
    QSignalSpy spy2(context, SIGNAL(keysAdded(QStringList)));

    QFile::copy("providers1v2.xml.src", "tmp.file");
    rename("tmp.file", "providers.xml");
    QTest::qWait(500);

    QCOMPARE(spy1.count(), 1);
    QList<QVariant> args1 = spy1.takeFirst();
    QCOMPARE(args1.at(0).toList().size(), 3);

    QCOMPARE(spy2.count(), 1);
    QList<QVariant> args2 = spy2.takeFirst();
    QCOMPARE(args2.at(0).toList().size(), 1);

    // Now just make sure that the new key is availible
    QStringList newKeys = context->listKeys();
    QVERIFY(newKeys.contains("Battery.AboutToExplode"));
}

void ContextRegistryInfoUnitTest::changeWithRemove()
{
    QSignalSpy spy1(context, SIGNAL(keysChanged(QStringList)));

    QFile::remove("providers.xml");
    QFile::copy("providers1v1.xml.src", "providers.xml");
    QTest::qWait(500);

    QCOMPARE(spy1.count(), 2);
}

void ContextRegistryInfoUnitTest::cleanupTestCase()
{
    QFile::remove("providers.xml");
}
    
#include "moc_contextregistryinfounittest_cpp.cpp"
QTEST_MAIN(ContextRegistryInfoUnitTest);