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

#include "contextgroup.h" // Object to be tested
#include "context.h"

#include <QObject>
#include <QSignalSpy>
#include <QTest>

namespace ContextProvider {

// Mock implementation of the Property class

QMap<QString, Property*> createdObjects;

Property::Property(Service &service, QString key, QObject* parent)
{
    // Store the created object
    createdObjects.insert(key, this);
}

Property::Property(QString key, QObject* parent)
{
    // Store the created object
    createdObjects.insert(key, this);
}

class ContextGroupUnitTest : public QObject
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
    Group* contextGroup;
    // Mock objects
    Property* property1;
    Property* property2;
};

// Before each test
void ContextGroupUnitTest::init()
{
    property1 = new Property("test.key.1");
    property2 = new Property("test.key.2");
    createdObjects.clear();
}

// After each test
void ContextGroupUnitTest::cleanup()
{
    delete contextGroup;
    contextGroup = 0;
    delete property1;
    property1 = 0;
    delete property2;
    property2 = 0;
}

void ContextGroupUnitTest::oneProperty()
{
    // Create the object to be tested
    QSet<Property*> propertySet;
    propertySet.insert(property1);
    contextGroup = new Group(propertySet);
    // Start spying on signals
    QSignalSpy firstSpy(contextGroup, SIGNAL(firstSubscriberAppeared()));
    QSignalSpy lastSpy(contextGroup, SIGNAL(lastSubscriberDisappeared()));

    // Test: check the initial subscription status
    // Expected result: the group is not subscribed to
    QCOMPARE(contextGroup->isSubscribedTo(), false);

    // Test: property is subscribed to
    emit property1->firstSubscriberAppeared("test.key.1");

    // Expected result: the Group emits the firstSubscriberAppeared signal
    QCOMPARE(firstSpy.count(), 1);
    QCOMPARE(lastSpy.count(), 0);
    // And the group is subscribed to
    QCOMPARE(contextGroup->isSubscribedTo(), true);

    // Test: property is unsubscribed from
    emit property1->lastSubscriberDisappeared("test.key.1");

    // Expected result: the Group emits the lastSubscriberDisappeared signal
    QCOMPARE(firstSpy.count(), 1);
    QCOMPARE(lastSpy.count(), 1);
    // And the group is not subscribed to
    QCOMPARE(contextGroup->isSubscribedTo(), false);
}

void ContextGroupUnitTest::twoProperties()
{
    // Create the object to be tested
    QSet<Property*> propertySet;
    propertySet.insert(property1);
    propertySet.insert(property2);
    contextGroup = new Group(propertySet);
    // Start spying on signals
    QSignalSpy firstSpy(contextGroup, SIGNAL(firstSubscriberAppeared()));
    QSignalSpy lastSpy(contextGroup, SIGNAL(lastSubscriberDisappeared()));

    // Test: property 1 is subscribed to
    emit property1->firstSubscriberAppeared("test.key.1");

    // Expected result: the Group emits the firstSubscriberAppeared signal
    QCOMPARE(firstSpy.count(), 1);
    QCOMPARE(lastSpy.count(), 0);
    // And the group is subscribed to
    QCOMPARE(contextGroup->isSubscribedTo(), true);

    // Test: property 2 is subscribed to
    emit property2->firstSubscriberAppeared("test.key.2");

    // Expected result: the Group doesn't emit anything
    QCOMPARE(firstSpy.count(), 1);
    QCOMPARE(lastSpy.count(), 0);
    // And the group is still subscribed to
    QCOMPARE(contextGroup->isSubscribedTo(), true);

    // Test: property 1 is unsubscribed from
    emit property1->lastSubscriberDisappeared("test.key.1");

    // Expected result: the Group doesn't emit anything
    QCOMPARE(firstSpy.count(), 1);
    QCOMPARE(lastSpy.count(), 0);
    // And the group is still subscribed to
    QCOMPARE(contextGroup->isSubscribedTo(), true);

    // Test: property 2 is unsubscribed from
    emit property2->lastSubscriberDisappeared("test.key.2");

    // Expected result: the Group emits the lastSubscriberDisappeared signal
    QCOMPARE(firstSpy.count(), 1);
    QCOMPARE(lastSpy.count(), 1);
    // And the group is not subscribed to
    QCOMPARE(contextGroup->isSubscribedTo(), false);
}

void ContextGroupUnitTest::stringListApiOneProperty()
{
    Service service;

    // Create the object to be tested
    QStringList list;
    list << "first.key";
    contextGroup = new Group(service, list);
    // Start spying on signals
    QSignalSpy firstSpy(contextGroup, SIGNAL(firstSubscriberAppeared()));
    QSignalSpy lastSpy(contextGroup, SIGNAL(lastSubscriberDisappeared()));

    // Test: check that Group created the Property objects
    // Expected result: the Group created the Property object with the correct name
    QVERIFY(createdObjects.contains("first.key"));
    QVERIFY(createdObjects.size() == 1);

    // Test: check the initial subscription status
    // Expected result: the group is not subscribed to
    QCOMPARE(contextGroup->isSubscribedTo(), false);

    // Test: property is subscribed to
    Property* propertyFirst = createdObjects.value("first.key");
    emit propertyFirst->firstSubscriberAppeared("test.key.1");

    // Expected result: the Group emits the firstSubscriberAppeared signal
    QCOMPARE(firstSpy.count(), 1);
    QCOMPARE(lastSpy.count(), 0);
    // And the group is subscribed to
    QCOMPARE(contextGroup->isSubscribedTo(), true);

    // Test: property is unsubscribed from
    emit propertyFirst->lastSubscriberDisappeared("test.key.1");

    // Expected result: the Group emits the lastSubscriberDisappeared signal
    QCOMPARE(firstSpy.count(), 1);
    QCOMPARE(lastSpy.count(), 1);
    // And the group is not subscribed to
    QCOMPARE(contextGroup->isSubscribedTo(), false);
}

void ContextGroupUnitTest::stringListApiTwoProperties()
{
    Service service;

    // Create the object to be tested
    QStringList list;
    list << "first.key" << "second.key";
    contextGroup = new Group(service,list);
    // Start spying on signals
    QSignalSpy firstSpy(contextGroup, SIGNAL(firstSubscriberAppeared()));
    QSignalSpy lastSpy(contextGroup, SIGNAL(lastSubscriberDisappeared()));

    // Test: check that Group created the Property objects
    // Expected result: the Group created the Property object with the correct name
    QVERIFY(createdObjects.contains("first.key"));
    QVERIFY(createdObjects.contains("second.key"));
    QVERIFY(createdObjects.size() == 2);

    // Test: property 1 is subscribed to
    Property* propertyFirst = createdObjects.value("first.key");
    emit propertyFirst->firstSubscriberAppeared("first.key");

    // Expected result: the Group emits the firstSubscriberAppeared signal
    QCOMPARE(firstSpy.count(), 1);
    QCOMPARE(lastSpy.count(), 0);
    // And the group is subscribed to
    QCOMPARE(contextGroup->isSubscribedTo(), true);

    // Test: property 2 is subscribed to
    Property* propertySecond = createdObjects.value("second.key");
    emit propertySecond->firstSubscriberAppeared("second.key");

    // Expected result: the Group doesn't emit anything
    QCOMPARE(firstSpy.count(), 1);
    QCOMPARE(lastSpy.count(), 0);
    // And the group is still subscribed to
    QCOMPARE(contextGroup->isSubscribedTo(), true);

    // Test: property 1 is unsubscribed from
    emit propertyFirst->lastSubscriberDisappeared("first.key");

    // Expected result: the Group doesn't emit anything
    QCOMPARE(firstSpy.count(), 1);
    QCOMPARE(lastSpy.count(), 0);
    // And the group is still subscribed to
    QCOMPARE(contextGroup->isSubscribedTo(), true);

    // Test: property 2 is unsubscribed from
    emit propertySecond->lastSubscriberDisappeared("second.key");

    // Expected result: the Group emits the lastSubscriberDisappeared signal
    QCOMPARE(firstSpy.count(), 1);
    QCOMPARE(lastSpy.count(), 1);
    // And the group is not subscribed to
    QCOMPARE(contextGroup->isSubscribedTo(), false);
}

#include "contextgroupunittest.moc"

} // end namespace

QTEST_MAIN(ContextProvider::ContextGroupUnitTest);
