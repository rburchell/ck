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

#include "servicetest.h"
#include "sconnect.h"

#include "service.h"
#include "property.h"

#include <QtTest/QtTest>
#include <QProcess>
#include <QStringList>

#define SERVICE_NAME "org.maemo.contextkit.testProvider"

namespace ContextProvider {

void ServiceTests::initTestCase()
{
}

void ServiceTests::cleanupTestCase()
{
}

// Before each test
void ServiceTests::init()
{
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
        writeToClient("assign session " SERVICE_NAME " service\n");
    }
}

// After each test
void ServiceTests::cleanup()
{
    // Stop the client
    if (clientStarted) {
        client->kill();
        client->waitForFinished();
    }
    delete client; client = NULL;
}

void ServiceTests::startStopStart()
{
    // Check that the initialization went well.
    // Doing this only in init() is not enough; doesn't stop the test case.
    QVERIFY(clientStarted);

    // Test: create a Service and an associated Property
    Service* service = new Service(QDBusConnection::SessionBus, SERVICE_NAME);
    Property* property = new Property(*service, "Test.Property");

    // Test: command client to subscribe
    QString actual = writeToClient("subscribe service Test.Property\n");

    // Expected result: service is started automatically and Subscribe works
    QString expected = "Subscribe returned: Unknown";
    QCOMPARE(actual.simplified(), expected.simplified());

    // Test: stop the service and try to subscribe again
    service->stop();
    actual = writeToClient("subscribe service Test.Property\n");

    // Expected result: the client can no more subscribe
    expected = "Subscribe error: org.freedesktop.DBus.Error.ServiceUnknown";
    QCOMPARE(actual.simplified(), expected.simplified());

    // Test: start the service and try to subscribe again
    service->start();
    actual = writeToClient("subscribe service Test.Property\n");

    // Expected result: service is started and Subscribe works
    expected = "Subscribe returned: Unknown";
    QCOMPARE(actual.simplified(), expected.simplified());

    delete service;
    delete property;
}

void ServiceTests::readStandardOutput()
{
    isReadyToRead = true;
}

// Note: input must end with \n
QString ServiceTests::writeToClient(const char* input)
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

QTEST_MAIN(ContextProvider::ServiceTests);
