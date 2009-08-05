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

#include "context.h"
#include "sconnect.h"

#include <QtTest/QtTest>
#include <QProcess>
#include <QStringList>

namespace ContextProvider {

SubscriptionTests::SubscriptionTests()
    : service1 (QDBusConnection::SessionBus, "org.freedesktop.ContextKit.testProvider1", this),
      test_int (service1, "Test.Int", this),
      test_double (service1, "Test.Double", this),
      service2 (QDBusConnection::SessionBus, "org.freedesktop.ContextKit.testProvider2", this),
      test_string (service2, "Test.String", this),
      test_bool (service2, "Test.Bool", this)
{
    service1.start();
    service2.start();
}

void SubscriptionTests::initTestCase()
{
}

void SubscriptionTests::cleanupTestCase()
{
}

// Before each test
void SubscriptionTests::init()
{
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

    service1.stop();
    service2.stop();
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
    QString actual = writeToClient("subscribe org.freedesktop.ContextKit.testProvider1 test.int\n");

    // Expected result: The return value of Subscribe contains the key as unknown.
    QString expected("Known keys: Unknown keys: test.int");
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
    test_double.set(-8.22);

    // Ask the client to call Subscribe with 1 valid key. The property
    // has a value we just set.
    QString actual = writeToClient("subscribe org.freedesktop.ContextKit.testProvider1 test.double\n");

    // Expected result: The return value of Subscribe contains the key and its value.
    QString expected("Known keys: test.double(double:-8.22) Unknown keys: ");
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
    QString actual = writeToClient("subscribe org.freedesktop.ContextKit.testProvider1 test.invalid\n");

    // Expected result: The return value of Subscribe doesn't contain the key.
    QString expected("Known keys: Unknown keys: ");
    QCOMPARE(actual.simplified(), expected.simplified());
}

void SubscriptionTests::subscriberNotifications()
{
    // Check that the initialization went well.
    // Doing this only in init() is not enough; doesn't stop the test case.
    QVERIFY(clientStarted);

    QSignalSpy intItemFirst(intItem, SIGNAL(firstSubscriberAppeared(const QString&)));
    QSignalSpy intItemLast(intItem, SIGNAL(lastSubscriberDisappeared(const QString&)));
    QSignalSpy doubleItemFirst(doubleItem, SIGNAL(firstSubscriberAppeared(const QString&)));
    QSignalSpy doubleItemLast(doubleItem, SIGNAL(lastSubscriberDisappeared(const QString&)));
    QSignalSpy stringItemFirst(stringItem, SIGNAL(firstSubscriberAppeared(const QString&)));
    QSignalSpy stringItemLast(stringItem, SIGNAL(lastSubscriberDisappeared(const QString&)));
    QSignalSpy boolItemFirst(boolItem, SIGNAL(firstSubscriberAppeared(const QString&)));
    QSignalSpy boolItemLast(boolItem, SIGNAL(lastSubscriberDisappeared(const QString&)));

    // Ask the client to call GetSubscriber (for both provider bus names), ignore the result
    writeToClient("getsubscriber session org.freedesktop.ContextKit.testProvider1\n");
    writeToClient("getsubscriber session org.freedesktop.ContextKit.testProvider2\n");

    // Test: ask the client to call Subscribe with 2 valid keys
    writeToClient("subscribe org.freedesktop.ContextKit.testProvider1 test.int test.double\n");

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
    writeToClient("subscribe org.freedesktop.ContextKit.testProvider2 test.bool\n");

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
    writeToClient("unsubscribe org.freedesktop.ContextKit.testProvider1 test.double\n");

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
    writeToClient("unsubscribe org.freedesktop.ContextKit.testProvider1 test.int\n");
    writeToClient("unsubscribe org.freedesktop.ContextKit.testProvider2 test.bool\n");

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
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
    // Return the output from the client
    return client->readAll();
}


} // end namespace

QTEST_MAIN(ContextProvider::SubscriptionTests);
