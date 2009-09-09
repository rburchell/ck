/*
 * Copyright (C) 2009 Nokia Corporation.
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
#include "testbluezplugin.h"

// Plugin interface definition
#include "iproviderplugin.h"

#include <QtTest/QtTest>
#include <QLibrary>

typedef ContextSubscriber::IProviderPlugin* (*PluginFactoryFunc)(const QString& constructionString);

//
// Definition of testcases
//

// Before all tests
void BluezPluginTests::initTestCase()
{
}

// After all tests
void BluezPluginTests::cleanupTestCase()
{
}

// Before each test
void BluezPluginTests::init()
{
    library = new QLibrary("libcontextsubscriberbluez");
}

// After each test
void BluezPluginTests::cleanup()
{
    delete library;
    library = 0;
}

// Test cases

void BluezPluginTests::loading()
{
    /*library->load();
    qDebug() << library->errorString();
    QVERIFY(library->isLoaded());
    PluginFactoryFunc f = (PluginFactoryFunc) library->resolve("bluezPluginFactory");
    QVERIFY(f);
    */
    // FIXME: implement the test when plugin loading / building is more clear
}

QTEST_MAIN(BluezPluginTests);
