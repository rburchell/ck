/*
 * Copyright (C) 2008 Nokia Corporation.
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

// Header file of the tests
#include "testpropertyprovider.h"

// Mock header files
#include "dbusnamelistener.h"
#include "handlesignalrouter.h"
#include "subscriberinterface.h"
#include "managerinterface.h"

// Header file of the class to be tested
#include "propertyprovider.h"

#include <QtTest/QtTest>
#include <QDebug>
#include <QDBusConnection>

Q_DECLARE_METATYPE(QVariant);
Q_DECLARE_METATYPE(QSet<QString>);

#define MYLOGLEVEL 2
void myMessageOutput(QtMsgType type, const char *msg)
{
    switch (type) {
    case QtDebugMsg:
        if (MYLOGLEVEL <= 0)
            fprintf(stderr, "Debug: %s\n", msg);
        break;
    case QtWarningMsg:
        if (MYLOGLEVEL <= 1)
            fprintf(stderr, "Warning: %s\n", msg);
        break;
    case QtCriticalMsg:
        if (MYLOGLEVEL <= 2)
            fprintf(stderr, "Critical: %s\n", msg);
        break;
    case QtFatalMsg:
        if (MYLOGLEVEL <= 3)
            fprintf(stderr, "Fatal: %s\n", msg);
        abort();
    }
}

// Mock instances
// These will be created by the test program
DBusNameListener* mockDBusNameListener;
HandleSignalRouter* mockHandleSignalRouter;
// These will be created by the tested class and stored here
ManagerInterface* mockManagerInterface;
SubscriberInterface* mockSubscriberInterface;

// Mock implementation of the ManagerInterface

int ManagerInterface::getSubscriberCount = 0;
int ManagerInterface::creationCount = 0;
QList<QDBusConnection::BusType> ManagerInterface::creationBusTypes;
QStringList ManagerInterface::creationBusNames;

ManagerInterface::ManagerInterface(const QDBusConnection::BusType busType, const QString &busName, QObject *parent)
    : getSubscriberFailed(false)
{
    // Store the mock implementation (created by the tested class)
    mockManagerInterface = this;

    qDebug() << "Creating mock ManagerInterface";
    // Log the event: Manager interface created
    ++ creationCount;
    // Log the parameters
    creationBusTypes << busType;
    creationBusNames << busName;
}


void ManagerInterface::getSubscriber()
{
    ++ getSubscriberCount;
}

bool ManagerInterface::isGetSubscriberFailed() const
{
    return getSubscriberFailed;
}

void ManagerInterface::resetLogs()
{
    creationCount = 0;
    creationBusTypes.clear();
    creationBusNames.clear();
    getSubscriberCount = 0;
}

// Mock implementation of the SubscriberInterface

int SubscriberInterface::creationCount = 0;
QList<QDBusConnection::BusType> SubscriberInterface::creationBusTypes;
QStringList SubscriberInterface::creationBusNames;
QStringList SubscriberInterface::creationObjectPaths;

int SubscriberInterface::subscribeCount = 0;
QList<QSet<QString> > SubscriberInterface::subscribeKeys;
int SubscriberInterface::unsubscribeCount = 0;
QList<QSet<QString> > SubscriberInterface::unsubscribeKeys;

SubscriberInterface::SubscriberInterface(const QDBusConnection::BusType busType, const QString& busName,
                        const QString& objectPath, QObject* parent)
{
    // Store the mock implementation (created by the tested class)
    mockSubscriberInterface = this;

    // Log the event: interface created
    ++ creationCount;
    // Log the parameters
    creationBusTypes << busType;
    creationBusNames << busName;
    creationObjectPaths << objectPath;
}

void SubscriberInterface::subscribe(QSet<QString> keys)
{
    ++ subscribeCount;
    subscribeKeys << keys;
}

void SubscriberInterface::unsubscribe(QSet<QString> keys)
{
    ++ unsubscribeCount;
    unsubscribeKeys << keys;
}

void SubscriberInterface::resetLogs()
{
    creationCount = 0;
    creationBusTypes.clear();
    creationBusNames.clear();
    creationObjectPaths.clear();

    subscribeCount = 0;
    subscribeKeys.clear();
    unsubscribeCount = 0;
    unsubscribeKeys.clear();
}


// Mock implementation of the DBusNameListener

DBusNameListener* DBusNameListener::instance(const QDBusConnection::BusType busType, const QString &busName)
{
    // qDebug() << "Returning a mock dbus name listener";
    return mockDBusNameListener;
}

DBusNameListener::DBusNameListener()
{
}


bool DBusNameListener::isServicePresent() const
{
    return true;
}

// Mock implementation of HandleSignalRouter

HandleSignalRouter* HandleSignalRouter::instance()
{
    return mockHandleSignalRouter;
}

void HandleSignalRouter::onValueChanged(QString key, QVariant value, bool processingSubscription)
{
}

//
// Definition of testcases
//


// Before all tests
void PropertyProviderUnitTests::initTestCase()
{
    qRegisterMetaType<QVariant>("QVariant");
    qRegisterMetaType<QSet<QString> >("QSet<QString>");

    qInstallMsgHandler(myMessageOutput);
}

// After all tests
void PropertyProviderUnitTests::cleanupTestCase()
{
}

// Before each test
void PropertyProviderUnitTests::init()
{
    // Create the mock instances
    mockDBusNameListener = new DBusNameListener();
    mockHandleSignalRouter = new HandleSignalRouter();

    // Reset logging of the mock objects
    ManagerInterface::resetLogs();
    SubscriberInterface::resetLogs();

}

// After each test
void PropertyProviderUnitTests::cleanup()
{
    delete mockDBusNameListener;
    mockDBusNameListener = 0;
    delete mockHandleSignalRouter;
    mockHandleSignalRouter = 0;

}

void PropertyProviderUnitTests::initializing()
{
    // Test:
    // Create the object to be tested
    QString busName = "Fake.Bus.Name." + QString(__FUNCTION__);
    propertyProvider = PropertyProvider::instance(QDBusConnection::SessionBus, busName);
    // Note: For each test, we need to create a separate instance.
    // Otherwise the tests are dependent on each other.

    // The PropertyProvider is created (as part of initTestCase)
    // Expected results:
    // PropertyProvider constructed the ManagerInterface with correct parameters
    QCOMPARE(ManagerInterface::creationCount, 1);
    QCOMPARE(ManagerInterface::creationBusTypes.at(0), QDBusConnection::SessionBus);
    QCOMPARE(ManagerInterface::creationBusNames.at(0), busName);
    // PropertyProvider also called the GetSubscriber on the Manager interface
    QCOMPARE(ManagerInterface::getSubscriberCount, 1);
}

void PropertyProviderUnitTests::getSubscriberSucceeds()
{
    // Setup:
    // Create the object to be tested
    QString busName = "Fake.Bus.Name." + QString(__FUNCTION__);
    propertyProvider = PropertyProvider::instance(QDBusConnection::SystemBus, busName);
    // Note: For each test, we need to create a separate instance.
    // Otherwise the tests are dependent on each other.

    // Test:
    // Command the mock manager to emit the getSubscriberFinished signal
    // with a non-empty subscriber object path
    emit mockManagerInterface->getSubscriberFinished("Fake.Subscriber.Path");

    // Expected results:
    // the SubscriberInterface object is created
    QCOMPARE(SubscriberInterface::creationCount, 1);
    QCOMPARE(SubscriberInterface::creationBusTypes.at(0), QDBusConnection::SystemBus);
    QCOMPARE(SubscriberInterface::creationBusNames.at(0), busName);
    QCOMPARE(SubscriberInterface::creationObjectPaths.at(0), QString("Fake.Subscriber.Path"));
}

void PropertyProviderUnitTests::getSubscriberFails()
{
    // Setup:
    // Create the object to be tested
    QString busName = "Fake.Bus.Name." + QString(__FUNCTION__);
    propertyProvider = PropertyProvider::instance(QDBusConnection::SessionBus, busName);
    // Note: For each test, we need to create a separate instance.
    // Otherwise the tests are dependent on each other.

    // And subscribe to a key (so that we can test whether something interesting happened)
    propertyProvider->subscribe("Key.Which.Will.Fail");
    // Start spying on the signal subscribeFinished
    QSignalSpy spy(propertyProvider, SIGNAL(subscribeFinished(QSet<QString>)));

    // Test:
    // Command the mock manager to emit the getSubscriberFinished signal
    // with an empty subscriber object path
    emit mockManagerInterface->getSubscriberFinished("");

    // Expected results:
    // The provider signals that the subscription was finished for that key
    QCOMPARE(spy.count(), 1);
    QList<QVariant> parameters = spy.takeFirst();
    QSet<QString> keysReceived = parameters.at(0).value<QSet<QString> >();
    QCOMPARE(keysReceived.size(), 1);
    QVERIFY(keysReceived.contains("Key.Which.Will.Fail"));
}

void PropertyProviderUnitTests::subscription()
{
    // Setup:
    // Create the object to be tested
    QString busName = "Fake.Bus.Name." + QString(__FUNCTION__);
    propertyProvider = PropertyProvider::instance(QDBusConnection::SessionBus, busName);
    // Note: For each test, we need to create a separate instance.
    // Otherwise the tests are dependent on each other.
    // Command the mock manager to emit the getSubscriberFinished signal
    // with a non-empty subscriber object path
    emit mockManagerInterface->getSubscriberFinished("Fake.Subscriber.Path");

    // Test:
    // Subscribe to a property
    propertyProvider->subscribe("Fake.Key");

    // Expected results:
    // Subscription is scheduled to happen only when we enter the main
    // loop. So, no subscription happened yet.
    QCOMPARE(SubscriberInterface::subscribeCount, 0);

    // Test:
    // Simulate being idle
    QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents);

    // Expected results:
    // The subscription really happends, i.e.,
    // the provider calls the SubscriberInterface::subscribe
    QCOMPARE(SubscriberInterface::subscribeCount, 1);
    QSet<QString> keys = SubscriberInterface::subscribeKeys.at(0);
    QCOMPARE(keys.size(), 1);
    QVERIFY(keys.contains(QString("Fake.Key")));
}

void PropertyProviderUnitTests::subscriptionOfTwoProperties()
{
    // Setup:
    // Create the object to be tested
    QString busName = "Fake.Bus.Name." + QString(__FUNCTION__);
    propertyProvider = PropertyProvider::instance(QDBusConnection::SessionBus, busName);
    // Note: For each test, we need to create a separate instance.
    // Otherwise the tests are dependent on each other.
    // Command the mock manager to emit the getSubscriberFinished signal
    // with a non-empty subscriber object path
    emit mockManagerInterface->getSubscriberFinished("Fake.Subscriber.Path");

    // Test:
    // Subscribe to a property
    propertyProvider->subscribe("Fake.Key.One");
    // And another one
    propertyProvider->subscribe("Fake.Key.Two");

    // Expected results:
    // Subscription is scheduled to happen only when we enter the main
    // loop. So, no subscription happened yet.
    QCOMPARE(SubscriberInterface::subscribeCount, 0);

    // Test:
    // Simulate being idle
    QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents);

    // Expected results:
    // The subscription really happends, i.e.,
    // the provider calls the SubscriberInterface::subscribe.
    // Both keys are batched inside the same request.
    QCOMPARE(SubscriberInterface::subscribeCount, 1);
    QSet<QString> keys = SubscriberInterface::subscribeKeys.at(0);
    QCOMPARE(keys.size(), 2);
    QVERIFY(keys.contains(QString("Fake.Key.One")));
    QVERIFY(keys.contains(QString("Fake.Key.Two")));
}

void PropertyProviderUnitTests::unsubscription()
{
    // Setup:
    // Create the object to be tested
    QString busName = "Fake.Bus.Name." + QString(__FUNCTION__);
    propertyProvider = PropertyProvider::instance(QDBusConnection::SessionBus, busName);
    // Note: For each test, we need to create a separate instance.
    // Otherwise the tests are dependent on each other.
    // Command the mock manager to emit the getSubscriberFinished signal
    // with a non-empty subscriber object path
    emit mockManagerInterface->getSubscriberFinished("Fake.Subscriber.Path");

    // Subscribe to a property
    propertyProvider->subscribe("Fake.Key");
    // Simulate being idle, so that the subscription will really happen
    QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents);

    // Test:
    // Unsubscribe to the same property
    propertyProvider->unsubscribe("Fake.Key");

    // Expected results:
    // Unsubscription is scheduled to happen only when we enter the main
    // loop. So, no unsubscription happened yet.
    QCOMPARE(SubscriberInterface::unsubscribeCount, 0);

    // Test:
    // Simulate being idle
    QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents);

    // Expected results:
    // the provider calls the SubscriberInterface::subscribe
    QCOMPARE(SubscriberInterface::unsubscribeCount, 1);
    QSet<QString> keys = SubscriberInterface::unsubscribeKeys.at(0);
    QCOMPARE(keys.size(), 1);
    QVERIFY(keys.contains(QString("Fake.Key")));
}

void PropertyProviderUnitTests::unsubscriptionOfTwoProperties()
{
    // Setup:
    // Create the object to be tested
    QString busName = "Fake.Bus.Name." + QString(__FUNCTION__);
    propertyProvider = PropertyProvider::instance(QDBusConnection::SessionBus, busName);
    // Note: For each test, we need to create a separate instance.
    // Otherwise the tests are dependent on each other.
    // Command the mock manager to emit the getSubscriberFinished signal
    // with a non-empty subscriber object path
    emit mockManagerInterface->getSubscriberFinished("Fake.Subscriber.Path");

    // Subscribe to two properties
    propertyProvider->subscribe("Fake.Key.One");
    propertyProvider->subscribe("Fake.Key.Two");
    // Simulate being idle, so that the subscription will really happen
    QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents);

    // Test:
    // Unsubscribe to the two properties
    propertyProvider->unsubscribe("Fake.Key.One");
    propertyProvider->unsubscribe("Fake.Key.Two");

    // Expected results:
    // Unsubscription is scheduled to happen only when we enter the main
    // loop. So, no unsubscription happened yet.
    QCOMPARE(SubscriberInterface::unsubscribeCount, 0);

    // Test:
    // Simulate being idle
    QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents);

    // Expected results:
    // the provider calls the SubscriberInterface::subscribe
    // and both keys are batched inside the same request.
    QCOMPARE(SubscriberInterface::unsubscribeCount, 1);
    QSet<QString> keys = SubscriberInterface::unsubscribeKeys.at(0);
    QCOMPARE(keys.size(), 2);
    QVERIFY(keys.contains(QString("Fake.Key.One")));
    QVERIFY(keys.contains(QString("Fake.Key.Two")));
}

void PropertyProviderUnitTests::immediateUnsubscription()
{
    // Setup:
    // Create the object to be tested
    QString busName = "Fake.Bus.Name." + QString(__FUNCTION__);
    propertyProvider = PropertyProvider::instance(QDBusConnection::SessionBus, busName);
    // Note: For each test, we need to create a separate instance.
    // Otherwise the tests are dependent on each other.
    // Command the mock manager to emit the getSubscriberFinished signal
    // with a non-empty subscriber object path
    emit mockManagerInterface->getSubscriberFinished("Fake.Subscriber.Path");

    // Test:
    // Subscribe to a property
    propertyProvider->subscribe("Fake.Key");
    // And unsubscribe to it immediately
    propertyProvider->unsubscribe("Fake.Key");

    // Expected results:
    // No subscription / unsubscription happens
    QCOMPARE(SubscriberInterface::subscribeCount, 0);
    QCOMPARE(SubscriberInterface::unsubscribeCount, 0);

    // Test:
    // Simulate being idle
    QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents);

    // Expected results:
    // Still no subscription / unsubscription happens
    QCOMPARE(SubscriberInterface::subscribeCount, 0);
    QCOMPARE(SubscriberInterface::unsubscribeCount, 0);
}

void PropertyProviderUnitTests::immediateResubscription()
{
    // Setup:
    // Create the object to be tested
    QString busName = "Fake.Bus.Name." + QString(__FUNCTION__);
    propertyProvider = PropertyProvider::instance(QDBusConnection::SessionBus, busName);
    // Note: For each test, we need to create a separate instance.
    // Otherwise the tests are dependent on each other.
    // Command the mock manager to emit the getSubscriberFinished signal
    // with a non-empty subscriber object path
    emit mockManagerInterface->getSubscriberFinished("Fake.Subscriber.Path");

    // Subscribe to a property
    propertyProvider->subscribe("Fake.Key");
    // and make the subscription really happen
    QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents);
    // The logs now include this initial subscription -> clear them.
    SubscriberInterface::resetLogs();

    // Test:
    // Unsubscribe from the prooperty
    propertyProvider->unsubscribe("Fake.Key");
    // And subscribe to it immediately
    propertyProvider->subscribe("Fake.Key");

    // Expected results:
    // No subscription / unsubscription happens
    QCOMPARE(SubscriberInterface::subscribeCount, 0);
    QCOMPARE(SubscriberInterface::unsubscribeCount, 0);

    // Test:
    // Simulate being idle
    QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents);

    // Expected results:
    // Still no subscription / unsubscription happens
    QCOMPARE(SubscriberInterface::subscribeCount, 0);
    QCOMPARE(SubscriberInterface::unsubscribeCount, 0);
}

void PropertyProviderUnitTests::subscriptionFinished()
{
    // Setup:
    // Create the object to be tested
    QString busName = "Fake.Bus.Name." + QString(__FUNCTION__);
    propertyProvider = PropertyProvider::instance(QDBusConnection::SessionBus, busName);
    // Note: For each test, we need to create a separate instance.
    // Otherwise the tests are dependent on each other.
    // Command the mock manager to emit the getSubscriberFinished signal
    // with a non-empty subscriber object path
    emit mockManagerInterface->getSubscriberFinished("Fake.Subscriber.Path");

    // Spy the signal PropertyProvider::subscribeFinished
    QSignalSpy spy(propertyProvider, SIGNAL(subscribeFinished(QSet<QString>)));

    // Subscribe to a property
    propertyProvider->subscribe("Fake.Key");
    // Make the subscription really happen
    QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents);

    // Test:
    // Command the subscriber to emit the subscribeFinished signal
    QSet<QString> keysFinished;
    keysFinished.insert("Fake.Key");
    emit mockSubscriberInterface->subscribeFinished(keysFinished);

    // Expected results:
    // The subscribeFinished is emitted
    QCOMPARE(spy.count(), 1);
    QList<QVariant> parameters = spy.takeFirst();
    QSet<QString> keysReceived = parameters.at(0).value<QSet<QString> >();
    QCOMPARE(keysReceived.size(), 1);
    QVERIFY(keysReceived.contains("Fake.Key"));
}

void PropertyProviderUnitTests::subscriptionAfterGetSubscriberFailed()
{
    // Setup:
    // Create the object to be tested
    QString busName = "Fake.Bus.Name." + QString(__FUNCTION__);
    propertyProvider = PropertyProvider::instance(QDBusConnection::SessionBus, busName);
    // Note: For each test, we need to create a separate instance.
    // Otherwise the tests are dependent on each other.

    // Command the mock manager to emit the getSubscriberFinished signal
    // with an empty subscriber object path
    emit mockManagerInterface->getSubscriberFinished("");
    // And make the manager return true when we ask did it fail
    mockManagerInterface->getSubscriberFailed = true;

    // Start spying on the signal subscribeFinished
    QSignalSpy spy(propertyProvider, SIGNAL(subscribeFinished(QSet<QString>)));

    // Test:
    // Subscribe to a property
    propertyProvider->subscribe("Fake.Key");

    // Expected results:
    // PropertyProvider doesn't actually subscribe to it.
    // PropertyProvider signals that the subscription was finished for that key.
    QCOMPARE(spy.count(), 1);
    QList<QVariant> parameters = spy.takeFirst();
    QSet<QString> keysReceived = parameters.at(0).value<QSet<QString> >();
    QCOMPARE(keysReceived.size(), 1);
    QVERIFY(keysReceived.contains("Fake.Key"));
}


void PropertyProviderUnitTests::valuesChanged()
{
    // Setup:
    // Create the object to be tested
    QString busName = "Fake.Bus.Name." + QString(__FUNCTION__);
    propertyProvider = PropertyProvider::instance(QDBusConnection::SessionBus, busName);
    // Note: For each test, we need to create a separate instance.
    // Otherwise the tests are dependent on each other.
    // Command the mock manager to emit the getSubscriberFinished signal
    // with a non-empty subscriber object path
    emit mockManagerInterface->getSubscriberFinished("Fake.Subscriber.Path");

    // Start spying the valueChanged signal
    QSignalSpy spy(propertyProvider, SIGNAL(valueChanged(QString, QVariant, bool)));

    // Subscribe to two properties
    propertyProvider->subscribe("Fake.Key.One");
    propertyProvider->subscribe("Fake.Key.Two");
    // Make the subscriptions really happen
    QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents);

    // Test:
    // Command the subscriber to emit a valuesChanged signal for this key
    QMap<QString, QVariant> values;
    values.insert("Fake.Key.One", QVariant(312));
    values.insert("Fake.Key.Two", QVariant("myValue"));
    emit mockSubscriberInterface->valuesChanged(values, false);

    // Expected results:
    // The valueChanged is emitted for both keys
    QCOMPARE(spy.count(), 2);
    QList<QVariant> parameters1 = spy.takeFirst();
    QCOMPARE(parameters1.at(0), QVariant("Fake.Key.One"));
    QCOMPARE(parameters1.size(), 3);
    QCOMPARE(parameters1.at(1).value<QVariant>(), QVariant(312));
    // Note: Getting the QVariant out from QSignalSpy causes this ugliness.
    QCOMPARE(parameters1.at(2), QVariant(false));
    QList<QVariant> parameters2 = spy.takeFirst();
    QCOMPARE(parameters2.size(), 3);
    QCOMPARE(parameters2.at(0), QVariant("Fake.Key.Two"));
    QCOMPARE(parameters2.at(1).value<QVariant>(), QVariant("myValue"));
    QCOMPARE(parameters2.at(2), QVariant(false));
}

void PropertyProviderUnitTests::valuesChangedWithUnnecessaryProperties()
{
    // Setup:
    // Create the object to be tested
    QString busName = "Fake.Bus.Name." + QString(__FUNCTION__);
    propertyProvider = PropertyProvider::instance(QDBusConnection::SessionBus, busName);
    // Note: For each test, we need to create a separate instance.
    // Otherwise the tests are dependent on each other.

    // Command the mock manager to emit the getSubscriberFinished signal
    // with a non-empty subscriber object path
    emit mockManagerInterface->getSubscriberFinished("Fake.Subscriber.Path");

    // Start spying the valueChanged signal
    QSignalSpy spy(propertyProvider, SIGNAL(valueChanged(QString, QVariant, bool)));

    // Subscribe to two properties
    propertyProvider->subscribe("Fake.Key.One");
    propertyProvider->subscribe("Fake.Key.Two");
    // Make the subscriptions really happen
    QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents);

    // Test:
    // Command the subscriber to emit a valuesChanged signal
    // for two keys which are not subscribed to.
    QMap<QString, QVariant> values;
    values.insert("Something.Totally.Different", QVariant(false));
    values.insert("Not.Subscribed.To", QVariant(8.42));
    emit mockSubscriberInterface->valuesChanged(values, false);

    // Expected results:
    // The valueChanged not is emitted at all
    QCOMPARE(spy.count(), 0);
}

void PropertyProviderUnitTests::providerDisappearsAndAppears()
{
    // Setup:
    // Create the object to be tested
    QString busName = "Fake.Bus.Name." + QString(__FUNCTION__);
    propertyProvider = PropertyProvider::instance(QDBusConnection::SessionBus, busName);
    // Note: For each test, we need to create a separate instance.
    // Otherwise the tests are dependent on each other.

    // Command the mock manager to emit the getSubscriberFinished signal
    // with a non-empty subscriber object path
    emit mockManagerInterface->getSubscriberFinished("Fake.Subscriber.Path");

    // Subscribe to a property
    propertyProvider->subscribe("Fake.Key");
    // Make the subscriptions really happen
    QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents);

    // The logs now contain calls that happen during initialization, reset them.
    ManagerInterface::resetLogs();
    SubscriberInterface::resetLogs();

    // Test:
    // Fake that the provider disappears
    emit mockDBusNameListener->nameDisappeared();

    // And appears again
    emit mockDBusNameListener->nameAppeared();

    // Expected results:
    // PropertyProvider called again the GetSubscriber on the Manager interface
    QCOMPARE(ManagerInterface::getSubscriberCount, 1);

    // Test:
    // Command the mock manager to emit the getSubscriberFinished signal
    // with a non-empty subscriber object path
    emit mockManagerInterface->getSubscriberFinished("Fake.Subscriber.Path");

    // Expected result:
    // The PropertyProvider creates the Subscriber object
    QCOMPARE(SubscriberInterface::creationCount, 1);
    QCOMPARE(SubscriberInterface::creationBusTypes.at(0), QDBusConnection::SessionBus);
    QCOMPARE(SubscriberInterface::creationBusNames.at(0), busName);
    QCOMPARE(SubscriberInterface::creationObjectPaths.at(0), QString("Fake.Subscriber.Path"));

    // Test:
    // Simulate being idle
    QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents);

    // Expected results:
    // The PropertyProvider renews the subscription to the key
    QCOMPARE(SubscriberInterface::subscribeCount, 1);
    QSet<QString> keys = SubscriberInterface::subscribeKeys.at(0);
    QCOMPARE(keys.size(), 1);
    QVERIFY(keys.contains(QString("Fake.Key")));
}



QTEST_MAIN(PropertyProviderUnitTests);
