/*
 * Copyright (C) 2008-2010 Nokia Corporation.
 *
 * Contact: Marius Vollmer <marius.vollmer@nokia.com>
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

#include "testwaitforsubscription.h"

#include "contextproperty.h"
#include "sconnect.h"

#include <QtTest/QtTest>
#include <QDebug>
#include <QProcess>

#include <stdlib.h>

WaitForSubscriptionTests::WaitForSubscriptionTests()
    : providerStarted(false), isReadyToRead(false)
{
}

void WaitForSubscriptionTests::initTestCase()
{
    setenv("CONTEXT_PROVIDERS", ".", 0);
    provider = new QProcess();
    provider->start("context-provide",
                    QStringList() << "--session"
                    << "com.nokia.test"
                    << "string" << "Test.Prop" << "someValue"
                    << "string" << "Test.Prop2" << "someOther"
                    << "string" << "Test.Prop3" << "thirdValue"
                    << "string" << "Test.Prop4" << "fourthValue");
    // Record whether the client was successfully started
    sconnect(provider, SIGNAL(readyReadStandardOutput()),
             this, SLOT(readStandardOutput()));

    providerStarted = provider->waitForStarted();

    while (!isReadyToRead) {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    }
}

void WaitForSubscriptionTests::cleanupTestCase()
{
    if (providerStarted) {
        provider->kill();
        provider->waitForFinished();
    }
    delete provider;
}

void WaitForSubscriptionTests::waitAndBlockExisting()
{
    QVERIFY(providerStarted);

    ContextProperty prop("Test.Prop");
    Helper h;
    QTimer::singleShot(0, &h, SLOT(onTimeout()));

    prop.waitForSubscriptionAndBlock();

    // The event loop hasn't spinned...
    QVERIFY(!h.processed);

    // And the property should have a value
    QCOMPARE(prop.value(), QVariant(QString("someValue")));

    // For test sanity; check that we *did* schedule the event correctly
    QTest::qWait(100);
    QVERIFY(h.processed);
}

void WaitForSubscriptionTests::waitAndBlockNonExisting()
{
    QVERIFY(providerStarted);

    ContextProperty prop("Test.NotThereAtAll");
    Helper h;
    QTimer::singleShot(0, &h, SLOT(onTimeout()));

    prop.waitForSubscriptionAndBlock();

    // The event loop hasn't spinned...
    QVERIFY(!h.processed);

    // And the property shouldn't have a value
    QCOMPARE(prop.value(), QVariant());

    // For test sanity; check that we *did* schedule the event correctly
    QTest::qWait(100);
    QVERIFY(h.processed);
}

void WaitForSubscriptionTests::waitAndSpinExisting()
{
    QVERIFY(providerStarted);

    // Use a different property here, to make sure the previous test doesn't
    // affect this one.
    ContextProperty prop("Test.Prop2");
    Helper h;
    QTimer::singleShot(0, &h, SLOT(onTimeout()));

    prop.waitForSubscription();

    // The event loop has spinned...
    QVERIFY(h.processed);

    // And the property should have a value
    QCOMPARE(prop.value(), QVariant(QString("someOther")));
}

void WaitForSubscriptionTests::waitAndSpinNonExisting()
{
    QVERIFY(providerStarted);

    ContextProperty prop("Test.NotThereAtAll");
    Helper h;
    QTimer::singleShot(0, &h, SLOT(onTimeout()));

    prop.waitForSubscription();

    // The event loop hasn't spinned, since we realize so early that "we cannot
    // ever hope to subscribe to this property".
    QVERIFY(!h.processed);

    // And the property shouldn't have a value
    QCOMPARE(prop.value(), QVariant());

    // For test sanity; check that we *did* schedule the event correctly
    QTest::qWait(100);
    QVERIFY(h.processed);
}

void WaitForSubscriptionTests::waitAndBlockSubscribed()
{
    QVERIFY(providerStarted);

    ContextProperty prop("Test.Prop3");

    // Wait until the property has a value...
    while (prop.value().isNull())
        QTest::qWait(100);

    Helper h;
    QTimer::singleShot(0, &h, SLOT(onTimeout()));

    prop.waitForSubscriptionAndBlock();

    // The event loop hasn't spinned...
    QVERIFY(!h.processed);

    // And the property should have a value
    QCOMPARE(prop.value(), QVariant(QString("thirdValue")));

    // For test sanity; check that we *did* schedule the event correctly
    QTest::qWait(100);
    QVERIFY(h.processed);
}

void WaitForSubscriptionTests::waitAndSpinSubscribed()
{
    QVERIFY(providerStarted);

    ContextProperty prop("Test.Prop4");

    // Wait until the property has a value...
    while (prop.value().isNull())
        QTest::qWait(100);

    Helper h;
    QTimer::singleShot(0, &h, SLOT(onTimeout()));

    prop.waitForSubscription();

    // The event loop hasn't spinned since the property was already subscribed
    QVERIFY(!h.processed);

    // And the property should have a value
    QCOMPARE(prop.value(), QVariant(QString("fourthValue")));

    // For test sanity; check that we *did* schedule the event correctly
    QTest::qWait(100);
    QVERIFY(h.processed);
}

void WaitForSubscriptionTests::readStandardOutput()
{
    isReadyToRead = true;
}

QTEST_MAIN(WaitForSubscriptionTests);
