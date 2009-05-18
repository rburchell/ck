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
#include "testpropertyprovider.h"

// Mock header files
#include "dbusnamelistener.h"
#include "handlesignalrouter.h"
#include "subscriberinterface.h"

// Header file of the class to be tested
#include "propertyprovider.h"

#include <QtTest/QtTest>
#include <QDebug>
#include <QDBusConnection>

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

// Mock instances
DBusNameListener* mockDBusNameListener;
HandleSignalRouter* mockHandleSignalRouter;

// Mock implementation of the ManagerInterface

ManagerInterface::ManagerInterface(const QDBusConnection::BusType busType, const QString &busName, QObject *parent)
{
}


void ManagerInterface::getSubscriber()
{
}

bool ManagerInterface::isGetSubscriberFailed() const
{
    return false;
}

// Mock implementation of the SubscriberInterface
SubscriberInterface::SubscriberInterface(const QDBusConnection::BusType busType, const QString& busName,
                        const QString& objectPath, QObject* parent)
{
}

void SubscriberInterface::subscribe(QSet<QString> keys)
{
}

void SubscriberInterface::unsubscribe(QSet<QString> keys)
{
}

// Mock implementation of the DBusNameListener

DBusNameListener* DBusNameListener::instance(const QDBusConnection::BusType busType, const QString &busName)
{
    return mockDBusNameListener;
}

DBusNameListener::DBusNameListener()
{
}


bool DBusNameListener::isServicePresent() const
{
    return true;
}

// Mock implementation of HandleSignalRouter
HandleSignalRouter* HandleSignalRouter::instance()
{
    return mockHandleSignalRouter;
}

void HandleSignalRouter::onValueChanged(QString key, QVariant value, bool processingSubscription)
{
}

//
// Definition of testcases
//

// Before all tests
void PropertyProviderUnitTests::initTestCase()
{
}

// After all tests
void PropertyProviderUnitTests::cleanupTestCase()
{
}

// Before each test
void PropertyProviderUnitTests::init()
{
    // Create the mock instances
    mockDBusNameListener = new DBusNameListener();
    mockHandleSignalRouter = new HandleSignalRouter();

    // Create the object to be tested
    propertyProvider = PropertyProvider::instance(QDBusConnection::SessionBus, "Fake.Bus.Name");
}

// After each test
void PropertyProviderUnitTests::cleanup()
{
    delete mockDBusNameListener;
    mockDBusNameListener = 0;
    delete propertyProvider;
    propertyProvider = 0;
}

void PropertyProviderUnitTests::temp()
{
    QVERIFY(1 == 1);
}

QTEST_MAIN(PropertyProviderUnitTests);
