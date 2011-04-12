/*
 * Copyright (C) 2008-2011 Nokia Corporation.
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

#include "testregression.h"

#include "contextproperty.h"
#include "sconnect.h"

#include <QtTest/QtTest>
#include <QDebug>
#include <QProcess>

#include <stdlib.h>

RegressionTests::RegressionTests()
    : providerStarted(false), isReadyToRead(false)
{
}

void RegressionTests::initTestCase()
{
    setenv("CONTEXT_PROVIDERS", ".", 0);
}

void RegressionTests::init()
{
    provider = new QProcess();
    provider->start("context-provide",
                    QStringList() << "--session"
                    << "com.nokia.test"
                    << "string" << "Test.Prop" << "\"firstValue\"");
    // Record whether the client was successfully started
    sconnect(provider, SIGNAL(readyReadStandardOutput()),
             this, SLOT(onProviderReadyRead()));

    providerStarted = provider->waitForStarted();

    while (true) {
        // wait until we see the "Service started" line
        isReadyToRead = false;
        while (!isReadyToRead) {
            QCoreApplication::processEvents(QEventLoop::AllEvents);
        }
        QString input = provider->readAllStandardOutput();
        if (input.contains("Service started"))
            break;
    }
}

void RegressionTests::cleanupTestCase()
{
}

void RegressionTests::cleanup()
{
    if (providerStarted) {
        provider->kill();
        provider->waitForFinished();
    }
    delete provider;
}

void RegressionTests::twoContextProperties()
{
    // Regression test for bug NB#239586
    QVERIFY(providerStarted);
    // Create 2 context properties, both are subscribed at first
    ContextProperty prop1("Test.Prop");
    ContextProperty prop2("Test.Prop");

    QSignalSpy spy1(&prop1, SIGNAL(valueChanged()));
    QSignalSpy spy2(&prop2, SIGNAL(valueChanged()));

    // Spin so that we get a valueChanged for both
    QTime timer;
    timer.start();
    while (timer.elapsed() < 5000 && (spy1.count() == 0 || spy2.count() == 0)) {
        QTest::qWait(100);
    }
    QCOMPARE(spy1.count(), 1);
    QCOMPARE(spy2.count(), 1);

    QCOMPARE(prop1.value(), QVariant("firstValue"));
    QCOMPARE(prop2.value(), QVariant("firstValue"));

    spy1.clear();
    spy2.clear();

    // Unsubscribe one of the properties, spin the event loop so that it really
    // happens
    prop1.unsubscribe();
    QTest::qWait(2000);

    // Both properties keep their values
    QCOMPARE(prop1.value(), QVariant("firstValue"));
    QCOMPARE(prop2.value(), QVariant("firstValue"));

    // Change the value on the provider side.

    // Both properties get the new value and the valueChanged signal.
    writeToProvider("Test.Prop=\"newValue\"\n");

    timer.restart();
    while (timer.elapsed() < 5000 && (spy1.count() == 0 || spy2.count() == 0)) {
        QTest::qWait(100);
    }

    QCOMPARE(spy1.count(), 1);
    QCOMPARE(spy2.count(), 1);

    QCOMPARE(prop1.value(), QVariant("newValue"));
    QCOMPARE(prop2.value(), QVariant("newValue"));

    spy1.clear();
    spy2.clear();

    // Subscribe prop1 and change the value back to firstValue -> also prop1
    // should get notified
    prop1.subscribe();
    prop1.waitForSubscription(true);

    writeToProvider("Test.Prop=\"firstValue\"\n");

    timer.restart();
    while (timer.elapsed() < 5000 && (spy1.count() == 0 || spy2.count() == 0)) {
        QTest::qWait(100);
    }

    QCOMPARE(spy1.count(), 1);
    QCOMPARE(spy2.count(), 1);

    QCOMPARE(prop1.value(), QVariant("firstValue"));
    QCOMPARE(prop2.value(), QVariant("firstValue"));
}

void RegressionTests::waitForSubscriptionWhenPluginReady()
{
    // Regression test for bug NB#242089
    QVERIFY(providerStarted);

    ContextProperty prop("Test.Prop");
    prop.waitForSubscription(true);
    QCOMPARE(prop.value(), QVariant("firstValue"));

    // unsubscribe and spin the event loop so that it really happens
    prop.unsubscribe();
    QTest::qWait(2000);

    // the value changes while we're unsubscribed
    writeToProvider("Test.Prop=\"newValue\"\n");
    QTest::qWait(2000);

    // our value doesn't change, since we're unsubscribed
    QCOMPARE(prop.value(), QVariant("firstValue"));

    // then we subscribe again, and do waitForSubscription
    prop.subscribe();
    prop.waitForSubscription(true);

    // We should really get the new value here.  The bug was that
    // waitForSubscription didn't really do anything.
    QCOMPARE(prop.value(), QVariant("newValue"));
}

QString RegressionTests::writeToProvider(const char* input)
{
    isReadyToRead = false;
    provider->write(input);
    provider->waitForBytesWritten();
    while (!isReadyToRead) {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    }
    // Return the output from the client
    return provider->readAllStandardOutput();
}

void RegressionTests::onProviderReadyRead()
{
    isReadyToRead = true;
}

QTEST_MAIN(RegressionTests);
