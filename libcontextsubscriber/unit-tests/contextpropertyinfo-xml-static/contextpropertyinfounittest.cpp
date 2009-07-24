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
    void checkLowBattery();
    void checkSystemBus();
    void checkNonExistant();
    void checkAttribute();
    void checkNewTypeBool();
    void checkNewTypeInt32();
    void checkNewTypeString();
    void checkNewTypeDouble();
    void checkNewTypeComplex();
};

void ContextPropertyInfoUnitTest::initTestCase()
{
    qInstallMsgHandler(myMessageOutput);

    utilSetEnv("CONTEXT_PROVIDERS", LOCAL_DIR);
    utilSetEnv("CONTEXT_CORE_DECLARATIONS", "/dev/null");
    ContextRegistryInfo::instance();
}

void ContextPropertyInfoUnitTest::checkLowBattery()
{
    ContextPropertyInfo prop("Battery.LowBattery");

    QCOMPARE(prop.key(), QString("Battery.LowBattery"));
    QCOMPARE(prop.doc(), QString("This is true when battery is low"));
    QCOMPARE(prop.type(), QString("TRUTH"));
    QCOMPARE(prop.providerDBusName(), QString("org.freedesktop.ContextKit.contextd1"));
    QCOMPARE(prop.providerDBusType(), QDBusConnection::SessionBus);
}

void ContextPropertyInfoUnitTest::checkSystemBus()
{
    ContextPropertyInfo prop("Battery.Charging");
    QCOMPARE(prop.providerDBusType(), QDBusConnection::SystemBus);
}

void ContextPropertyInfoUnitTest::checkNonExistant()
{
    ContextPropertyInfo prop("This.doesnt.exist");
    QCOMPARE(prop.doc(), QString(""));
    QCOMPARE(prop.type(), QString(""));
}

void ContextPropertyInfoUnitTest::checkAttribute()
{
    ContextPropertyInfo prop("Key.With.Attribute");
    QCOMPARE(prop.type(), QString("TRUTH"));
}

void ContextPropertyInfoUnitTest::checkNewTypeBool()
{
    ContextPropertyInfo prop("Key.With.bool");
    QCOMPARE(prop.type(), QString("TRUTH"));
}

void ContextPropertyInfoUnitTest::checkNewTypeInt32()
{
    ContextPropertyInfo prop("Key.With.int32");
    QCOMPARE(prop.type(), QString("INT"));
}

void ContextPropertyInfoUnitTest::checkNewTypeString()
{
    ContextPropertyInfo prop("Key.With.string");
    QCOMPARE(prop.type(), QString("STRING"));
}

void ContextPropertyInfoUnitTest::checkNewTypeDouble()
{
    ContextPropertyInfo prop("Key.With.double");
    QCOMPARE(prop.type(), QString("DOUBLE"));
}

void ContextPropertyInfoUnitTest::checkNewTypeComplex()
{
    ContextPropertyInfo prop("Key.With.complex");
    QCOMPARE(prop.type(), QString(""));
}

#include "contextpropertyinfounittest.moc"
QTEST_MAIN(ContextPropertyInfoUnitTest);
