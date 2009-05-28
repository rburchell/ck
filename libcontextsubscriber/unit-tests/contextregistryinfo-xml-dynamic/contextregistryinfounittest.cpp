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
#include "fileutils.h"

class ContextRegistryInfoUnitTest : public QObject
{
    Q_OBJECT

private:
    ContextRegistryInfo *context;

private slots:
    void initTestCase();
    void basicChange();
    void changeWithRemove();
    void keyRemoval();
    void cleanupTestCase();
};

void ContextRegistryInfoUnitTest::initTestCase()
{
    utilSetEnv("CONTEXT_PROVIDERS", LOCAL_DIR);

    // Setup state
    utilCopyLocalWithRemove("providers1v1.xml.src", "providers.context");
           
    context = ContextRegistryInfo::instance("xml");
}

void ContextRegistryInfoUnitTest::basicChange()
{
    QSignalSpy spy1(context, SIGNAL(keysChanged(QStringList)));
    QSignalSpy spy2(context, SIGNAL(keysAdded(QStringList)));

    utilCopyLocalAtomically("providers1v2.xml.src", "providers.context");

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
    
    utilCopyLocalWithRemove("providers1v1.xml.src", "providers.context");

    // Sometimes it's 1, sometimes it's 2 -- depending on how agile the watcher is.
    QVERIFY(spy1.count() > 0);
}

void ContextRegistryInfoUnitTest::keyRemoval()
{
    utilCopyLocalWithRemove("providers1v2.xml.src", "providers.context");
    
    QSignalSpy spy1(context, SIGNAL(keysChanged(QStringList)));
    QSignalSpy spy2(context, SIGNAL(keysRemoved(QStringList)));
    
    utilCopyLocalAtomically("providers1v1.xml.src", "providers.context");

    QCOMPARE(spy1.count(), 1);
    QList<QVariant> args1 = spy1.takeFirst();
    QCOMPARE(args1.at(0).toList().size(), 2);

    QCOMPARE(spy2.count(), 1);
    QList<QVariant> args2 = spy2.takeFirst();
    QCOMPARE(args2.at(0).toList().size(), 1);

    // Now just make sure that the new key is not there
    QStringList newKeys = context->listKeys();
    QVERIFY(! newKeys.contains("Battery.AboutToExplode"));
}

void ContextRegistryInfoUnitTest::cleanupTestCase()
{
    QFile::remove(LOCAL_FILE("providers.context"));
}
    
#include "moc_contextregistryinfounittest_cpp.cpp"
QTEST_MAIN(ContextRegistryInfoUnitTest);
