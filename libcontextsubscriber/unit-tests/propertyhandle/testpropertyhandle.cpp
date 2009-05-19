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
#include "testpropertyhandle.h"

// Mock header files
#include "propertyprovider.h"
#include "dbusnamelistener.h"
#include "contextpropertyinfo.h"
#include "contextregistryinfo.h"

// Header file of the class to be tested
#include "propertyhandle.h"

#include <QtTest/QtTest>
#include <QDebug>
#include <QDBusConnection>

#define MYLOGLEVEL 0
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

// Mock instances
// These will be created by the test program
PropertyProvider* mockPropertyProvider;
DBusNameListener* mockDBusNameListener;
ContextRegistryInfo* mockContextRegistryInfo;
// These will be created by the tested class and stored here
ContextPropertyInfo* mockContextPropertyInfo;

// Mock implementation of the PropertyProvider

PropertyProvider* PropertyProvider::instance(const QDBusConnection::BusType busType, const QString &busName)
{
    // qDebug() << "Returning a mock dbus name listener";
    return mockPropertyProvider;
}

PropertyProvider::PropertyProvider()
{
}

void PropertyProvider::subscribe(const QString& key)
{
}

void PropertyProvider::unsubscribe(const QString& key)
{
}


// Mock implementation of the DBusNameListener

DBusNameListener* DBusNameListener::instance(const QDBusConnection::BusType busType, const QString &busName)
{
    // qDebug() << "Returning a mock dbus name listener";
    return mockDBusNameListener;
}

DBusNameListener::DBusNameListener()
    : servicePresent(true)
{
}


bool DBusNameListener::isServicePresent() const
{
    return servicePresent;
}

// Mock implementation of ContextPropertyInfo

ContextPropertyInfo::ContextPropertyInfo(const QString &key, QObject *parent)
{
}

QString ContextPropertyInfo::type() const
{
    return "FakeType";
}

QString ContextPropertyInfo::providerDBusName() const
{
    return "Fake.Provider";
}

QDBusConnection::BusType ContextPropertyInfo::providerDBusType() const
{
    return QDBusConnection::SessionBus;
}

// Mock implementation of the ContextRegistryInfo

ContextRegistryInfo* ContextRegistryInfo::instance(const QString& backendName)
{
    return mockContextRegistryInfo;
}

//
// Definition of testcases
//


// Before all tests
void PropertyHandleUnitTests::initTestCase()
{
    qInstallMsgHandler(myMessageOutput);
}

// After all tests
void PropertyHandleUnitTests::cleanupTestCase()
{
}

// Before each test
void PropertyHandleUnitTests::init()
{
    // Create the mock instances
    mockPropertyProvider = new PropertyProvider();
    mockContextRegistryInfo = new ContextRegistryInfo();
}

// After each test
void PropertyHandleUnitTests::cleanup()
{
    delete mockPropertyProvider;
    mockPropertyProvider = 0;
    delete mockContextRegistryInfo;
    mockContextRegistryInfo = 0;
}

void PropertyHandleUnitTests::initializing()
{
    // Create the object to be tested
    QString key = "Property." + QString(__FUNCTION__);
    propertyHandle = PropertyHandle::instance(key);
    // Note: For each test, we need to create a separate instance.
    // Otherwise the tests are dependent on each other.

}

QTEST_MAIN(PropertyHandleUnitTests);
