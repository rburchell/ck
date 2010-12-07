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
#include <string>

#define SERVICE_NAME "org.maemo.contextkit.testProvider"
int serviceNameIx = 0;

namespace ContextProvider {

void ServiceTest::initTestCase()
{
}

void ServiceTest::cleanupTestCase()
{
}

// Before each test
void ServiceTest::init()
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
void ServiceTest::cleanup()
{
    // Stop the client
    if (clientStarted) {
        client->kill();
        client->waitForFinished();
    }
    delete client; client = NULL;
}

void ServiceTest::startStopStart()
{
    // Check that the initialization went well.
    // Doing this only in init() is not enough; doesn't stop the test case.
    QVERIFY(clientStarted);

    // Use a different service name in each test; this way the ServiceBackends
    QString serviceName = QString::number(serviceNameIx).prepend(SERVICE_NAME);
    writeToClient(("assign session " + serviceName + " service\n").toStdString().c_str());
    ++serviceNameIx;

    // Test: create a Service and an associated Property
    Service* service = new Service(QDBusConnection::SessionBus, serviceName);
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

void ServiceTest::recreate()
{
    // Check that the initialization went well.
    // Doing this only in init() is not enough; doesn't stop the test case.
    QVERIFY(clientStarted);

    // Use a different service name in each test; this way the ServiceBackends
    QString serviceName = QString::number(serviceNameIx).prepend(SERVICE_NAME);
    writeToClient(("assign session " + serviceName + " service\n").toStdString().c_str());
    ++serviceNameIx;

    // Test: create a Service and an associated Property
    Service* service = new Service(QDBusConnection::SessionBus, serviceName);
    Property* property = new Property(*service, "Test.Property");

    // Test: command client to subscribe
    QString actual = writeToClient("subscribe service Test.Property\n");

    // Expected result: service is started automatically and Subscribe works
    QString expected = "Subscribe returned: Unknown";
    QCOMPARE(actual.simplified(), expected.simplified());

    // Test: delete the property and the service
    delete property;
    delete service;

    actual = writeToClient("subscribe service Test.Property\n");

    // Expected result: the client can no more subscribe
    expected = "Subscribe error: org.freedesktop.DBus.Error.ServiceUnknown";
    QCOMPARE(actual.simplified(), expected.simplified());

    // Test: recreate the service and try to subscribe again
    service = new Service(QDBusConnection::SessionBus, serviceName);
    property = new Property(*service, "Test.Property");
    actual = writeToClient("subscribe service Test.Property\n");

    // Expected result: service is started and Subscribe works
    expected = "Subscribe returned: Unknown";
    QCOMPARE(actual.simplified(), expected.simplified());

    delete service;
    delete property;
}

void ServiceTest::multiStart()
{
    // Check that the initialization went well.
    // Doing this only in init() is not enough; doesn't stop the test case.
    QVERIFY(clientStarted);

    // Use a different service name in each test; this way the ServiceBackends
    QString serviceName = QString::number(serviceNameIx).prepend(SERVICE_NAME);
    writeToClient(("assign session " + serviceName + " service\n").toStdString().c_str());
    ++serviceNameIx;

    // Test: create a Service and an associated Property
    Service* service = new Service(QDBusConnection::SessionBus, serviceName);
    Property* property = new Property(*service, "Test.Property");

    // Test: command client to subscribe
    QString actual = writeToClient("subscribe service Test.Property\n");

    // Expected result: service is started automatically and Subscribe works
    QString expected = "Subscribe returned: Unknown";
    QCOMPARE(actual.simplified(), expected.simplified());

    // Start listening to signals (we already have one subscriber)
    QSignalSpy firstSpy(property,
                        SIGNAL(firstSubscriberAppeared(const QString&)));
    QSignalSpy lastSpy(property,
                        SIGNAL(firstSubscriberAppeared(const QString&)));

    // Test: start the service again (even though it's started)
    service->start();

    // Expected result: the service is still there, and remembers the client
    actual = writeToClient("subscribe service Test.Property\n");
    expected = "Subscribe returned: Unknown";
    QCOMPARE(actual.simplified(), expected.simplified());

    // (so we don't get a signal)
    QTest::qWait(1000);
    QCOMPARE(firstSpy.count(), 0);
    QCOMPARE(lastSpy.count(), 0);

    delete service;
    delete property;
}

void ServiceTest::defaultService()
{
    // Check that the initialization went well.
    // Doing this only in init() is not enough; doesn't stop the test case.
    QVERIFY(clientStarted);

    // Use a different service name in each test; this way the ServiceBackends
    QString serviceName = QString::number(serviceNameIx).prepend(SERVICE_NAME);
    writeToClient(("assign session " + serviceName + " service\n").toStdString().c_str());
    ++serviceNameIx;

    // Test: create a Service. Set the Service as default. Then create
    // a Property without a Service.
    Service* service = new Service(QDBusConnection::SessionBus, serviceName);
    service->setAsDefault();
    Property* property = new Property("Test.Property");

    // Expected result: the Property got associated with the default
    // Service.

    // Test: command client to subscribe
    QString actual = writeToClient("subscribe service Test.Property\n");

    // Expected result: Subscribe works
    QString expected = "Subscribe returned: Unknown";
    QCOMPARE(actual.simplified(), expected.simplified());

    delete service;
    delete property;
}

void ServiceTest::recreateProperty()
{
    // Check that the initialization went well.
    // Doing this only in init() is not enough; doesn't stop the test case.
    QVERIFY(clientStarted);

    // Use a different service name in each test; this way the ServiceBackends
    QString serviceName = QString::number(serviceNameIx).prepend(SERVICE_NAME);
    writeToClient(("assign session " + serviceName + " service\n").toStdString().c_str());
    ++serviceNameIx;

    // Test: create a Service and an associated Property. Set a value
    // to the property.
    Service* service = new Service(QDBusConnection::SessionBus, serviceName);
    Property* property = new Property(*service, "Test.Property");

    property->setValue("keep this value");

    // Test: delete the property and create it again
    delete property;
    property = new Property(*service, "Test.Property");

    // Expeted result: the Property has kept its value
    QCOMPARE(property->value(), QVariant("keep this value"));

    delete property;
    delete service;
}

void ServiceTest::readStandardOutput()
{
    isReadyToRead = true;
}

// Note: input must end with \n
QString ServiceTest::writeToClient(const char* input)
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

QTEST_MAIN(ContextProvider::ServiceTest);
