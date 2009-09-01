/*
 * Copyright (C) 2008 Nokia Corporation.
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

// Header file of the tests
#include "testinfoxmlkeysfinder.h"

// Header file of the class to be tested
#include "infoxmlkeysfinder.h"

// Utils
#include "fileutils.h"

#include <QtTest/QtTest>
#include <QDebug>

namespace ContextSubscriber {

//
// Definition of testcases
//


// Before all tests
void InfoXmlKeysFinderUnitTests::initTestCase()
{
}

// After all tests
void InfoXmlKeysFinderUnitTests::cleanupTestCase()
{
}

// Before each test
void InfoXmlKeysFinderUnitTests::init()
{
}

// After each test
void InfoXmlKeysFinderUnitTests::cleanup()
{
}

// Test cases

void InfoXmlKeysFinderUnitTests::dbusStyleXml()
{
    InfoXmlKeysFinder handler;

    QFile f(LOCAL_FILE("provider-dbus.context"));
    QXmlInputSource source (&f);
    QXmlSimpleReader reader;

    reader.setContentHandler(&handler);
    reader.parse(source);

    QHash<QString, InfoKeyData> keys = handler.keyDataHash;
    QCOMPARE(keys.size(), 2);

    InfoKeyData one = keys.value("Test.One");
    QCOMPARE(one.name, QString("Test.One"));
    QCOMPARE(one.type, QString("INT"));
    QCOMPARE(one.doc, QString("An integer."));
    QCOMPARE(one.plugin, QString("contextkit-dbus"));
    QCOMPARE(one.constructionString, QString("system:org.freedesktop.test"));

    InfoKeyData two = keys.value("Test.Two");
    QCOMPARE(two.name, QString("Test.Two"));
    QCOMPARE(two.type, QString("STRING"));
    QCOMPARE(two.doc, QString("A string."));
    QCOMPARE(two.plugin, QString("contextkit-dbus"));
    QCOMPARE(two.constructionString, QString("system:org.freedesktop.test"));
}

void InfoXmlKeysFinderUnitTests::pluginStyleXml()
{
    InfoXmlKeysFinder handler;

    QFile f(LOCAL_FILE("provider-plugin.context"));
    QXmlInputSource source (&f);
    QXmlSimpleReader reader;

    reader.setContentHandler(&handler);
    reader.parse(source);

    QHash<QString, InfoKeyData> keys = handler.keyDataHash;
    QCOMPARE(keys.size(), 2);

    InfoKeyData one = keys.value("Test.Something");
    QCOMPARE(one.name, QString("Test.Something"));
    QCOMPARE(one.type, QString("TRUTH"));
    QCOMPARE(one.doc, QString("Something"));
    QCOMPARE(one.plugin, QString("testplugin.so"));
    QCOMPARE(one.constructionString, QString("factoryParameter"));

    InfoKeyData two = keys.value("Test.SomethingElse");
    QCOMPARE(two.name, QString("Test.SomethingElse"));
    QCOMPARE(two.type, QString("DOUBLE"));
    QCOMPARE(two.doc, QString("Something else"));
    QCOMPARE(two.plugin, QString("testplugin.so"));
    QCOMPARE(two.constructionString, QString("factoryParameter"));
}

void InfoXmlKeysFinderUnitTests::allTypes()
{
    InfoXmlKeysFinder handler;

    QFile f(LOCAL_FILE("provider-alltypes.context"));
    QXmlInputSource source (&f);
    QXmlSimpleReader reader;

    reader.setContentHandler(&handler);
    reader.parse(source);

    QHash<QString, InfoKeyData> keys = handler.keyDataHash;
    QCOMPARE(keys.size(), 8);

    // Note: In the expected results, the type names are canonized.

    InfoKeyData int1 = keys.value("Test.Int1");
    QCOMPARE(int1.name, QString("Test.Int1"));
    QCOMPARE(int1.type, QString("INT"));

    InfoKeyData int2 = keys.value("Test.Int2");
    QCOMPARE(int2.name, QString("Test.Int2"));
    QCOMPARE(int2.type, QString("INT"));

    InfoKeyData string1 = keys.value("Test.String1");
    QCOMPARE(string1.name, QString("Test.String1"));
    QCOMPARE(string1.type, QString("STRING"));

    InfoKeyData string2 = keys.value("Test.String2");
    QCOMPARE(string2.name, QString("Test.String2"));
    QCOMPARE(string2.type, QString("STRING"));

    InfoKeyData bool1 = keys.value("Test.Bool1");
    QCOMPARE(bool1.name, QString("Test.Bool1"));
    QCOMPARE(bool1.type, QString("TRUTH"));

    InfoKeyData bool2 = keys.value("Test.Bool2");
    QCOMPARE(bool2.name, QString("Test.Bool2"));
    QCOMPARE(bool2.type, QString("TRUTH"));

    InfoKeyData double1 = keys.value("Test.Double1");
    QCOMPARE(double1.name, QString("Test.Double1"));
    QCOMPARE(double1.type, QString("DOUBLE"));

    InfoKeyData double2 = keys.value("Test.Double2");
    QCOMPARE(double2.name, QString("Test.Double2"));
    QCOMPARE(double2.type, QString("DOUBLE"));
}


void InfoXmlKeysFinderUnitTests::complexTypes()
{
    InfoXmlKeysFinder handler;

    QFile f(LOCAL_FILE("provider-complextypes.context"));
    QXmlInputSource source (&f);
    QXmlSimpleReader reader;

    reader.setContentHandler(&handler);
    reader.parse(source);

    QHash<QString, InfoKeyData> keys = handler.keyDataHash;
    QCOMPARE(keys.size(), 1);

    // Note: for now, complex types are not handled. The type of a key
    // with complex type is an empty string. This is a placeholder for
    // future tests.

    InfoKeyData complex = keys.value("Test.Complex");
    QCOMPARE(complex.name, QString("Test.Complex"));
    QCOMPARE(complex.type, QString(""));
}

} // end namespace

QTEST_MAIN(ContextSubscriber::InfoXmlKeysFinderUnitTests);
