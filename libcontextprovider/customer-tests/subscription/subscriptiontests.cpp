/*
 * This file is part of libcontextprovider.
 *
 * Copyright (C) 2009 Nokia Corporation.
 *
 * Contact: Jean-Luc Lamadon <jean-luc.lamadon@nokia.com>
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

#include "subscriptiontests.h"
#include "sconnect.h"

#include "service.h"
#include "property.h"

#include <QtTest/QtTest>
#include <QProcess>
#include <QStringList>

namespace ContextProvider {

void SubscriptionTests::initTestCase()
{
}

void SubscriptionTests::cleanupTestCase()
{
}

// Before each test
void SubscriptionTests::init()
{
    // Create the services
    service1 = new Service(QDBusConnection::SessionBus, "org.freedesktop.ContextKit.testProvider1");
    test_int = new Property(*service1, "Test.Int");
    test_double = new Property(*service1, "Test.Double");

    service2 = new Service(QDBusConnection::SessionBus, "org.freedesktop.ContextKit.testProvider2");
    test_string = new Property (*service2, "Test.String");
    test_bool = new Property(*service2, "Test.Bool");

    // Process the events so that the services get started
    QCoreApplication::processEvents(QEventLoop::AllEvents);

    // Initialize test program state
    isReadyToRead = false;

    // Start the client
    client = new QProcess();
    sconnect(client, SIGNAL(readyReadStandardOutput()), this, SLOT(readStandardOutput()));
    client->start("client");
    // Record whether the client was successfully started
    clientStarted = client->waitForStarted();
}

// After each test
void SubscriptionTests::cleanup()
{
    // Stop the client
    if (clientStarted) {
        client->kill();
        client->waitForFinished();
    }
    delete client; client = NULL;

    delete service1; service1 = NULL;
    delete service2; service2 = NULL;

    delete test_int; test_int = NULL;
    delete test_double; test_double = NULL;
    delete test_bool; test_bool = NULL;
    delete test_string; test_string = NULL;
}

void SubscriptionTests::testGetSubscriber()
{
    // Check that the initialization went well.
    // Doing this only in init() is not enough; doesn't stop the test case.
    QVERIFY(clientStarted);

    QString actual = writeToClient("getsubscriber session org.freedesktop.ContextKit.testProvider1\n");

    QString expected("GetSubscriber returned /org/freedesktop/ContextKit/Subscriber/0");
    QCOMPARE(actual.simplified(), expected.simplified());
}

void SubscriptionTests::testGetSubscriberTwice()
{
    // Check that the initialization went well.
    // Doing this only in init() is not enough; doesn't stop the test case.
    QVERIFY(clientStarted);

    // Ask the client to call GetSubscriber
    QString result1 = writeToClient("getsubscriber session org.freedesktop.ContextKit.testProvider1\n");

    // Ask the client to call GetSubscriber again
    QString result2 = writeToClient("getsubscriber session org.freedesktop.ContextKit.testProvider1\n");

    // Expected result: the client got the same subscriber path twice
    QCOMPARE(result1, result2);
}

// Note: These tests don't verify that different clients get different
// subscribers. We might want to add a test case here.

void SubscriptionTests::subscribeReturnValueForUnknownProperty()
{
    // Check that the initialization went well.
    // Doing this only in init() is not enough; doesn't stop the test case.
    QVERIFY(clientStarted);

    // Ask the client to call GetSubscriber, ignore the result
    writeToClient("getsubscriber session org.freedesktop.ContextKit.testProvider1\n");

    // Ask the client to call Subscribe with 1 valid key. The property
    // is currently unknown since we haven't set a value for it.
    QString actual = writeToClient("subscribe org.freedesktop.ContextKit.testProvider1 Test.Int\n");

    // Expected result: The return value of Subscribe contains the key as unknown.
    QString expected("Known keys: Unknown keys: Test.Int");
    QCOMPARE(actual.simplified(), expected.simplified());
}

void SubscriptionTests::subscribeReturnValueForKnownProperty()
{
    // Check that the initialization went well.
    // Doing this only in init() is not enough; doesn't stop the test case.
    QVERIFY(clientStarted);

    // Ask the client to call GetSubscriber, ignore the result
    writeToClient("getsubscriber session org.freedesktop.ContextKit.testProvider1\n");

    // Set a value for a property
    test_double->setValue(-8.22);

    // Ask the client to call Subscribe with 1 valid key. The property
    // has a value we just set.
    QString actual = writeToClient("subscribe org.freedesktop.ContextKit.testProvider1 Test.Double\n");

    // Expected result: The return value of Subscribe contains the key and its value.
    QString expected("Known keys: Test.Double(double:-8.22) Unknown keys: ");
    QCOMPARE(actual.simplified(), expected.simplified());
}

void SubscriptionTests::subscribeReturnValueForInvalidProperty()
{
    // Check that the initialization went well.
    // Doing this only in init() is not enough; doesn't stop the test case.
    QVERIFY(clientStarted);

    // Ask the client to call GetSubscriber, ignore the result
    writeToClient("getsubscriber session org.freedesktop.ContextKit.testProvider1\n");

    // Ask the client to call Subscribe with 1 invalid key.
    QString actual = writeToClient("subscribe org.freedesktop.ContextKit.testProvider1 Test.Invalid\n");

    // Expected result: The return value of Subscribe contains the key as unknown.
    QString expected("Known keys: Unknown keys: ");
    QCOMPARE(actual.simplified(), expected.simplified());
}

void SubscriptionTests::subscriberNotifications()
{
    // Check that the initialization went well.
    // Doing this only in init() is not enough; doesn't stop the test case.
    QVERIFY(clientStarted);

    QSignalSpy intItemFirst(test_int, SIGNAL(firstSubscriberAppeared(const QString&)));
    QSignalSpy intItemLast(test_int, SIGNAL(lastSubscriberDisappeared(const QString&)));
    QSignalSpy doubleItemFirst(test_double, SIGNAL(firstSubscriberAppeared(const QString&)));
    QSignalSpy doubleItemLast(test_double, SIGNAL(lastSubscriberDisappeared(const QString&)));
    QSignalSpy stringItemFirst(test_string, SIGNAL(firstSubscriberAppeared(const QString&)));
    QSignalSpy stringItemLast(test_string, SIGNAL(lastSubscriberDisappeared(const QString&)));
    QSignalSpy boolItemFirst(test_bool, SIGNAL(firstSubscriberAppeared(const QString&)));
    QSignalSpy boolItemLast(test_bool, SIGNAL(lastSubscriberDisappeared(const QString&)));

    // Ask the client to call GetSubscriber (for both provider bus names), ignore the result
    writeToClient("getsubscriber session org.freedesktop.ContextKit.testProvider1\n");
    writeToClient("getsubscriber session org.freedesktop.ContextKit.testProvider2\n");

    // Test: ask the client to call Subscribe with 2 valid keys
    writeToClient("subscribe org.freedesktop.ContextKit.testProvider1 Test.Int Test.Double\n");

    // Expected result: we get the notifications for the subscribed keys
    QCOMPARE(intItemFirst.count(), 1);
    QCOMPARE(intItemLast.count(), 0);

    QCOMPARE(doubleItemFirst.count(), 1);
    QCOMPARE(doubleItemLast.count(), 0);

    // But not for the other ones
    QCOMPARE(stringItemFirst.count(), 0);
    QCOMPARE(stringItemLast.count(), 0);

    QCOMPARE(boolItemFirst.count(), 0);
    QCOMPARE(boolItemLast.count(), 0);

    // Clear the results
    intItemFirst.clear();
    doubleItemFirst.clear();

    // Test: subscribe to one more key (note different bus name)
    writeToClient("subscribe org.freedesktop.ContextKit.testProvider2 Test.Bool\n");

    // Expected result: a notification for the corresponding property, not for others
    QCOMPARE(intItemFirst.count(), 0);
    QCOMPARE(intItemLast.count(), 0);

    QCOMPARE(doubleItemFirst.count(), 0);
    QCOMPARE(doubleItemLast.count(), 0);

    QCOMPARE(boolItemFirst.count(), 1);
    QCOMPARE(boolItemLast.count(), 0);

    QCOMPARE(stringItemFirst.count(), 0);
    QCOMPARE(stringItemLast.count(), 0);

    // Clear the results
    boolItemFirst.clear();

    // Test: Unsubscribe from one of the keys
    writeToClient("unsubscribe org.freedesktop.ContextKit.testProvider1 Test.Double\n");

    // Expected result:
    QCOMPARE(intItemFirst.count(), 0);
    QCOMPARE(intItemLast.count(), 0);

    QCOMPARE(doubleItemFirst.count(), 0);
    QCOMPARE(doubleItemLast.count(), 1);

    QCOMPARE(boolItemFirst.count(), 0);
    QCOMPARE(boolItemLast.count(), 0);

    QCOMPARE(stringItemFirst.count(), 0);
    QCOMPARE(stringItemLast.count(), 0);

    // Clear the results
    doubleItemLast.clear();

    // Unsubscribe from the rest of the keys, too
    writeToClient("unsubscribe org.freedesktop.ContextKit.testProvider1 Test.Int\n");
    writeToClient("unsubscribe org.freedesktop.ContextKit.testProvider2 Test.Bool\n");

    // Expected result:
    QCOMPARE(intItemFirst.count(), 0);
    QCOMPARE(intItemLast.count(), 1);

    QCOMPARE(doubleItemFirst.count(), 0);
    QCOMPARE(doubleItemLast.count(), 0);

    QCOMPARE(boolItemFirst.count(), 0);
    QCOMPARE(boolItemLast.count(), 1);

    QCOMPARE(stringItemFirst.count(), 0);
    QCOMPARE(stringItemLast.count(), 0);

    // Clear the results
    intItemLast.clear();
    boolItemLast.clear();
}

void SubscriptionTests::readStandardOutput()
{
    isReadyToRead = true;
}

QString SubscriptionTests::writeToClient(const char* input)
{
    isReadyToRead = false;
    client->write(input);
    client->waitForBytesWritten();
    // Blocking for reading operation is bad idea since the client
    // expects provider to reply to dbus calls

    while (!isReadyToRead) {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    }
    // Return the output from the client
    return client->readAll();
}


} // end namespace

QTEST_MAIN(ContextProvider::SubscriptionTests);