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

class ContextPropertyInfoUnitTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void checkLowBattery();
    void checkSystemBus();
    void checkNonExistant();
};

void ContextPropertyInfoUnitTest::initTestCase()
{
    if (getenv("srcdir"))
        setenv("CONTEXT_PROVIDERS", (QString(getenv("srcdir")) + "/").toUtf8().constData(), 0);
    else
        setenv("CONTEXT_PROVIDERS", "./", 0);

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

#include "moc_contextpropertyinfounittest_cpp.cpp"
QTEST_MAIN(ContextPropertyInfoUnitTest);
