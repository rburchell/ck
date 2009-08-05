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

#include "valuechangestests.h"
#include <QtTest/QtTest>

namespace ContextProvider {

void ValueChangesTests::initTestCase()
{
}

void ValueChangesTests::cleanupTestCase()
{
}

// Before each test
void ValueChangesTests::init()
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
    sconnect(client, SIGNAL(readyReadStandardOutput()), this, SLOT(readStandardOutput()));
    client->start("client");
    // Record whether the client was successfully started
    clientStarted = client->waitForStarted();
}

// After each test
void ValueChangesTests::cleanup()
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

void ValueChangesTests::subscribedPropertyChanges()
{
    // Check that the initialization went well.
    // Doing this only in init() is not enough; doesn't stop the test case.
    QVERIFY(clientStarted);

    // Ask the client to call GetSubscriber, ignore the result
    writeToClient("getsubscriber session org.freedesktop.ContextKit.testProvider1\n");

    // Subscribe to a property (which is currently unknown)
    writeToClient("subscribe org.freedesktop.ContextKit.testProvider1 test.double\n");

    // Test: Change the value of the property
    doubleItem->set(51.987);
    // and tell the client to wait for the Changed signal
    QString actual = writeToClient("waitforchanged 5000\n");

    // Expected result: the client got the Changed signal
    QString expected = "Changed signal received";

    QCOMPARE(actual.simplified(), expected.simplified());
}

void ValueChangesTests::readStandardOutput()
{
    isReadyToRead = true;
}

QString ValueChangesTests::writeToClient(const char* input)
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

QTEST_MAIN(ContextProvider::ValueChangesTests);
