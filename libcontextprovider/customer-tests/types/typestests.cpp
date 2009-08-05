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

#include "typestests.h"

#include "context.h"
#include "sconnect.h"

#include <QtTest/QtTest>
#include <QStringList>
#include <QProcess>

namespace ContextProvider {

TypesTests::TypesTests()
    : service (QDBusConnection::SessionBus, "org.freedesktop.ContextKit.testProvider1", this),
      intItem (service, "Test.Int", this),
      stringItem (service, "Test.String", this),
      boolItem (service, "Test.Bool", this),
      doubleItem (service, "Test.Double", this),
      stringListItem (service, "Test.Stringlist", this),
      charItem (service, "Test.Char", this),
      dateItem (service, "Test.Date", this),
      timeItem (service, "Test.Time", this)
{
    service.start();
}

void TypesTests::initTestCase()
{
}

void TypesTests::cleanupTestCase()
{
}

// Before each test
void TypesTests::init()
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
void TypesTests::cleanup()
{
    // Stop the client
    if (clientStarted) {
        client->kill();
        client->waitForFinished();
    }
    delete client; client = NULL;

    service.stop();
}

void TypesTests::typesInReturnValueOfSubscribe()
{
    // Check that the initialization went well.
    // Doing this only in init() is not enough; doesn't stop the test case.
    QVERIFY(clientStarted);

    // Ask the client to call GetSubscriber, ignore the result
    writeToClient("getsubscriber session org.freedesktop.ContextKit.testProvider1\n");

    // Set some values to the properties
    intItem.set(4510);
    doubleItem.set(-9.031);
    stringItem.set("this-is-a-test-string");
    boolItem.set(false);
    QStringList temp;
    temp << "string1" << "string2";
    stringListItem.set(temp);
    charItem.set(QChar('g'));
    dateItem.set(QDate(2009, 8, 5));
    timeItem.set(QTime(14, 30, 20));

    // Test: subscribe to properties
    QString actual = writeToClient("subscribe org.freedesktop.ContextKit.testProvider1 test.int test.double "
                                   "test.string test.bool\n");

    // Expected result: the values are printed correctly
    QString expected ="Known keys: test.bool(bool:false) test.double(double:-9.031) test.int(int:4510) "
                     "test.string(QString:this-is-a-test-string) Unknown keys: ";
    QCOMPARE(actual.simplified(), expected.simplified());

    // Test: subscribe to more properties
    actual = writeToClient("subscribe org.freedesktop.ContextKit.testProvider1 test.stringlist\n");

    // Expected result: the values are printed correctly
    expected = "Known keys: test.stringlist(QStringList:string1/string2) Unknown keys: ";
    QCOMPARE(actual.simplified(), expected.simplified());

    /* FIXME: Other types are not working yet.
    // Test: subscribe to more properties
    actual = writeToClient("subscribe org.freedesktop.ContextKit.testProvider1 test.date\n");

    // Expected result: the values are printed correctly
    expected = "Known keys: test.stringlist(QDBusArgument:) Unknown keys: ";
    QCOMPARE(actual.simplified(), expected.simplified());
    */
}

void TypesTests::typesInChangeSignal()
{
    // Check that the initialization went well.
    // Doing this only in init() is not enough; doesn't stop the test case.
    QVERIFY(clientStarted);

    // Ask the client to call GetSubscriber, ignore the result
    writeToClient("getsubscriber session org.freedesktop.ContextKit.testProvider1\n");

    // Set some values to the properties
    intItem.set(4510);
    doubleItem.set(-9.031);
    stringItem.set("this-is-a-test-string");
    boolItem.set(false);
    QStringList temp;
    temp << "string1" << "string2";
    stringListItem.set(temp);
    charItem.set(QChar('g'));
    dateItem.set(QDate(2009, 8, 5));
    timeItem.set(QTime(14, 30, 20));

    // Subscribe to properties, ignore the return values
    writeToClient("subscribe org.freedesktop.ContextKit.testProvider1 test.int test.double "
                                   "test.string test.bool\n");
    writeToClient("subscribe org.freedesktop.ContextKit.testProvider1 test.stringlist\n");
    //writeToClient("subscribe org.freedesktop.ContextKit.testProvider1 test.char test.date test.time\n");
    // FIXME: Complex types not working yet!

    // Test: modify the properties
    intItem.set(-11);
    doubleItem.set(4.88);
    stringItem.set("anotherstring");
    boolItem.set(true);
    // And tell the client to wait for Changed signal
    QString actual = writeToClient("waitforchanged 3000\n");

    // Expected result: The client got the Changed signal with correct values
    QString expected = "Changed signal received, parameters: Known keys: test.bool(bool:true) test.double(double:4.88) "
        "test.int(int:-11) test.string(QString:anotherstring) Unknown keys:";

    QCOMPARE(actual.simplified(), expected.simplified());

    // Reset the client (make it forget the previous Changed signal)
    writeToClient("resetsignalstatus\n");

    // Test: modify the properties
    temp.clear();
    temp << "something" << "else" << "here";
    stringListItem.set(temp);
    // And tell the client to wait for Changed signal
    actual = writeToClient("waitforchanged 3000\n");

    // Expected result: The client got the Changed signal with correct values
    expected = "Changed signal received, parameters: Known keys: "
        "test.stringlist(QStringList:something/else/here) Unknown keys:";

    QCOMPARE(actual.simplified(), expected.simplified());
}


void TypesTests::readStandardOutput()
{
    isReadyToRead = true;
}

QString TypesTests::writeToClient(const char* input)
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

QTEST_MAIN(ContextProvider::TypesTests);
