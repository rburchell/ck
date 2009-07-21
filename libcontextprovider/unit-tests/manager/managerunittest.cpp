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

#include <QtTest/QtTest>
#include <QtCore>
#include <stdlib.h>
#include "manager.h"
#include "subscriber.h"

using namespace ContextProvider;

class ManagerUnitTest : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();
    void keys();
    void duplicateKeys();
    void values();
    void subscriptionCount();
    void subscriptionCountNonExistent();
    void getSubscriber();
    void getKeys();
    void busNameIsGone();
    void subscribeAndUnsubscribe();
    void setKeyValue();
    void getKeyValue();
    void getKeyValueInvalid();
    void setKeyValueSame();
    void setKeyValueNonExistent();

private:
    Manager *manager;
};

// Before each test
void ManagerUnitTest::init()
{
    QStringList keys;
    keys.append("Battery.Power");
    keys.append("Battery.Voltage");
    keys.append("Battery.AboutToExplode");

    manager = new Manager(keys);
}

// After each test
void ManagerUnitTest::cleanup()
{
    delete manager;
}

void ManagerUnitTest::keys()
{
    QVERIFY(manager->keyIsValid("Battery.Power"));
    QVERIFY(manager->keyIsValid("Battery.Voltage"));
    QVERIFY(manager->keyIsValid("Battery.AboutToExplode"));
    QVERIFY(manager->keyIsValid("Battery.Something") == false);
}

void ManagerUnitTest::getKeys()
{
    QStringList keys = manager->getKeys();
    QCOMPARE(keys.length(), 3);
    QVERIFY(keys.contains("Battery.Power"));
    QVERIFY(keys.contains("Battery.Voltage"));
    QVERIFY(keys.contains("Battery.AboutToExplode"));
}

void ManagerUnitTest::duplicateKeys()
{    
    QStringList keys;
    keys.append("Battery.Voltage");
    keys.append("Battery.Voltage");
    keys.append("Battery.Power");

    Manager *m = new Manager(keys);

    QVERIFY(m->keyIsValid("Battery.Power"));
    QVERIFY(m->keyIsValid("Battery.Voltage"));
    
    delete m;
}

void ManagerUnitTest::values()
{
    QVERIFY(manager->getKeyValue("Battery.Power").isValid() == FALSE);
    QVERIFY(manager->getKeyValue("Battery.Voltage").isValid() == FALSE);
    QVERIFY(manager->getKeyValue("Battery.AboutToExplode").isValid() == FALSE);
    QVERIFY(manager->getKeyValue("Battery.Something").isValid() == FALSE);
}

void ManagerUnitTest::subscriptionCount()
{
    QCOMPARE(manager->getSubscriptionCount("Battery.Power"), 0);
    QCOMPARE(manager->getSubscriptionCount("Battery.Voltage"), 0);
    QCOMPARE(manager->getSubscriptionCount("Battery.AboutToExplode"), 0);

    QSignalSpy spy1(manager, SIGNAL(firstSubscriberAppeared(QString)));
    QSignalSpy spy2(manager, SIGNAL(lastSubscriberDisappeared(QString)));

    manager->increaseSubscriptionCount("Battery.Voltage");
    QCOMPARE(manager->getSubscriptionCount("Battery.Voltage"), 1);
    QCOMPARE(spy1.count(), 1);
    QList<QVariant> args1 = spy1.takeFirst();
    QCOMPARE(args1.at(0).toString(), QString("Battery.Voltage"));

    manager->increaseSubscriptionCount("Battery.Voltage");
    QCOMPARE(manager->getSubscriptionCount("Battery.Voltage"), 2);
    QCOMPARE(spy1.count(), 0);

    manager->decreaseSubscriptionCount("Battery.Voltage");
    QCOMPARE(manager->getSubscriptionCount("Battery.Voltage"), 1);
    QCOMPARE(spy2.count(), 0);

    manager->decreaseSubscriptionCount("Battery.Voltage");
    QCOMPARE(manager->getSubscriptionCount("Battery.Voltage"), 0);
    QCOMPARE(spy2.count(), 1);
    QList<QVariant> args2 = spy2.takeFirst();
    QCOMPARE(args2.at(0).toString(), QString("Battery.Voltage"));

    // Check if following decrease does not trigger a fire
    manager->decreaseSubscriptionCount("Battery.Voltage");
    QCOMPARE(manager->getSubscriptionCount("Battery.Voltage"), 0);
    QCOMPARE(spy2.count(), 0);
}

void ManagerUnitTest::subscriptionCountNonExistent()
{
    QCOMPARE(manager->getSubscriptionCount("Battery.Power"), 0);
    QCOMPARE(manager->getSubscriptionCount("Battery.Voltage"), 0);
    QCOMPARE(manager->getSubscriptionCount("Battery.NonExistent"), -1);

    QSignalSpy spy1(manager, SIGNAL(firstSubscriberAppeared(QString)));
    QSignalSpy spy2(manager, SIGNAL(lastSubscriberDisappeared(QString)));

    manager->increaseSubscriptionCount("Battery.NonExistent");
    QCOMPARE(manager->getSubscriptionCount("Battery.NonExistent"), -1);
    QCOMPARE(spy1.count(), 0);

    manager->decreaseSubscriptionCount("Battery.NonExistent");
    QCOMPARE(manager->getSubscriptionCount("Battery.NonExistent"), -1);
    QCOMPARE(spy2.count(), 0);
}

void ManagerUnitTest::getSubscriber()
{
    Subscriber *s1 = manager->getSubscriber("some:name");
    QVERIFY(s1 != NULL);

    Subscriber *s2 = manager->getSubscriber("some:name");
    QVERIFY(s2 != NULL);
    QVERIFY(s1 == s2);
    QCOMPARE(manager->hasSubscriberWithBusName("some:name"), true);
}

void ManagerUnitTest::busNameIsGone()
{
    Subscriber *s1 = manager->getSubscriber("some:name");
    QVERIFY(s1 != NULL);

    QCOMPARE(manager->hasSubscriberWithBusName("some:name"), true);
    manager->busNameIsGone("some:name");
    QCOMPARE(manager->hasSubscriberWithBusName("some:name"), false);
}

void ManagerUnitTest::subscribeAndUnsubscribe()
{
    Subscriber *s = manager->getSubscriber("some:name");
    QVERIFY(s != NULL);
   
    QStringList keys;
    QStringList undetermined;
    keys.append("Battery.Voltage");

    QMap <QString, QVariant> values = s->subscribe(keys, undetermined);
    QCOMPARE(values.size() + undetermined.size(), keys.size());

    QCOMPARE(manager->getSubscriptionCount("Battery.Voltage"), 1);

    s->unsubscribe(keys);
    
    QCOMPARE(manager->getSubscriptionCount("Battery.Voltage"), 0);
    QCOMPARE(manager->hasSubscriberWithBusName("some:name"), true);
}

void ManagerUnitTest::setKeyValue()
{
    QSignalSpy spy(manager, SIGNAL(keyValueChanged(const QString&, const QVariant&)));
    manager->setKeyValue("Battery.Power", QVariant(100));
    QCOMPARE(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();
    QCOMPARE(args.at(0).toString(), QString("Battery.Power"));
    // FIXME FIXME FIXME
    //QCOMPARE(args.at(1).toInt(), 100);
}

void ManagerUnitTest::getKeyValue()
{
    QCOMPARE(manager->getKeyValue("Battery.Power"), QVariant());
    manager->setKeyValue("Battery.Power", QVariant(86));
    QCOMPARE(manager->getKeyValue("Battery.Power"), QVariant(86));
}

void ManagerUnitTest::getKeyValueInvalid()
{
    QCOMPARE(manager->getKeyValue("Battery.Something"), QVariant());
    manager->setKeyValue("Battery.Something", QVariant(86));
    QCOMPARE(manager->getKeyValue("Battery.Something"), QVariant());
}

void ManagerUnitTest::setKeyValueSame()
{
    QSignalSpy spy(manager, SIGNAL(keyValueChanged(const QString&, const QVariant&)));
    manager->setKeyValue("Battery.Power", QVariant(100));
    manager->setKeyValue("Battery.Power", QVariant(100));
    QCOMPARE(spy.count(), 1);
}

void ManagerUnitTest::setKeyValueNonExistent()
{
    QSignalSpy spy(manager, SIGNAL(keyValueChanged(const QString&, const QVariant&)));
    manager->setKeyValue("Battery.NonExistent", QVariant(100));
    QCOMPARE(spy.count(), 0);
}

#include "managerunittest.moc"
QTEST_MAIN(ManagerUnitTest);
