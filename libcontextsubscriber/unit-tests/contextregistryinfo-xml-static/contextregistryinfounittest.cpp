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

class ContextRegistryInfoUnitTest : public QObject
{
    Q_OBJECT

private:
    ContextRegistryInfo *context;

private slots:
    void initTestCase();
    void checkProperBackend();
    void listKeys();
    void listKeysForProvider();
    void listProviders();
    void checkPrefix();
};

void ContextRegistryInfoUnitTest::initTestCase()
{
    qInstallMsgHandler(myMessageOutput);

    utilSetEnv("CONTEXT_PROVIDERS", LOCAL_DIR);
    utilSetEnv("CONTEXT_CORE_DECLARATIONS", "/dev/null");
    context = ContextRegistryInfo::instance();
    QVERIFY(context != NULL);
}

void ContextRegistryInfoUnitTest::checkProperBackend()
{
    QCOMPARE(context->backendName(), QString("xml"));
}

void ContextRegistryInfoUnitTest::listKeys()
{
    QStringList list = context->listKeys();
    QCOMPARE(list.size(), 3);

    QStringList expectedKeys;
    expectedKeys << "Battery.ChargePercentage";
    expectedKeys << "Battery.LowBattery";
    expectedKeys << "Battery.Charging";

    foreach (QString key, list) {
        if (expectedKeys.contains(key))
            expectedKeys.removeAll(key);
    }

    QCOMPARE(expectedKeys.size(), 0);
}

void ContextRegistryInfoUnitTest::listKeysForProvider()
{
    QStringList list = context->listKeys("org.freedesktop.ContextKit.contextd1");
    QCOMPARE(list.size(), 2);

    QStringList expectedKeys;
    expectedKeys << "Battery.ChargePercentage";
    expectedKeys << "Battery.LowBattery";

    foreach (QString key, list) {
        if (expectedKeys.contains(key))
            expectedKeys.removeAll(key);
    }

    QCOMPARE(expectedKeys.size(), 0);
    QCOMPARE(context->listKeys("Something.that.doesnt.exist").size(), 0);
}

void ContextRegistryInfoUnitTest::listProviders()
{
    QStringList list = context->listProviders();
    QCOMPARE(list.size(), 2);
    QVERIFY(list.contains("org.freedesktop.ContextKit.contextd1"));
    QVERIFY(list.contains("org.freedesktop.ContextKit.contextd2"));
}

void ContextRegistryInfoUnitTest::checkPrefix()
{
    QStringList list = context->listKeys();
    foreach (QString key, list) {
        QVERIFY(key.startsWith("Context", Qt::CaseInsensitive) == false);
    }
}

#include "contextregistryinfounittest.moc"
QTEST_MAIN(ContextRegistryInfoUnitTest);
