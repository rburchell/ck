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
#include "sconnect.h"

#include <QtTest/QtTest>
#include <QStringList>
#include <QProcess>

#define SERVICE_NAME1 "org.maemo.contextkit.testProvider1"

namespace ContextProvider {

void TypesTests::initTestCase()
{
}

void TypesTests::cleanupTestCase()
{
}

// Before each test
void TypesTests::init()
{
    // Start the services
    service = new Service(QDBusConnection::SessionBus, SERVICE_NAME1);

    intItem = new Property(*service, "Test.Int");
    stringItem = new Property(*service, "Test.String");
    boolItem = new Property(*service, "Test.Bool");
    doubleItem = new Property(*service, "Test.Double");
    stringListItem = new Property(*service, "Test.StringList");
    charItem = new Property(*service, "Test.Char");
    dateItem = new Property(*service, "Test.Date");
    timeItem = new Property (*service, "Test.Time");

    // Nullify the values (we create the same Properties over and
    // over, and they will keep their old values.
    intItem->unsetValue();
    stringItem->unsetValue();
    boolItem->unsetValue();
    doubleItem->unsetValue();
    stringListItem->unsetValue();
    charItem->unsetValue();
    dateItem->unsetValue();
    timeItem->unsetValue();

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
    }
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

    delete intItem; intItem = NULL;
    delete doubleItem; doubleItem = NULL;
    delete boolItem; boolItem = NULL;
    delete stringItem; stringItem = NULL;
    delete stringListItem; stringListItem = NULL;
    delete charItem; charItem = NULL;
    delete timeItem; timeItem = NULL;
    delete dateItem; dateItem = NULL;

    delete service; service = NULL;
}

void TypesTests::typesInReturnValueOfSubscribe()
{
    // Check that the initialization went well.
    // Doing this only in init() is not enough; doesn't stop the test case.
    QVERIFY(clientStarted);

    // Set some values to the properties
    intItem->setValue(4510);
    doubleItem->setValue(-9.031);
    stringItem->setValue("this-is-a-test-string");
    boolItem->setValue(false);
    QStringList temp;
    temp << "string1" << "string2";
    stringListItem->setValue(temp);
    charItem->setValue(QChar('g'));
    dateItem->setValue(QDate(2009, 8, 5));
    timeItem->setValue(QTime(14, 30, 20));

    // Test: subscribe to properties
    // Expected result: the values are printed correctly
    QString actual = writeToClient("subscribe service1 Test.Int\n");
    QString expected = "Subscribe returned: int:4510";
    QCOMPARE(actual.simplified(), expected.simplified());

    actual = writeToClient("subscribe service1 Test.Double\n");
    expected = "Subscribe returned: double:-9.031";
    QCOMPARE(actual.simplified(), expected.simplified());

    actual = writeToClient("subscribe service1 Test.Bool\n");
    expected = "Subscribe returned: bool:false";
    QCOMPARE(actual.simplified(), expected.simplified());

    actual = writeToClient("subscribe service1 Test.String\n");
    expected = "Subscribe returned: QString:this-is-a-test-string";
    QCOMPARE(actual.simplified(), expected.simplified());

    // Test: subscribe to more properties
    actual = writeToClient("subscribe service1 Test.StringList\n");

    // Expected result: the values are printed correctly
    expected = "Subscribe returned: QStringList:string1/string2";
    QCOMPARE(actual.simplified(), expected.simplified());

    /* FIXME: Other types are not working yet.
    // Test: subscribe to more properties
    actual = writeToClient("subscribe service1 test.date\n");

    // Expected result: the values are printed correctly
    expected = "Subscribe returned: QDBusArgument:";
    QCOMPARE(actual.simplified(), expected.simplified());
    */
}

void TypesTests::typesInChangeSignal()
{
    // Check that the initialization went well.
    // Doing this only in init() is not enough; doesn't stop the test case.
    QVERIFY(clientStarted);

    // Set some values to the properties
    intItem->setValue(4510);
    doubleItem->setValue(-9.031);
    stringItem->setValue("this-is-a-test-string");
    boolItem->setValue(false);
    QStringList temp;
    temp << "string1" << "string2";
    stringListItem->setValue(temp);
    charItem->setValue(QChar('g'));
    dateItem->setValue(QDate(2009, 8, 5));
    timeItem->setValue(QTime(14, 30, 20));

    // Subscribe to properties, ignore the return values
    writeToClient("subscribe service1 Test.Int\n");
    writeToClient("subscribe service1 Test.String\n");
    writeToClient("subscribe service1 Test.Bool\n");
    writeToClient("subscribe service1 Test.Double\n");
    writeToClient("subscribe service1 Test.StringList\n");

    //writeToClient("subscribe service1 test.char\n");
    //writeToClient("subscribe service1 test.date\n");
    //writeToClient("subscribe service1 test.time\n");
    // FIXME: Complex types not working yet!

    // Test: modify the properties
    // Expected result: The client got the Changed signal with correct values

    intItem->setValue(-11);

    QString actual = writeToClient("waitforchanged 3000\n");
    QString expected = "ValueChanged: org.maemo.contextkit.testProvider1 /org/maemo/contextkit/Test/Int int:-11";
    QCOMPARE(actual.simplified(), expected.simplified());
    writeToClient("resetsignalstatus\n");

    doubleItem->setValue(4.88);

    actual = writeToClient("waitforchanged 3000\n");
    expected = "ValueChanged: org.maemo.contextkit.testProvider1 /org/maemo/contextkit/Test/Double double:4.88";
    QCOMPARE(actual.simplified(), expected.simplified());
    writeToClient("resetsignalstatus\n");

    stringItem->setValue("anotherstring");

    actual = writeToClient("waitforchanged 3000\n");
    expected = "ValueChanged: org.maemo.contextkit.testProvider1 /org/maemo/contextkit/Test/String QString:anotherstring";
    QCOMPARE(actual.simplified(), expected.simplified());
    writeToClient("resetsignalstatus\n");

    boolItem->setValue(true);

    actual = writeToClient("waitforchanged 3000\n");
    expected = "ValueChanged: org.maemo.contextkit.testProvider1 /org/maemo/contextkit/Test/Bool bool:true";
    QCOMPARE(actual.simplified(), expected.simplified());
    writeToClient("resetsignalstatus\n");

    // Test: modify the more complex properties
    temp.clear();
    temp << "something" << "else" << "here";
    stringListItem->setValue(temp);
    actual = writeToClient("waitforchanged 3000\n");

    // Expected result: The client got the Changed signal with correct values
    expected = "ValueChanged: org.maemo.contextkit.testProvider1 /org/maemo/contextkit/Test/StringList QStringList:something/else/here";
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
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    }
    // Return the output from the client
    return client->readAll();
}


} // end namespace

QTEST_MAIN(ContextProvider::TypesTests);
