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
#include "context.h"
#include "manager.h"
#include "manageradaptor.h"

namespace ContextProvider {

Manager *firstManagerCreated = NULL;
QVariant *lastVariantSet = NULL;

void resetVariants()
{
    delete lastVariantSet;
    lastVariantSet = NULL;
}

bool Manager::keyIsValid(const QString &key) const
{
    return true;
}

void Manager::increaseSubscriptionCount(const QString &key)
{
}
 
void Manager::decreaseSubscriptionCount(const QString &key)
{
}

const QVariant Manager::keyValue(const QString &key)
{
    return QVariant();
}

void Manager::setKeyValue(const QString &key, const QVariant &v)
{
    delete lastVariantSet;
    lastVariantSet = new QVariant(v);
}

QStringList Manager::getKeys()
{
    return keys;
}

QVariant Manager::getKeyValue(const QString &key)
{
    if (lastVariantSet)
        return QVariant(*lastVariantSet);
    else
        return QVariant();
}

void Manager::fakeFirst(const QString &key)
{
    emit firstSubscriberAppeared(key);
}

void Manager::fakeLast(const QString &key)
{
    emit lastSubscriberDisappeared(key);
}

Manager::Manager(const QStringList &k) : keys(k)
{
    if (firstManagerCreated == NULL)
        firstManagerCreated = this;
}

ManagerAdaptor::ManagerAdaptor(Manager* manager, QDBusConnection *conn)
    : QDBusAbstractAdaptor(manager)
{
}

class ContextUnitTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void registerAgain();
    void registerSystem();
    void badServiceName();
    void stopService();
    void getProperty();
    void getNonExistentProperty();
    void checkSignals();
    void setGetBool();
    void setGetInt();
    void setGetDouble();
    void setGetString();
    void setGetQVariant();
    void unset();
    void getInvalid();
    void registerKeyAgain(); // has to be last since our global helper pointer becomes non-valid
};

// Before all tests
void ContextUnitTest::initTestCase()
{
    QStringList keys;
    keys.append("Battery.Voltage");
    QCOMPARE(Property::initService(QDBusConnection::SessionBus, "com.test.provider", keys), true);
}

void ContextUnitTest::registerAgain()
{
    QStringList keys;
    keys.append("Battery.Voltage");
    QCOMPARE(Property::initService(QDBusConnection::SessionBus, "com.test.provider", keys), false);
}

void ContextUnitTest::registerSystem()
{
    QStringList keys;
    keys.append("Battery.Power");
    QCOMPARE(Property::initService(QDBusConnection::SystemBus, "com.test.provider", keys), false);
}

void ContextUnitTest::badServiceName()
{
    QStringList keys;
    keys.append("Battery.Whatever");
    QCOMPARE(Property::initService(QDBusConnection::SessionBus, "!!!", keys), false);
}

void ContextUnitTest::stopService()
{
    Property::stopService("com.test.stopper"); // Bolox.

    QStringList keys;
    keys.append("Some.Key");
    QCOMPARE(Property::initService(QDBusConnection::SessionBus, "com.test.stopper", keys), true);
    QCOMPARE(Property::initService(QDBusConnection::SessionBus, "com.test.stopper", keys), false);

    Property::stopService("com.test.stopper");
    QCOMPARE(Property::initService(QDBusConnection::SessionBus, "com.test.stopper", keys), true);

    Property::stopService("com.test.stopper");
}

void ContextUnitTest::getProperty()
{
    Property c("Battery.Voltage");
    QCOMPARE(c.getKey(), QString("Battery.Voltage"));
    QCOMPARE(c.isValid(), true);
}
    
void ContextUnitTest::getNonExistentProperty()
{
    resetVariants();
    Property c("Battery.NonExistent");
    QCOMPARE(c.getKey(), QString("Battery.NonExistent"));
    QCOMPARE(c.isValid(), false);
}

void ContextUnitTest::checkSignals()
{
    resetVariants();
    Property c("Battery.Voltage");
   
    QSignalSpy spy1(&c, SIGNAL(firstSubscriberAppeared(QString)));
    QSignalSpy spy2(&c, SIGNAL(lastSubscriberDisappeared(QString)));

    firstManagerCreated->fakeFirst("Battery.Voltage");

    QCOMPARE(spy1.count(), 1);
    QList<QVariant> args1 = spy1.takeFirst();
    QCOMPARE(args1.at(0).toString(), QString("Battery.Voltage"));
    
    firstManagerCreated->fakeLast("Battery.Voltage");

    QCOMPARE(spy2.count(), 1);
    QList<QVariant> args2 = spy2.takeFirst();
    QCOMPARE(args2.at(0).toString(), QString("Battery.Voltage"));
}

void ContextUnitTest::setGetBool()
{
    resetVariants();
    Property("Battery.Voltage").set(true);
    QCOMPARE(*lastVariantSet, QVariant(true));
    QCOMPARE(Property("Battery.Voltage").get(), QVariant(true));

    // Set on invalid
    Property("Battery.NonExistent").set(false);
    QCOMPARE(*lastVariantSet, QVariant(true));
}

void ContextUnitTest::setGetInt()
{
    resetVariants();
    Property("Battery.Voltage").set(666);
    QCOMPARE(*lastVariantSet, QVariant(666));
    QCOMPARE(Property("Battery.Voltage").get(), QVariant(666));
    QVERIFY(Property("Battery.Voltage").isSet());
    
    // Set on invalid
    Property("Battery.NonExistent").set(999);
    QCOMPARE(*lastVariantSet, QVariant(666));
}

void ContextUnitTest::setGetDouble()
{
    resetVariants();
    Property("Battery.Voltage").set(0.456);
    QCOMPARE(*lastVariantSet, QVariant(0.456));
    QCOMPARE(Property("Battery.Voltage").get(), QVariant(0.456));
    QVERIFY(Property("Battery.Voltage").isSet());

    // Set on invalid
    Property("Battery.NonExistent").set(0.666);
    QCOMPARE(*lastVariantSet, QVariant(0.456));
}

void ContextUnitTest::setGetString()
{
    resetVariants();
    Property("Battery.Voltage").set(QString("Hello!"));
    QCOMPARE(*lastVariantSet, QVariant(QString("Hello!")));
    QCOMPARE(Property("Battery.Voltage").get(), QVariant("Hello!"));
    QVERIFY(Property("Battery.Voltage").isSet());
   
    // Set on invalid
    Property("Battery.NonExistent").set(QString("World"));
    QCOMPARE(*lastVariantSet, QVariant(QString("Hello!")));
}

void ContextUnitTest::setGetQVariant()
{
    resetVariants();
    Property("Battery.Voltage").set(QVariant(123));
    QCOMPARE(*lastVariantSet, QVariant(123));
    QCOMPARE(Property("Battery.Voltage").get(), QVariant(123));
    QVERIFY(Property("Battery.Voltage").isSet());
    
    // Set on invalid
    Property("Battery.NonExistent").set(QVariant("Hey!"));
    QCOMPARE(*lastVariantSet, QVariant(123));
}

void ContextUnitTest::unset()
{
    resetVariants();
    QVERIFY(Property("Battery.Voltage").isSet() == false);
    Property("Battery.Voltage").set(QVariant(123));
    QVERIFY(Property("Battery.Voltage").isSet());
    QCOMPARE(Property("Battery.Voltage").get(), QVariant(123));
    Property("Battery.Voltage").unset();
    QCOMPARE(Property("Battery.Voltage").get(), QVariant());
    QVERIFY(! Property("Battery.Voltage").isSet());

    // Unset invalid
    Property c("Battery.Invalid");
    QCOMPARE(c.isValid(), false);
    QCOMPARE(c.isSet(), false);
    c.unset();
    QCOMPARE(c.get(), QVariant());
}

void ContextUnitTest::getInvalid()
{
    resetVariants();
    QCOMPARE(Property("Battery.NonExistent").get(), QVariant());
}

void ContextUnitTest::registerKeyAgain()
{
    QStringList keys;
    keys.append("Battery.Voltage");
    QCOMPARE(Property::initService(QDBusConnection::SessionBus, "com.test.provider2", keys), false);
}

#include "contextunittest.moc"

} // end namespace

QTEST_MAIN(ContextProvider::ContextUnitTest);
