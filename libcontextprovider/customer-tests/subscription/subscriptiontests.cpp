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
    isReadyToRead = false;
}

void SubscriptionTests::cleanupTestCase()
{
}

void SubscriptionTests::init()
{
}

void SubscriptionTests::cleanup()
{
}

void SubscriptionTests::testGetSubscriber()
{
    client = new QProcess();
    clientName = "client";
    sconnect(client, SIGNAL(readyReadStandardOutput()), this, SLOT(readStandardOutput()));
    client->start(clientName);
    client->waitForStarted();
    client->write("getsubscriber session org.freedesktop.ContextKit.testProvider1\n");
    client->waitForBytesWritten();
    // Blocking for reading operation is bad idea since the client
    // expects provider to reply to getsubscriber dbus call

    while (!isReadyToRead) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
    QString actual(client->readAll());
    QString expected("GetSubscriber returned /org/freedesktop/ContextKit/Subscriber/1");
    QCOMPARE(actual.simplified(),expected.simplified());
    client->kill();
    client->waitForFinished();
}

void SubscriptionTests::readStandardOutput()
{
    isReadyToRead = true;
}

} // end namespace

QTEST_MAIN(ContextProvider::SubscriptionTests);
