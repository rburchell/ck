/*
 * Copyright (C) 2008, 2009 Nokia Corporation.
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

#include "signalgrouper.h" // Object to be tested
#include "context.h"

#include <QObject>
#include <QSignalSpy>
#include <QTest>


// Mock implementation of the Context class

QMap<QString, Context*> createdObjects;

Context::Context(QString key, QObject* parent)
{
    // Store the created object
    createdObjects.insert(key, this);
}

class SignalGrouperUnitTest : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    void oneProperty();
    void twoProperties();
    void stringListApiOneProperty();
    void stringListApiTwoProperties();

private:
    // Object to be tested
    SignalGrouper* signalGrouper;
    // Mock objects
    Context* property1;
    Context* property2;
};

// Before each test
void SignalGrouperUnitTest::init()
{
    property1 = new Context("test.key.1");
    property2 = new Context("test.key.2");
    createdObjects.clear();
}

// After each test
void SignalGrouperUnitTest::cleanup()
{
    delete signalGrouper;
    signalGrouper = 0;
    delete property1;
    property1 = 0;
    delete property2;
    property2 = 0;
}

void SignalGrouperUnitTest::oneProperty()
{
    // Create the object to be tested
    QSet<Context*> propertySet;
    propertySet.insert(property1);
    signalGrouper = new SignalGrouper(propertySet);
    // Start spying on signals
    QSignalSpy firstSpy(signalGrouper, SIGNAL(firstSubscriberAppeared()));
    QSignalSpy lastSpy(signalGrouper, SIGNAL(lastSubscriberDisappeared()));

    // Test: check the initial subscription status
    // Expected result: the group is not subscribed to
    QCOMPARE(signalGrouper->isSubscribedTo(), false);

    // Test: property is subscribed to
    emit property1->firstSubscriberAppeared("test.key.1");

    // Expected result: the SignalGrouper emits the firstSubscriberAppeared signal
    QCOMPARE(firstSpy.count(), 1);
    QCOMPARE(lastSpy.count(), 0);
    // And the group is subscribed to
    QCOMPARE(signalGrouper->isSubscribedTo(), true);

    // Test: property is unsubscribed from
    emit property1->lastSubscriberDisappeared("test.key.1");

    // Expected result: the SignalGrouper emits the lastSubscriberDisappeared signal
    QCOMPARE(firstSpy.count(), 1);
    QCOMPARE(lastSpy.count(), 1);
    // And the group is not subscribed to
    QCOMPARE(signalGrouper->isSubscribedTo(), false);
}

void SignalGrouperUnitTest::twoProperties()
{
    // Create the object to be tested
    QSet<Context*> propertySet;
    propertySet.insert(property1);
    propertySet.insert(property2);
    signalGrouper = new SignalGrouper(propertySet);
    // Start spying on signals
    QSignalSpy firstSpy(signalGrouper, SIGNAL(firstSubscriberAppeared()));
    QSignalSpy lastSpy(signalGrouper, SIGNAL(lastSubscriberDisappeared()));

    // Test: property 1 is subscribed to
    emit property1->firstSubscriberAppeared("test.key.1");

    // Expected result: the SignalGrouper emits the firstSubscriberAppeared signal
    QCOMPARE(firstSpy.count(), 1);
    QCOMPARE(lastSpy.count(), 0);
    // And the group is subscribed to
    QCOMPARE(signalGrouper->isSubscribedTo(), true);

    // Test: property 2 is subscribed to
    emit property2->firstSubscriberAppeared("test.key.2");

    // Expected result: the SignalGrouper doesn't emit anything
    QCOMPARE(firstSpy.count(), 1);
    QCOMPARE(lastSpy.count(), 0);
    // And the group is still subscribed to
    QCOMPARE(signalGrouper->isSubscribedTo(), true);

    // Test: property 1 is unsubscribed from
    emit property1->lastSubscriberDisappeared("test.key.1");

    // Expected result: the SignalGrouper doesn't emit anything
    QCOMPARE(firstSpy.count(), 1);
    QCOMPARE(lastSpy.count(), 0);
    // And the group is still subscribed to
    QCOMPARE(signalGrouper->isSubscribedTo(), true);

    // Test: property 2 is unsubscribed from
    emit property2->lastSubscriberDisappeared("test.key.2");

    // Expected result: the SignalGrouper emits the lastSubscriberDisappeared signal
    QCOMPARE(firstSpy.count(), 1);
    QCOMPARE(lastSpy.count(), 1);
    // And the group is not subscribed to
    QCOMPARE(signalGrouper->isSubscribedTo(), false);
}

void SignalGrouperUnitTest::stringListApiOneProperty()
{
    // Create the object to be tested
    QStringList list;
    list << "first.key";
    signalGrouper = new SignalGrouper(list);
    // Start spying on signals
    QSignalSpy firstSpy(signalGrouper, SIGNAL(firstSubscriberAppeared()));
    QSignalSpy lastSpy(signalGrouper, SIGNAL(lastSubscriberDisappeared()));

    // Test: check that SignalGrouper created the Context objects
    // Expected result: the SignalGrouper created the Context object with the correct name
    QVERIFY(createdObjects.contains("first.key"));
    QVERIFY(createdObjects.size() == 1);

    // Test: check the initial subscription status
    // Expected result: the group is not subscribed to
    QCOMPARE(signalGrouper->isSubscribedTo(), false);

    // Test: property is subscribed to
    Context* propertyFirst = createdObjects.value("first.key");
    emit propertyFirst->firstSubscriberAppeared("test.key.1");

    // Expected result: the SignalGrouper emits the firstSubscriberAppeared signal
    QCOMPARE(firstSpy.count(), 1);
    QCOMPARE(lastSpy.count(), 0);
    // And the group is subscribed to
    QCOMPARE(signalGrouper->isSubscribedTo(), true);

    // Test: property is unsubscribed from
    emit propertyFirst->lastSubscriberDisappeared("test.key.1");

    // Expected result: the SignalGrouper emits the lastSubscriberDisappeared signal
    QCOMPARE(firstSpy.count(), 1);
    QCOMPARE(lastSpy.count(), 1);
    // And the group is not subscribed to
    QCOMPARE(signalGrouper->isSubscribedTo(), false);
}

void SignalGrouperUnitTest::stringListApiTwoProperties()
{
    // Create the object to be tested
    QStringList list;
    list << "first.key" << "second.key";
    signalGrouper = new SignalGrouper(list);
    // Start spying on signals
    QSignalSpy firstSpy(signalGrouper, SIGNAL(firstSubscriberAppeared()));
    QSignalSpy lastSpy(signalGrouper, SIGNAL(lastSubscriberDisappeared()));

    // Test: check that SignalGrouper created the Context objects
    // Expected result: the SignalGrouper created the Context object with the correct name
    QVERIFY(createdObjects.contains("first.key"));
    QVERIFY(createdObjects.contains("second.key"));
    QVERIFY(createdObjects.size() == 2);

    // Test: property 1 is subscribed to
    Context* propertyFirst = createdObjects.value("first.key");
    emit propertyFirst->firstSubscriberAppeared("first.key");

    // Expected result: the SignalGrouper emits the firstSubscriberAppeared signal
    QCOMPARE(firstSpy.count(), 1);
    QCOMPARE(lastSpy.count(), 0);
    // And the group is subscribed to
    QCOMPARE(signalGrouper->isSubscribedTo(), true);

    // Test: property 2 is subscribed to
    Context* propertySecond = createdObjects.value("second.key");
    emit propertySecond->firstSubscriberAppeared("second.key");

    // Expected result: the SignalGrouper doesn't emit anything
    QCOMPARE(firstSpy.count(), 1);
    QCOMPARE(lastSpy.count(), 0);
    // And the group is still subscribed to
    QCOMPARE(signalGrouper->isSubscribedTo(), true);

    // Test: property 1 is unsubscribed from
    emit propertyFirst->lastSubscriberDisappeared("first.key");

    // Expected result: the SignalGrouper doesn't emit anything
    QCOMPARE(firstSpy.count(), 1);
    QCOMPARE(lastSpy.count(), 0);
    // And the group is still subscribed to
    QCOMPARE(signalGrouper->isSubscribedTo(), true);

    // Test: property 2 is unsubscribed from
    emit propertySecond->lastSubscriberDisappeared("second.key");

    // Expected result: the SignalGrouper emits the lastSubscriberDisappeared signal
    QCOMPARE(firstSpy.count(), 1);
    QCOMPARE(lastSpy.count(), 1);
    // And the group is not subscribed to
    QCOMPARE(signalGrouper->isSubscribedTo(), false);
}


#include "signalgrouperunittest.moc"
QTEST_MAIN(SignalGrouperUnitTest);
