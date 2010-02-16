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

#define SERVICE_NAME1 "org.maemo.contextkit.testProvider1"
#define SERVICE_NAME2 "org.maemo.contextkit.testProvider2"

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
    service1 = new Service(QDBusConnection::SessionBus, SERVICE_NAME1);
    test_int = new Property(*service1, "Test.Int");
    test_double = new Property(*service1, "Test.Double");

    service2 = new Service(QDBusConnection::SessionBus, SERVICE_NAME2);
    test_string = new Property (*service2, "Test.String");
    test_bool = new Property(*service2, "Test.Bool");

    // Nullify the values (we create the same Properties over and
    // over, and they will keep their old values.
    test_int->unsetValue();
    test_double->unsetValue();
    test_string->unsetValue();
    test_bool->unsetValue();

    // Initialize test program state
    isReadyToRead = false;

    // Start the client
    client = new QProcess();
    sconnect(client, SIGNAL(readyReadStandardOutput()), this, SLOT(readStandardOutput()));
    client->start("client");
    // Record whether the client was successfully started
    clientStarted = client->waitForStarted();

    // Associate shorter names for the test services when communicating with the client
    if (clientStarted) {
        writeToClient("assign session " SERVICE_NAME1 " service1\n");
        writeToClient("assign session " SERVICE_NAME2 " service2\n");
    }
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

    delete test_int; test_int = NULL;
    delete test_double; test_double = NULL;
    delete test_bool; test_bool = NULL;
    delete test_string; test_string = NULL;

    delete service1; service1 = NULL;
    delete service2; service2 = NULL;
}

void SubscriptionTests::subscribeReturnValueForUnknownProperty()
{
    // Check that the initialization went well.
    // Doing this only in init() is not enough; doesn't stop the test case.
    QVERIFY(clientStarted);

    // Ask the client to call Subscribe with 1 valid key. The property
    // is currently unknown since we haven't set a value for it.
    QString actual = writeToClient("subscribe service1 Test.Int\n");

    // Expected result: The return value of Subscribe contains the key as unknown.
    QString expected("Subscribe returned: Unknown");
    QCOMPARE(actual.simplified(), expected.simplified());
}

void SubscriptionTests::subscribeReturnValueForKnownProperty()
{
    // Check that the initialization went well.
    // Doing this only in init() is not enough; doesn't stop the test case.
    QVERIFY(clientStarted);

    // Set a value for a property
    test_double->setValue(-8.22);

    // Ask the client to call Subscribe with 1 valid key. The property
    // has a value we just set.
    QString actual = writeToClient("subscribe service1 Test.Double\n");

    // Expected result: The return value of Subscribe contains the key and its value.
    QString expected("Subscribe returned: double:-8.22");
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

    // Test: ask the client to call Subscribe to 2 keys
    writeToClient("subscribe service1 Test.Int\n");
    writeToClient("subscribe service1 Test.Double\n");

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
    writeToClient("subscribe service2 Test.Bool\n");

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
    writeToClient("unsubscribe service1 Test.Double\n");

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
    writeToClient("unsubscribe service1 Test.Int\n");
    writeToClient("unsubscribe service2 Test.Bool\n");

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

// Note: input must end with \n
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

void SubscriptionTests::multiSubscribe()
{
    // Check that the initialization went well.
    // Doing this only in init() is not enough; doesn't stop the test case.
    QVERIFY(clientStarted);

    // Ask the client to call Subscribe for a property twice. The
    // property exists and is currently unknown since we haven't set a
    // value for it.
    writeToClient("subscribe service1 Test.Int\n");

    QString actual = writeToClient("subscribe service1 Test.Int\n");

    // Expected result: Unsubscribe returns a D-Bus error
    QString expected("Subscribe error: org.maemo.contextkit.Error.MultipleSubscribe");
    QCOMPARE(actual.simplified(), expected.simplified());
}

void SubscriptionTests::illegalUnsubscribe()
{
    // Check that the initialization went well.
    // Doing this only in init() is not enough; doesn't stop the test case.
    QVERIFY(clientStarted);

    // Ask the client to call Unubscribe for a property which we're
    // not subscribed to. The property exists and is currently unknown
    // since we haven't set a value for it.
    QString actual = writeToClient("unsubscribe service1 Test.Int\n");

    // Expected result: Unsubscribe returns a D-Bus error
    QString expected("Unsubscribe error: org.maemo.contextkit.Error.IllegalUnsubscribe");
    QCOMPARE(actual.simplified(), expected.simplified());
}

void SubscriptionTests::clientExits()
{
    // Check that the initialization went well.
    // Doing this only in init() is not enough; doesn't stop the test case.
    QVERIFY(clientStarted);

    QSignalSpy intItemFirst(test_int, SIGNAL(firstSubscriberAppeared(const QString&)));
    QSignalSpy intItemLast(test_int, SIGNAL(lastSubscriberDisappeared(const QString&)));

    // Test: ask the client to call Subscribe
    writeToClient("subscribe service1 Test.Int\n");

    // Expected result: we get a notification that the key was subscribed
    QCOMPARE(intItemFirst.count(), 1);
    QCOMPARE(intItemLast.count(), 0);

    // Test: kill the client
    client->kill();
    client->waitForFinished();
    clientStarted = false;

    // It takes some time and event processing until we get the signal
    int i = 0;
    while (intItemLast.count() == 0 && i++ < 100)
        QCoreApplication::processEvents(QEventLoop::AllEvents);

    // Expected result: we get a notification that the key was unsubscribed
    QCOMPARE(intItemFirst.count(), 1);
    QCOMPARE(intItemLast.count(), 1);
}

} // end namespace

QTEST_MAIN(ContextProvider::SubscriptionTests);
