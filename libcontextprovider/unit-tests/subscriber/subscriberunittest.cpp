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
#include "queuedinvoker.h"
#include "logging.h"

/* Queued invoker mock implementation */

namespace ContextProvider {

QueuedInvoker::QueuedInvoker()
{
}

void QueuedInvoker::queueOnce(const char *method)
{
    contextDebug() << "queueonce" << QString(method);
    if (!methodsToCall.contains(QString(method))) {
        methodsToCall.push_back(method);
    }
}

void QueuedInvoker::callAllMethodsInQueue()
{
    while (!methodsToCall.empty()) {
        QString method = methodsToCall.front();
        methodsToCall.pop_front();
        if (!QMetaObject::invokeMethod(this, method.toStdString().c_str(), Qt::DirectConnection)) {
            qFatal("    *****************\n"
                   "Erroneous usage of queueOnce\n"
                   "    *****************\n");
        }
    }
}

/* Manager mock implementation */

bool Manager::keyIsValid(const QString &key) const
{
    if (key == "Battery.OnBattery" || key == "Battery.Voltage" || key == "Battery.OverCharged")
        return true;
    else
        return false;
}

void Manager::increaseSubscriptionCount(const QString &key)
{
    increaseCalls.append(key);
}
 
void Manager::decreaseSubscriptionCount(const QString &key)
{
    decreaseCalls.append(key);
}

QVariant Manager::getKeyValue(const QString &key) const
{
    if (key == "Battery.OverCharged")
        return QVariant(true);
    else
        return QVariant();
}

void Manager::emitKeyValueChanged(const QString &key, const QVariant& newValue)
{
    emit keyValueChanged(key, newValue);
}

} // namespace ContextProvider

/* SubscriberUnitTest */

using namespace ContextProvider;

class SubscriberUnitTest : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();
    void path();
    void subscribeAndUnsubscribe();
    void subscribeToInvalid();
    void subscribeTwice();
    void unsubscribeFromInvalid();
    void unsubscribeTwice();
    void keyValueChangeUnsubscribed();
    void keyValueChangeSubscribed();

private:
    Subscriber *subscriber;
    Manager *manager;
};

// Before each test
void SubscriberUnitTest::init()
{
    manager = new Manager();
    subscriber = new Subscriber("/some/path", manager);
}

// After each test
void SubscriberUnitTest::cleanup()
{
    delete subscriber;
    delete manager; 
}

void SubscriberUnitTest::path()
{
    QCOMPARE(subscriber->dbusPath(), QString("/some/path"));
}

void SubscriberUnitTest::subscribeAndUnsubscribe()
{
    QStringList keys1;
    keys1.append("Battery.OnBattery");
    keys1.append("Battery.Voltage");
    QStringList undetermined;

    QMap<QString, QVariant> map = subscriber->subscribe(keys1, undetermined);
    QCOMPARE(undetermined.size(), 2);
    QVERIFY(subscriber->isSubscribedToKey("Battery.OnBattery"));
    QVERIFY(subscriber->isSubscribedToKey("Battery.Voltage"));
    
    QCOMPARE(manager->increaseCalls.count("Battery.OnBattery"), 1);
    QCOMPARE(manager->increaseCalls.count("Battery.Voltage"), 1);
    QCOMPARE(manager->decreaseCalls.count("Battery.OnBattery"), 0);
    QCOMPARE(manager->decreaseCalls.count("Battery.Voltage"), 0);

    QStringList keys2;
    keys2.append("Battery.Voltage");
    subscriber->unsubscribe(keys2);
    QVERIFY(subscriber->isSubscribedToKey("Battery.OnBattery"));
    QVERIFY(subscriber->isSubscribedToKey("Battery.Voltage") == false);
    
    QCOMPARE(manager->decreaseCalls.count("Battery.OnBattery"), 0);
    QCOMPARE(manager->decreaseCalls.count("Battery.Voltage"), 1);
    QCOMPARE(manager->increaseCalls.count("Battery.OnBattery"), 1);
    QCOMPARE(manager->increaseCalls.count("Battery.Voltage"), 1);
}

void SubscriberUnitTest::subscribeToInvalid()
{
    QStringList keys1;
    keys1.append("Battery.OnBattery");
    keys1.append("Battery.Voltage");
    keys1.append("Battery.NonExistant");
    QStringList undetermined;

    QMap<QString, QVariant> map = subscriber->subscribe(keys1, undetermined);
    QCOMPARE(undetermined.size(), 3);
    QVERIFY(subscriber->isSubscribedToKey("Battery.OnBattery"));
    QVERIFY(subscriber->isSubscribedToKey("Battery.Voltage"));
    QVERIFY(subscriber->isSubscribedToKey("Battery.NonExistant") == false);

    QCOMPARE(manager->increaseCalls.count("Battery.OnBattery"), 1);
    QCOMPARE(manager->increaseCalls.count("Battery.Voltage"), 1);
    QCOMPARE(manager->increaseCalls.count("Battery.NonExistant"), 0);
    QCOMPARE(manager->decreaseCalls.count("Battery.OnBattery"), 0);
    QCOMPARE(manager->decreaseCalls.count("Battery.Voltage"), 0);
    QCOMPARE(manager->decreaseCalls.count("Battery.NonExistant"), 0);
}

void SubscriberUnitTest::subscribeTwice()
{
    QStringList keys1;
    keys1.append("Battery.OnBattery");
    keys1.append("Battery.OnBattery");
    QStringList undetermined;

    QMap<QString, QVariant> map1 = subscriber->subscribe(keys1, undetermined);
    QCOMPARE(undetermined.size(), 1);
    QVERIFY(subscriber->isSubscribedToKey("Battery.OnBattery"));

    QCOMPARE(manager->increaseCalls.count("Battery.OnBattery"), 1);
    QCOMPARE(manager->decreaseCalls.count("Battery.OnBattery"), 0);

    // Try again...
    QStringList keys2;
    keys2.append("Battery.OnBattery");

    QMap<QString, QVariant> map2 = subscriber->subscribe(keys2, undetermined);
    QCOMPARE(undetermined.size(), 1);
    QVERIFY(subscriber->isSubscribedToKey("Battery.OnBattery"));

    QCOMPARE(manager->increaseCalls.count("Battery.OnBattery"), 1);
    QCOMPARE(manager->decreaseCalls.count("Battery.OnBattery"), 0);
}

void SubscriberUnitTest::unsubscribeFromInvalid()
{
    QStringList keys1;
    keys1.append("Battery.OnBattery");
    QStringList undetermined;

    subscriber->subscribe(keys1, undetermined);
    QVERIFY(subscriber->isSubscribedToKey("Battery.OnBattery"));

    QCOMPARE(manager->increaseCalls.count("Battery.OnBattery"), 1);
    QCOMPARE(manager->decreaseCalls.count("Battery.OnBattery"), 0);

    QStringList keys2;
    keys2.append("Battery.Invalid");
    subscriber->unsubscribe(keys2);
    QVERIFY(subscriber->isSubscribedToKey("Battery.OnBattery") == true);
    QVERIFY(subscriber->isSubscribedToKey("Battery.Invalid") == false);

    QCOMPARE(manager->increaseCalls.count("Battery.OnBattery"), 1);
    QCOMPARE(manager->decreaseCalls.count("Battery.OnBattery"), 0);
    QCOMPARE(manager->increaseCalls.count("Battery.Invalid"), 0);
    QCOMPARE(manager->decreaseCalls.count("Battery.Invalid"), 0);
}

void SubscriberUnitTest::unsubscribeTwice()
{
    QStringList keys1;
    keys1.append("Battery.OnBattery");
    QStringList undetermined;

    subscriber->subscribe(keys1, undetermined);
    QVERIFY(subscriber->isSubscribedToKey("Battery.OnBattery"));

    QCOMPARE(manager->increaseCalls.count("Battery.OnBattery"), 1);
    QCOMPARE(manager->decreaseCalls.count("Battery.OnBattery"), 0);

    subscriber->unsubscribe(keys1);
    QVERIFY(subscriber->isSubscribedToKey("Battery.OnBattery") == false);

    QCOMPARE(manager->increaseCalls.count("Battery.OnBattery"), 1);
    QCOMPARE(manager->decreaseCalls.count("Battery.OnBattery"), 1);

    subscriber->unsubscribe(keys1);
    QVERIFY(subscriber->isSubscribedToKey("Battery.OnBattery") == false);

    QCOMPARE(manager->increaseCalls.count("Battery.OnBattery"), 1);
    QCOMPARE(manager->decreaseCalls.count("Battery.OnBattery"), 1);
}

void SubscriberUnitTest::keyValueChangeUnsubscribed()
{
    QSignalSpy spy(subscriber, SIGNAL(changed(QMap<QString, QVariant>, QStringList)));
    manager->emitKeyValueChanged("Battery.Something", QVariant(55));
    subscriber->callAllMethodsInQueue();
    QCOMPARE(spy.count(), 0);
}

void SubscriberUnitTest::keyValueChangeSubscribed()
{   
    QStringList keys;
    keys.append("Battery.OnBattery");
    keys.append("Battery.OverCharged");
    keys.append("Battery.Voltage");
    QStringList undetermined;
    subscriber->subscribe(keys, undetermined);
    
    QSignalSpy spy(subscriber, SIGNAL(changed(QMap<QString, QVariant>, QStringList)));
    manager->emitKeyValueChanged("Battery.OverCharged", QVariant(true));
    manager->emitKeyValueChanged("Battery.OnBattery", QVariant());
    subscriber->callAllMethodsInQueue();
    QCOMPARE(spy.count(), 1);

    QList<QVariant> args = spy.takeFirst();

    QMap<QString, QVariant> map = args.at(0).toMap();
    QCOMPARE(map.size(), 1);
    QCOMPARE(map.value("Battery.OverCharged"), QVariant(true));

    QCOMPARE(args.at(1).toList().count(), 1);
    QCOMPARE(args.at(1).toList().at(0).toString(), QString("Battery.OnBattery"));
}

#include "subscriberunittest.moc"
QTEST_MAIN(SubscriberUnitTest);
