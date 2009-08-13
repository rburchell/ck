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
#include "sconnect.h"

#include "service.h"
#include "property.h"

#include <QtTest/QtTest>
#include <QStringList>
#include <QProcess>

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
    // Start the services
    service = new Service(QDBusConnection::SessionBus, "org.freedesktop.ContextKit.testProvider1");
    test_int = new Property(*service, "Test.Int");
    test_double = new Property(*service, "Test.Double");
    service->start();

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
void ValueChangesTests::cleanup()
{
    // Stop the client
    if (clientStarted) {
        client->kill();
        client->waitForFinished();
    }
    delete client; client = NULL;

    // Stop the service
    service->stop();

    delete service; service = NULL;

    delete test_int; test_int = NULL;
    delete test_double; test_double = NULL;
}

void ValueChangesTests::subscribedPropertyChanges()
{
    // Check that the initialization went well.
    // Doing this only in init() is not enough; doesn't stop the test case.
    QVERIFY(clientStarted);

    // Ask the client to call GetSubscriber, ignore the result
    writeToClient("getsubscriber session org.freedesktop.ContextKit.testProvider1\n");

    // Subscribe to a property (which is currently unknown)
    writeToClient("subscribe org.freedesktop.ContextKit.testProvider1 Test.Double\n");

    // Test: Change the value of the property
    test_double->setValue(51.987);
    // and tell the client to wait for the Changed signal
    QString actual = writeToClient("waitforchanged 3000\n");

    // Expected result: the client got the Changed signal
    QString expected = "Changed signal received, parameters: Known keys: Test.Double(double:51.987) Unknown keys:";

    QCOMPARE(actual.simplified(), expected.simplified());

    // Reset the signal status of the client (makes it forget that it has received the signal)
    writeToClient("resetsignalstatus\n");

    // Test: Change a property to unknown
    test_double->unsetValue();
    // and tell the client to wait for the Changed signal
    actual = writeToClient("waitforchanged 3000\n");

    // Expected result: the client got the Changed signal
    expected = "Changed signal received, parameters: Known keys: Unknown keys: Test.Double";

    QCOMPARE(actual.simplified(), expected.simplified());
}

void ValueChangesTests::nonsubscribedPropertyChanges()
{
    // Check that the initialization went well.
    // Doing this only in init() is not enough; doesn't stop the test case.
    QVERIFY(clientStarted);

    // Ask the client to call GetSubscriber, ignore the result
    writeToClient("getsubscriber session org.freedesktop.ContextKit.testProvider1\n");

    // Subscribe to a property (which is currently unknown)
    writeToClient("subscribe org.freedesktop.ContextKit.testProvider1 Test.Double\n");

    // Test: Change the value of the property
    test_int->setValue(100);
    // and tell the client to wait for the Changed signal
    QString actual = writeToClient("waitforchanged 3000\n");

    // Expected result: the client didn't get the Changed signal since it wasn't subscribed
    QString expected = "Timeout";

    QCOMPARE(actual.simplified(), expected.simplified());

    // Reset the signal status of the client (makes it forget that it has received the signal)
    writeToClient("resetsignalstatus\n");

    // Test: Change a property to unknown
    test_int->unsetValue();
    // and tell the client to wait for the Changed signal
    actual = writeToClient("waitforchanged 3000\n");

    // Expected result: the client didn't get the Changed signal since it wasn't subscribed
    expected = "Timeout";

    QCOMPARE(actual.simplified(), expected.simplified());
}

void ValueChangesTests::unsubscribedPropertyChanges()
{
    // Check that the initialization went well.
    // Doing this only in init() is not enough; doesn't stop the test case.
    QVERIFY(clientStarted);

    // Ask the client to call GetSubscriber, ignore the result
    writeToClient("getsubscriber session org.freedesktop.ContextKit.testProvider1\n");

    // Subscribe to a property (which is currently unknown)
    writeToClient("subscribe org.freedesktop.ContextKit.testProvider1 Test.Double\n");

    // Unsubscribe from the property
    writeToClient("unsubscribe org.freedesktop.ContextKit.testProvider1 Test.Double\n");

    // Test: Change the value of the property
    test_int->setValue(100);
    // and tell the client to wait for the Changed signal
    QString actual = writeToClient("waitforchanged 3000\n");

    // Expected result: the client didn't get the Changed signal since it wasn't subscribed
    QString expected = "Timeout";

    QCOMPARE(actual.simplified(), expected.simplified());

    // Reset the signal status of the client (makes it forget that it has received the signal)
    writeToClient("resetsignalstatus\n");

    // Test: Change a property to unknown
    test_int->unsetValue();
    // and tell the client to wait for the Changed signal
    actual = writeToClient("waitforchanged 3000\n");

    // Expected result: the client didn't get the Changed signal since it wasn't subscribed
    expected = "Timeout";

    QCOMPARE(actual.simplified(), expected.simplified());
}

void ValueChangesTests::twoPropertiesChange()
{
    // Check that the initialization went well.
    // Doing this only in init() is not enough; doesn't stop the test case.
    QVERIFY(clientStarted);

    // Ask the client to call GetSubscriber, ignore the result
    writeToClient("getsubscriber session org.freedesktop.ContextKit.testProvider1\n");

    // Subscribe to 2 properties (which are currently unknown)
    writeToClient("subscribe org.freedesktop.ContextKit.testProvider1 Test.Int Test.Double\n");

    // Test: Change the value of both properties
    test_int->setValue(100);
    test_double->setValue(4.111);
    // and tell the client to wait for the Changed signal
    QString actual = writeToClient("waitforchanged 3000\n");

    // Expected result: the client gets one Changed signal with both properties
    QString expected = "Changed signal received, parameters: Known keys: Test.Double(double:4.111) "
        "Test.Int(int:100) Unknown keys:";

    QCOMPARE(actual.simplified(), expected.simplified());

    // Reset the signal status of the client (makes it forget that it has received the signal)
    writeToClient("resetsignalstatus\n");

    // Test: Set both properties to unknown
    test_int->unsetValue();
    test_double->unsetValue();
    // and tell the client to wait for the Changed signal
    actual = writeToClient("waitforchanged 3000\n");

    // Expected result: the client gets one Changed signal with both properties
    expected = "Changed signal received, parameters: Known keys: Unknown keys: Test.Double Test.Int";

    QCOMPARE(actual.simplified(), expected.simplified());

}

void ValueChangesTests::sameValueSet()
{
    // Check that the initialization went well.
    // Doing this only in init() is not enough; doesn't stop the test case.
    QVERIFY(clientStarted);

    // Ask the client to call GetSubscriber, ignore the result
    writeToClient("getsubscriber session org.freedesktop.ContextKit.testProvider1\n");

    // Set a value to a property
    test_int->setValue(555);

    // Subscribe to 2 properties (one is currently unknown, the other has a value)
    writeToClient("subscribe org.freedesktop.ContextKit.testProvider1 Test.Int Test.Double\n");

    // Test: Set the value of the property to its current value
    test_int->setValue(555);
    // and tell the client to wait for the Changed signal
    QString actual = writeToClient("waitforchanged 3000\n");

    // Expected result: the client didn't get the Changed signal since the value didn't really change
    QString expected = "Timeout";

    QCOMPARE(actual.simplified(), expected.simplified());

    // Reset the signal status of the client (makes it forget that it has received the signal)
    writeToClient("resetsignalstatus\n");

    // Test: Unset a property which is already unknown
    test_double->unsetValue();
    // and tell the client to wait for the Changed signal
    actual = writeToClient("waitforchanged 3000\n");

    // Expected result: the client didn't get the Changed signal since the value didn't really change
    expected = "Timeout";

    QCOMPARE(actual.simplified(), expected.simplified());
}

void ValueChangesTests::changesBetweenZeroAndUnknown()
{
    // Check that the initialization went well.
    // Doing this only in init() is not enough; doesn't stop the test case.
    QVERIFY(clientStarted);

    // Ask the client to call GetSubscriber, ignore the result
    writeToClient("getsubscriber session org.freedesktop.ContextKit.testProvider1\n");

    // Subscribe to a property (which is currently unknown)
    QString actual = writeToClient("subscribe org.freedesktop.ContextKit.testProvider1 Test.Int\n");

    // Verify that the property really was unknown
    QString expected = "Known keys: Unknown keys: Test.Int";
    QCOMPARE(actual.simplified(), expected.simplified());

    // Test: Change the value of the property to 0
    test_int->setValue(0);
    // and tell the client to wait for the Changed signal
    actual = writeToClient("waitforchanged 3000\n");

    // Expected result: the client got the Changed signal
    expected = "Changed signal received, parameters: Known keys: Test.Int(int:0) Unknown keys:";

    QCOMPARE(actual.simplified(), expected.simplified());

    // Reset the signal status of the client (makes it forget that it has received the signal)
    writeToClient("resetsignalstatus\n");

    // Test: Change the property to unknown again
    test_int->unsetValue();
    // and tell the client to wait for the Changed signal
    actual = writeToClient("waitforchanged 3000\n");

    // Expected result: the client got the Changed signal
    expected = "Changed signal received, parameters: Known keys: Unknown keys: Test.Int";

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
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    }
    // Return the output from the client
    return client->readAll();
}


} // end namespace

QTEST_MAIN(ContextProvider::ValueChangesTests);
