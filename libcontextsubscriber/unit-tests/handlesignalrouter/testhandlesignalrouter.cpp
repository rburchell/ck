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
#include "testhandlesignalrouter.h"

// Mock header files
#include "propertyhandle.h"

// Header file of the class to be tested
#include "handlesignalrouter.h"

#include <QtTest/QtTest>
#include <QDebug>
#include <QDBusConnection>

// Help the QSignalSpy handle QVariant
Q_DECLARE_METATYPE(QVariant);

namespace ContextSubscriber {

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
// These will be created by the test program
PropertyHandle* mockHandleOne;
PropertyHandle* mockHandleTwo;
PropertyHandle* mockHandleThree;

// Mock implementation of the PropertyHandle

PropertyHandle::PropertyHandle(const QString& key)
    : myKey(key)
{
}

void PropertyHandle::onValueChanged()
{
    Q_EMIT onValueChangedCalled(myKey);
}

void PropertyHandle::setSubscribeFinished(Provider *prov)
{
    Q_EMIT setSubscribeFinishedCalled(prov, myKey);
}

PropertyHandle* PropertyHandle::instance(const QString& key)
{
    if (key == "Property.One") {
        return mockHandleOne;
    }
    if (key == "Property.Two") {
        return mockHandleTwo;
    }
    if (key == "Property.Three") {
        return mockHandleThree;
    }
    return 0;
}


//
// Definition of testcases
//

// Before all tests
void HandleSignalRouterUnitTests::initTestCase()
{
    qInstallMsgHandler(myMessageOutput);
    // Help the QSignalSpy handle QVariant
    qRegisterMetaType<QVariant>("QVariant");
}

// After all tests
void HandleSignalRouterUnitTests::cleanupTestCase()
{
}

// Before each test
void HandleSignalRouterUnitTests::init()
{
    // Create the mock instances
    mockHandleOne = new PropertyHandle("Property.One");
    mockHandleTwo = new PropertyHandle("Property.Two");
    mockHandleThree = new PropertyHandle("Property.Three");
}

// After each test
void HandleSignalRouterUnitTests::cleanup()
{
    // Delete the mock objects
    delete mockHandleOne;
    mockHandleOne = 0;
    delete mockHandleTwo;
    mockHandleTwo = 0;
    delete mockHandleThree;
    mockHandleThree = 0;
}

void HandleSignalRouterUnitTests::routingSignals()
{
    // Setup:
    // Get the object to be tested
    handleSignalRouter = HandleSignalRouter::instance();
    // Start spying the mock handles
    QSignalSpy spy1(mockHandleOne, SIGNAL(onValueChangedCalled(QString)));
    QSignalSpy spy2(mockHandleTwo, SIGNAL(onValueChangedCalled(QString)));
    QSignalSpy spy3(mockHandleThree, SIGNAL(onValueChangedCalled(QString)));
    QSignalSpy sspy1(mockHandleOne, SIGNAL(setSubscribeFinishedCalled(Provider *, QString)));
    QSignalSpy sspy2(mockHandleTwo, SIGNAL(setSubscribeFinishedCalled(Provider *, QString)));
    QSignalSpy sspy3(mockHandleThree, SIGNAL(setSubscribeFinishedCalled(Provider *, QString)));

    // Test:
    // Send a signal to the HandleSignalRouter
    handleSignalRouter->onValueChanged("Property.One");
    handleSignalRouter->onSubscribeFinished(0, "Property.One");

    // Expected results:
    // The mockHandleOne.setValue was called
    QCOMPARE(spy1.count(), 1);
    QList<QVariant> parameters = spy1.takeFirst();
    QCOMPARE(parameters.size(), 1);
    QCOMPARE(parameters.at(0), QVariant("Property.One"));

    QCOMPARE(sspy1.count(), 1);
    parameters = sspy1.takeFirst();
    QCOMPARE(parameters.size(), 2);
    QCOMPARE(parameters.at(1), QVariant("Property.One"));

    // The setValue of other mock handles were not called
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 0);
    QCOMPARE(sspy2.count(), 0);
    QCOMPARE(sspy3.count(), 0);

    // Test:
    // Send a signal to the HandleSignalRouter
    handleSignalRouter->onValueChanged("Property.Two");
    handleSignalRouter->onSubscribeFinished(0, "Property.Two");

    // Expected results:
    // The mockHandleTwo.setValue was called
    QCOMPARE(spy2.count(), 1);
    QCOMPARE(sspy2.count(), 1);
    parameters = spy2.takeFirst();
    QCOMPARE(parameters.size(), 1);
    QCOMPARE(parameters.at(0), QVariant("Property.Two"));
    parameters = sspy2.takeFirst();
    QCOMPARE(parameters.size(), 2);
    QCOMPARE(parameters.at(1), QVariant("Property.Two"));
    // The setValue of other mock handles were not called
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy3.count(), 0);
    QCOMPARE(sspy1.count(), 0);
    QCOMPARE(sspy3.count(), 0);

}

} // end namespace

QTEST_MAIN(ContextSubscriber::HandleSignalRouterUnitTests);
