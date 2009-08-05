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
#include <QtTest/QtTest>

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
    // Initialize test program state
    isReadyToRead = false;

    // Start the provider
    QStringList keysProvider1;
    keysProvider1.append("test.int");
    keysProvider1.append("test.double");

    QStringList keysProvider2;
    keysProvider2.append("test.string");
    keysProvider2.append("test.bool");

    Property::initService(QDBusConnection::SessionBus,
            "org.freedesktop.ContextKit.testProvider1",
            keysProvider1);

    Property::initService(QDBusConnection::SessionBus,
            "org.freedesktop.ContextKit.testProvider2",
            keysProvider2);

    intItem = new Property("test.int");
    doubleItem = new Property("test.double");

    stringItem = new Property("test.string");
    boolItem = new Property("test.bool");

    // Start the client
    client = new QProcess();
    clientName = "client";
    sconnect(client, SIGNAL(readyReadStandardOutput()), this, SLOT(readStandardOutput()));
    client->start(clientName);
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

    // Stop the provider
    Property::stopService("org.freedesktop.ContextKit.testProvider1");
    Property::stopService("org.freedesktop.ContextKit.testProvider2");

    delete intItem;
    delete doubleItem;
    delete stringItem;
    delete boolItem;
}

void SubscriptionTests::testGetSubscriber()
{
    // Check that the initialization went well.
    // Doing this only in init() is not enough; doesn't stop the test case.
    QVERIFY(clientStarted);

    qDebug() << "testgetsubs";
    QString actual = writeToClient("getsubscriber session org.freedesktop.ContextKit.testProvider1\n");

    QString expected("GetSubscriber returned /org/freedesktop/ContextKit/Subscriber/0");
    QCOMPARE(actual.simplified(), expected.simplified());
    qDebug() << "testgetsubs done";

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

void SubscriptionTests::subscribeToUnknownProperty()
{
    // Check that the initialization went well.
    // Doing this only in init() is not enough; doesn't stop the test case.
    QVERIFY(clientStarted);

    // Ask the client to call GetSubscriber, ignore the result
    writeToClient("getsubscriber session org.freedesktop.ContextKit.testProvider1\n");

    // Ask the client to call Subscribe with 1 valid key. The property
    // is currently unknown since we haven't set a value for it.
    QString actual = writeToClient("subscribe org.freedesktop.ContextKit.testProvider1 test.int\n");

    QString expected("Known keys: Unknown keys: test.int");
    QCOMPARE(actual.simplified(), expected.simplified());
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
    // expects provider to reply to getsubscriber dbus call

    while (!isReadyToRead) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
    // Return the output from the client
    return client->readAll();
}


} // end namespace

QTEST_MAIN(ContextProvider::SubscriptionTests);
