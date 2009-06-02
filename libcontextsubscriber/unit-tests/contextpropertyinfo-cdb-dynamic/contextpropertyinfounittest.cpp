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
#include "contextregistryinfo.h"
#include <fcntl.h>
#include "fileutils.h"

#define MYLOGLEVEL 2
void myMessageOutput(QtMsgType type, const char *msg)
{
    switch (type) {
    case QtDebugMsg:
        if (MYLOGLEVEL <= 0)
            fprintf(stderr, "Debug: %s\n", msg);
        break;
    case QtWarningMsg:
        if (MYLOGLEVEL <= 1)
            fprintf(stderr, "Warning: %s\n", msg);
        break;
    case QtCriticalMsg:
        if (MYLOGLEVEL <= 2)
            fprintf(stderr, "Critical: %s\n", msg);
        break;
    case QtFatalMsg:
        if (MYLOGLEVEL <= 3)
            fprintf(stderr, "Fatal: %s\n", msg);
        abort();
    }
}

class ContextPropertyInfoUnitTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void checkKeyTypeChanging();
    void checkKeyProviderChanging();
    void checkKeyRemoval();
    void checkKeyAdding();
    void cleanupTestCase();
};

void ContextPropertyInfoUnitTest::initTestCase()
{
    qInstallMsgHandler(myMessageOutput);

    utilSetEnv("CONTEXT_PROVIDERS", "./");
    utilCopyLocalAtomically("context-providers2v1.cdb", "cache.cdb");

    ContextRegistryInfo::instance("cdb");
}

void ContextPropertyInfoUnitTest::checkKeyTypeChanging()
{
    ContextPropertyInfo prop("Battery.LowBattery");
    QCOMPARE(prop.type(), QString("TRUTH"));

    QSignalSpy spy(&prop, SIGNAL(typeChanged(QString)));

    utilCopyLocalAtomically("context-providers2v2.cdb", "cache.cdb");

    QCOMPARE(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();
    QCOMPARE(args.at(0).toString(), QString("INT"));

    // Now just make sure that the new values are ok
    QCOMPARE(prop.type(), QString("INT"));
    QCOMPARE(prop.providerDBusType(), QDBusConnection::SessionBus);
    QCOMPARE(prop.providerDBusName(), QString("org.freedesktop.ContextKit.contextd1"));
}

void ContextPropertyInfoUnitTest::checkKeyProviderChanging()
{
    // Create initial state
    utilCopyLocalAtomically("context-providers4v1.cdb", "cache.cdb");

    ContextPropertyInfo prop("Battery.LowBattery");
    QCOMPARE(prop.providerDBusName(), QString("org.freedesktop.ContextKit.contextd1"));
    QSignalSpy spy(&prop, SIGNAL(providerChanged(QString)));

    utilCopyLocalAtomically("context-providers4v2.cdb", "cache.cdb");

    QCOMPARE(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();
    QCOMPARE(args.at(0).toString(), QString("org.freedesktop.ContextKit.contextd2"));
    QCOMPARE(prop.providerDBusName(), QString("org.freedesktop.ContextKit.contextd2"));
}

void ContextPropertyInfoUnitTest::checkKeyRemoval()
{
    // Create initial state
    utilCopyLocalAtomically("context-providers3v1.cdb", "cache.cdb");

    ContextPropertyInfo prop("Battery.LowBattery");
    QVERIFY(prop.type() != "");
    QVERIFY(prop.doc() != "");
    QVERIFY(prop.exists() == true);

    utilCopyLocalAtomically("context-providers3v2.cdb", "cache.cdb");

    QVERIFY(prop.type() == "");
    QVERIFY(prop.doc() == "");
    QVERIFY(prop.exists() == false);
}

void ContextPropertyInfoUnitTest::checkKeyAdding()
{
    // Create initial state
    utilCopyLocalAtomically("context-providers3v2.cdb", "cache.cdb");

    ContextPropertyInfo prop("Battery.LowBattery");
    QVERIFY(prop.type() == "");
    QVERIFY(prop.doc() == "");
    QVERIFY(prop.exists() == false);

    utilCopyLocalAtomically("context-providers3v1.cdb", "cache.cdb");

    QVERIFY(prop.exists() == true);
    QVERIFY(prop.type() != "");
    QVERIFY(prop.doc() != "");
}

void ContextPropertyInfoUnitTest::cleanupTestCase()
{
    QFile::remove("cache.cdb");
}

#include "contextpropertyinfounittest.moc"
QTEST_MAIN(ContextPropertyInfoUnitTest);
