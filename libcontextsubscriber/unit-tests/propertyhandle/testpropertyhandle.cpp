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
#include "testpropertyhandle.h"

// Mock header files
#include "propertyprovider.h"
#include "dbusnamelistener.h"
#include "contextpropertyinfo.h"
#include "contextregistryinfo.h"

// Header file of the class to be tested
#include "propertyhandle.h"

#include <QtTest/QtTest>
#include <QDebug>
#include <QDBusConnection>

#define MYLOGLEVEL 3
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
PropertyProvider* mockPropertyProvider;
PropertyProvider* mockCommanderProvider;

ContextRegistryInfo* mockContextRegistryInfo;
// These will be created by the tested class and stored here
ContextPropertyInfo* mockContextPropertyInfo;
DBusNameListener* mockDBusNameListener;

// Mock implementation of the PropertyProvider
int PropertyProvider::instanceCount = 0;
QList<QDBusConnection::BusType> PropertyProvider::instanceDBusTypes;
QStringList PropertyProvider::instanceDBusNames;
int PropertyProvider::subscribeCount = 0;
QStringList PropertyProvider::subscribeKeys;
QStringList PropertyProvider::subscribeProviderNames;

int PropertyProvider::unsubscribeCount = 0;
QStringList PropertyProvider::unsubscribeKeys;
QStringList PropertyProvider::unsubscribeProviderNames;

PropertyProvider* PropertyProvider::instance(const QDBusConnection::BusType busType, const QString &busName)
{
    ++ instanceCount;
    instanceDBusTypes << busType;
    instanceDBusNames << busName;
    if (busName.contains("Commander")) {
        return mockCommanderProvider;
    }
    return mockPropertyProvider;
}

PropertyProvider::PropertyProvider(QString name)
    : myName(name)
{
}

void PropertyProvider::subscribe(const QString& key)
{
    qDebug() << "subscribe" << key << myName;
    ++subscribeCount;
    subscribeKeys << key;
    subscribeProviderNames << myName;
}

void PropertyProvider::unsubscribe(const QString& key)
{
    qDebug() << "unsubscribe" << key << myName;
    ++unsubscribeCount;
    unsubscribeKeys << key;
    unsubscribeProviderNames << myName;
}

void PropertyProvider::resetLogs()
{
    instanceCount = 0;
    instanceDBusTypes.clear();
    instanceDBusNames.clear();
    subscribeCount = 0;
    subscribeKeys.clear();
    subscribeProviderNames.clear();
    unsubscribeCount = 0;
    unsubscribeKeys.clear();
    unsubscribeProviderNames.clear();
}

// Mock implementation of the DBusNameListener

DBusNameListener::DBusNameListener(const QDBusConnection::BusType busType,
                                   const QString &busName, bool initialCheck, QObject* parent)
 : servicePresent(false)
{
    // Store the object created by the class to be tested
    mockDBusNameListener = this;
}

bool DBusNameListener::isServicePresent() const
{
    return servicePresent;
}

// Mock implementation of ContextPropertyInfo

ContextPropertyInfo::ContextPropertyInfo(const QString &key, QObject *parent)
    : myType("")
{
    // Store the object created by the class to be tested
    mockContextPropertyInfo = this;
}

QString ContextPropertyInfo::type() const
{
    return myType;
}

QString ContextPropertyInfo::providerDBusName() const
{
    return "Fake.Provider";
}

QDBusConnection::BusType ContextPropertyInfo::providerDBusType() const
{
    return QDBusConnection::SessionBus;
}

// Mock implementation of the ContextRegistryInfo

ContextRegistryInfo* ContextRegistryInfo::instance(const QString& backendName)
{
    return mockContextRegistryInfo;
}

//
// Definition of testcases
//


// Before all tests
void PropertyHandleUnitTests::initTestCase()
{
    qInstallMsgHandler(myMessageOutput);
}

// After all tests
void PropertyHandleUnitTests::cleanupTestCase()
{
}

// Before each test
void PropertyHandleUnitTests::init()
{
    // Create the mock instances
    mockPropertyProvider = new PropertyProvider("NormalProvider");
    mockCommanderProvider = new PropertyProvider("Commander");
    mockContextRegistryInfo = new ContextRegistryInfo();
    // Reset the logs
    PropertyProvider::resetLogs();
    // Reset the DBusNameListener (it is created only once, by the class to be tested)
    mockDBusNameListener->servicePresent = false;
}

// After each test
void PropertyHandleUnitTests::cleanup()
{
    delete mockPropertyProvider;
    mockPropertyProvider = 0;
    delete mockCommanderProvider;
    mockCommanderProvider = 0;
    delete mockContextRegistryInfo;
    mockContextRegistryInfo = 0;

    // Disconnect all signals from the DBusNameListener.
    // Even if all the PropertyHandle objects (created in the tests) will live,
    // they won't receive any signals.
    QObject::disconnect(mockDBusNameListener, SIGNAL(nameAppeared()), 0, 0);
    QObject::disconnect(mockDBusNameListener, SIGNAL(nameDisappeared()), 0, 0);
}

void PropertyHandleUnitTests::initializing()
{
    // Test:
    // Create the object to be tested
    QString key = "Property." + QString(__FUNCTION__);
    propertyHandle = PropertyHandle::instance(key);
    // Note: For each test, we need to create a separate instance.
    // Otherwise the tests are dependent on each other.

    // Expected results:
    // The PropertyProvider with the correct DBusName and DBusType was created.
    QCOMPARE(PropertyProvider::instanceCount, 1);
    QCOMPARE(PropertyProvider::instanceDBusTypes.at(0), QDBusConnection::SessionBus);
    QCOMPARE(PropertyProvider::instanceDBusNames.at(0), QString("Fake.Provider"));
}

void PropertyHandleUnitTests::key()
{
    // Setup:
    // Create the object to be tested
    QString key = "Property." + QString(__FUNCTION__);
    propertyHandle = PropertyHandle::instance(key);
    // Note: For each test, we need to create a separate instance.
    // Otherwise the tests are dependent on each other.

    // Test and expected results:
    // The key() function returns the correct key
    QCOMPARE(propertyHandle->key(), key);
}

void PropertyHandleUnitTests::info()
{
    // Setup:
    // Create the object to be tested
    QString key = "Property." + QString(__FUNCTION__);
    propertyHandle = PropertyHandle::instance(key);
    // Note: For each test, we need to create a separate instance.
    // Otherwise the tests are dependent on each other.

    // Test and expected results:
    // The info() function returns the correct ContextPropertyInfo
    QCOMPARE(propertyHandle->info(), mockContextPropertyInfo);
}

void PropertyHandleUnitTests::subscribe()
{
    // Setup:
    // Create the object to be tested
    QString key = "Property." + QString(__FUNCTION__);
    propertyHandle = PropertyHandle::instance(key);
    // Note: For each test, we need to create a separate instance.
    // Otherwise the tests are dependent on each other.

    // Test:
    // Command the PropertyHandle to subscribe
    propertyHandle->subscribe();

    // Expected results:
    // The PropertyHandle calls the PropertyProvider::subscribe
    QCOMPARE(PropertyProvider::subscribeCount, 1);
    QCOMPARE(PropertyProvider::subscribeKeys.at(0), key);
    QCOMPARE(PropertyProvider::unsubscribeCount, 0);
}

void PropertyHandleUnitTests::subscribeAndUnsubscribe()
{
    // Setup:
    // Create the object to be tested
    QString key = "Property." + QString(__FUNCTION__);
    propertyHandle = PropertyHandle::instance(key);
    // Note: For each test, we need to create a separate instance.
    // Otherwise the tests are dependent on each other.

    // Test:
    // Command the PropertyHandle to subscribe
    propertyHandle->subscribe();
    // Command the PropertyHandle to unsubscribe
    propertyHandle->unsubscribe();

    // Expected results:
    // The PropertyHandle calls the PropertyProvider::unsubscribe
    QCOMPARE(PropertyProvider::subscribeCount, 1);
    QCOMPARE(PropertyProvider::subscribeKeys.at(0), key);
    QCOMPARE(PropertyProvider::unsubscribeCount, 1);
    QCOMPARE(PropertyProvider::unsubscribeKeys.at(0), key);
}

void PropertyHandleUnitTests::subscribeTwice()
{
    // Setup:
    // Create the object to be tested
    QString key = "Property." + QString(__FUNCTION__);
    propertyHandle = PropertyHandle::instance(key);
    // Note: For each test, we need to create a separate instance.
    // Otherwise the tests are dependent on each other.

    // Test:
    // Command the PropertyHandle to subscribe
    propertyHandle->subscribe();
    // and subscribe again
    propertyHandle->subscribe();

    // Expected results:
    // The PropertyHandle calls the PropertyProvider::subscribe
    // but only once.
    QCOMPARE(PropertyProvider::subscribeCount, 1);
    QCOMPARE(PropertyProvider::subscribeKeys.at(0), key);
    QCOMPARE(PropertyProvider::unsubscribeCount, 0);
}

void PropertyHandleUnitTests::subscriptionPendingAndFinished()
{
    // Setup:
    // Create the object to be tested
    QString key = "Property." + QString(__FUNCTION__);
    propertyHandle = PropertyHandle::instance(key);
    // Note: For each test, we need to create a separate instance.
    // Otherwise the tests are dependent on each other.

    // Test:
    // Command the PropertyHandle to subscribe
    propertyHandle->subscribe();

    // Expected results:
    // The subscription is marked as pending
    QVERIFY(propertyHandle->isSubscribePending());

    // Test:
    // Command the PropertyProvider to emit the subscriptionFinished signal
    // without the relevant key
    QSet<QString> toEmit1;
    toEmit1.insert("Non.Relevant.Key");
    emit mockPropertyProvider->subscribeFinished(toEmit1);

    // Expected results:
    // The subscription is still marked as pending
    QVERIFY(propertyHandle->isSubscribePending());

    // Test:
    // Command the PropertyProvider to emit the subscriptionFinished signal
    // with the relevant key
    QSet<QString> toEmit2;
    toEmit2.insert(key);
    emit mockPropertyProvider->subscribeFinished(toEmit2);

    // Expected results:
    // The subscription is no longer marked as pending
    QCOMPARE(propertyHandle->isSubscribePending(), false);
}

void PropertyHandleUnitTests::subscribeTwiceAndUnsubscribe()
{
    // Setup:
    // Create the object to be tested
    QString key = "Property." + QString(__FUNCTION__);
    propertyHandle = PropertyHandle::instance(key);
    // Note: For each test, we need to create a separate instance.
    // Otherwise the tests are dependent on each other.

    // Test:
    // Command the PropertyHandle to subscribe
    propertyHandle->subscribe();
    // and subscribe again
    propertyHandle->subscribe();
    // and then unsubscribe.
    propertyHandle->unsubscribe();

    // Expected results:
    // The PropertyHandle calls the PropertyProvider::subscribe
    // but only once. The unsubscribe is not called, since one of the
    // subscriptions is still valid.
    QCOMPARE(PropertyProvider::subscribeCount, 1);
    QCOMPARE(PropertyProvider::subscribeKeys.at(0), key);
    QCOMPARE(PropertyProvider::unsubscribeCount, 0);
}

void PropertyHandleUnitTests::subscribeTwiceAndUnsubscribeTwice()
{
    // Setup:
    // Create the object to be tested
    QString key = "Property." + QString(__FUNCTION__);
    propertyHandle = PropertyHandle::instance(key);
    // Note: For each test, we need to create a separate instance.
    // Otherwise the tests are dependent on each other.

    // Test:
    // Command the PropertyHandle to subscribe
    propertyHandle->subscribe();
    // and subscribe again
    propertyHandle->subscribe();
    // and then unsubscribe
    propertyHandle->unsubscribe();
    // and unsubscribe again
    propertyHandle->unsubscribe();

    // Expected results:
    // The PropertyHandle calls the PropertyProvider::subscribe
    // but only once. Also the unsubscription is called once.
    QCOMPARE(PropertyProvider::subscribeCount, 1);
    QCOMPARE(PropertyProvider::subscribeKeys.at(0), key);
    QCOMPARE(PropertyProvider::unsubscribeCount, 1);
    QCOMPARE(PropertyProvider::unsubscribeKeys.at(0), key);
}

void PropertyHandleUnitTests::setValueWithoutTypeCheck()
{
    // Setup:
    // Create the object to be tested
    QString key = "Property." + QString(__FUNCTION__);
    propertyHandle = PropertyHandle::instance(key);
    // Note: For each test, we need to create a separate instance.
    // Otherwise the tests are dependent on each other.

    // Start listening to the valueChanged signal
    QSignalSpy spy(propertyHandle, SIGNAL(valueChanged()));

    // Test:
    spy.clear();
    // Command the PropertyHandle to change its value
    // The new value is a double
    propertyHandle->setValue(QVariant(1.4), true);

    // Expected results:
    // The valueChanged signal was emitted
    QCOMPARE(spy.count(), 1);
    // The new value is now accessible
    QCOMPARE(propertyHandle->value(), QVariant(1.4));

    // Test:
    spy.clear();
    // Command the PropertyHandle to set its value to the same value it already has
    propertyHandle->setValue(QVariant(1.4), true);

    // Expected results:
    // The valueChanged signal is not emitted
    QCOMPARE(spy.count(), 0);

    // Test:
    spy.clear();
    // Command the PropertyHandle to change its value
    // The new value is an integer
    propertyHandle->setValue(QVariant(-8), true);

    // Expected results:
    // The valueChanged signal was emitted
    QCOMPARE(spy.count(), 1);
    // The new value is now accessible
    QCOMPARE(propertyHandle->value(), QVariant(-8));
    QCOMPARE(propertyHandle->value().isNull(), false);

    // Test:
    spy.clear();
    // Command the PropertyHandle to set its value to the same value it already has
    propertyHandle->setValue(QVariant(-8), true);

    // Expected results:
    // The valueChanged signal is not emitted
    QCOMPARE(spy.count(), 0);

    // Test:
    spy.clear();
    // Command the PropertyHandle to change its value
    // The new value is a string
    propertyHandle->setValue(QVariant("myValue"), true);

    // Expected results:
    // The valueChanged signal was emitted
    QCOMPARE(spy.count(), 1);
    // The new value is now accessible
    QCOMPARE(propertyHandle->value(), QVariant("myValue"));
    QCOMPARE(propertyHandle->value().isNull(), false);

    // Test:
    spy.clear();
    // Command the PropertyHandle to set its value to the same value it already has
    propertyHandle->setValue(QVariant("myValue"), true);

    // Expected results:
    // The valueChanged signal is not emitted
    QCOMPARE(spy.count(), 0);

    // Test:
    spy.clear();
    // Command the PropertyHandle to change its value
    // The new value is a boolean
    propertyHandle->setValue(QVariant(true), true);

    // Expected results:
    // The valueChanged signal was emitted
    QCOMPARE(spy.count(), 1);
    // The new value is now accessible
    QCOMPARE(propertyHandle->value(), QVariant(true));
    QCOMPARE(propertyHandle->value().isNull(), false);

    // Test:
    spy.clear();
    // Command the PropertyHandle to set its value to the same value it already has
    propertyHandle->setValue(QVariant(true), true);

    // Expected results:
    // The valueChanged signal is not emitted
    QCOMPARE(spy.count(), 0);

    // Test:
    spy.clear();
    // Command the PropertyHandle to change its value
    // The new value is a null
    propertyHandle->setValue(QVariant(), true);

    // Expected results:
    // The valueChanged signal was emitted
    QCOMPARE(spy.count(), 1);
    // The new value is now accessible
    QCOMPARE(propertyHandle->value(), QVariant());
    QCOMPARE(propertyHandle->value().isNull(), true);

    // Test:
    spy.clear();
    // Command the PropertyHandle to set its value to the same value it already has
    propertyHandle->setValue(QVariant(), true);

    // Expected results:
    // The valueChanged signal is not emitted
    QCOMPARE(spy.count(), 0);

}

void PropertyHandleUnitTests::setValueWithTypeCheckAndCorrectTypes()
{
    // Setup:
    // Create the object to be tested
    QString key = "Property." + QString(__FUNCTION__);
    propertyHandle = PropertyHandle::instance(key);
    // Note: For each test, we need to create a separate instance.
    // Otherwise the tests are dependent on each other.

    // Enable the type checks
    PropertyHandle::setTypeCheck(true);

    // Start listening to the valueChanged signal
    QSignalSpy spy(propertyHandle, SIGNAL(valueChanged()));

    // Setup:
    spy.clear();
    // Set the type to DOUBLE
    mockContextPropertyInfo->myType = "DOUBLE";

    // Test:
    // Command the PropertyHandle to change its value
    // The new value is a double
    propertyHandle->setValue(QVariant(1.4), true);

    // Expected results:
    // The valueChanged signal was emitted
    QCOMPARE(spy.count(), 1);
    // The new value is now accessible
    QCOMPARE(propertyHandle->value(), QVariant(1.4));

    // Test:
    spy.clear();
    // Command the PropertyHandle to change its value
    // The new value is a null
    propertyHandle->setValue(QVariant(), true);

    // Expected results:
    // The NULL value is always accepcted (not depending on the type)
    // The valueChanged signal was emitted
    QCOMPARE(spy.count(), 1);
    // The new value is now accessible
    QCOMPARE(propertyHandle->value(), QVariant());
    QCOMPARE(propertyHandle->value().isNull(), true);

    // Setup:
    spy.clear();
    // Set the type to INT
    mockContextPropertyInfo->myType = "INT";

    // Test:
    // Command the PropertyHandle to change its value
    // The new value is an integer
    propertyHandle->setValue(QVariant(-8), true);

    // Expected results:
    // The valueChanged signal was emitted
    QCOMPARE(spy.count(), 1);
    // The new value is now accessible
    QCOMPARE(propertyHandle->value(), QVariant(-8));
    QCOMPARE(propertyHandle->value().isNull(), false);

    // Test:
    spy.clear();
    // Command the PropertyHandle to change its value
    // The new value is a null
    propertyHandle->setValue(QVariant(), true);

    // Expected results:
    // The NULL value is always accepcted (not depending on the type)
    // The valueChanged signal was emitted
    QCOMPARE(spy.count(), 1);
    // The new value is now accessible
    QCOMPARE(propertyHandle->value(), QVariant());
    QCOMPARE(propertyHandle->value().isNull(), true);

    // Setup:
    spy.clear();
    // Set the type to STRING
    mockContextPropertyInfo->myType = "STRING";

    // Test:
    // Command the PropertyHandle to change its value
    // The new value is a string
    propertyHandle->setValue(QVariant("myValue"), true);

    // Expected results:
    // The valueChanged signal was emitted
    QCOMPARE(spy.count(), 1);
    // The new value is now accessible
    QCOMPARE(propertyHandle->value(), QVariant("myValue"));
    QCOMPARE(propertyHandle->value().isNull(), false);

    // Test:
    spy.clear();
    // Command the PropertyHandle to change its value
    // The new value is a null
    propertyHandle->setValue(QVariant(), true);

    // Expected results:
    // The NULL value is always accepcted (not depending on the type)
    // The valueChanged signal was emitted
    QCOMPARE(spy.count(), 1);
    // The new value is now accessible
    QCOMPARE(propertyHandle->value(), QVariant());
    QCOMPARE(propertyHandle->value().isNull(), true);

    // Setup:
    spy.clear();
    // Set the type to TRUTH
    mockContextPropertyInfo->myType = "TRUTH";

    // Test:
    // Command the PropertyHandle to change its value
    // The new value is a boolean
    propertyHandle->setValue(QVariant(true), true);

    // Expected results:
    // The valueChanged signal was emitted
    QCOMPARE(spy.count(), 1);
    // The new value is now accessible
    QCOMPARE(propertyHandle->value(), QVariant(true));
    QCOMPARE(propertyHandle->value().isNull(), false);

    // Test:
    spy.clear();
    // Command the PropertyHandle to change its value
    // The new value is a null
    propertyHandle->setValue(QVariant(), true);

    // Expected results:
    // The NULL value is always accepcted (not depending on the type)
    // The valueChanged signal was emitted
    QCOMPARE(spy.count(), 1);
    // The new value is now accessible
    QCOMPARE(propertyHandle->value(), QVariant());
    QCOMPARE(propertyHandle->value().isNull(), true);

}

void PropertyHandleUnitTests::setValueWithTypeCheckAndIncorrectTypes()
{
    // Setup:
    // Create the object to be tested
    QString key = "Property." + QString(__FUNCTION__);
    propertyHandle = PropertyHandle::instance(key);
    // Note: For each test, we need to create a separate instance.
    // Otherwise the tests are dependent on each other.

    // Enable the type checks
    PropertyHandle::setTypeCheck(true);

    // Start listening to the valueChanged signal
    QSignalSpy spy(propertyHandle, SIGNAL(valueChanged()));

    // Setup:
    // Set the type to DOUBLE
    mockContextPropertyInfo->myType = "DOUBLE";
    // Set an initial value
    propertyHandle->setValue(QVariant(4.2), true);
    spy.clear();

    // Test:
    // Command the PropertyHandle to change its value multiple times but with
    // incorrect types.
    propertyHandle->setValue(QVariant(5), true);
    propertyHandle->setValue(QVariant("string"), true);
    propertyHandle->setValue(QVariant(false), true);

    // Expected results:
    // The valueChanged signal was not emitted
    QCOMPARE(spy.count(), 0);
    // The value is still the old value
    QCOMPARE(propertyHandle->value(), QVariant(4.2));

    // Setup:
    // Set the type to INT
    mockContextPropertyInfo->myType = "INT";
    // Set an initial value
    propertyHandle->setValue(QVariant(22), true);
    spy.clear();

    // Test:
    // Command the PropertyHandle to change its value multiple times but with
    // incorrect types.
    propertyHandle->setValue(QVariant(5.6), true);
    propertyHandle->setValue(QVariant("string"), true);
    propertyHandle->setValue(QVariant(false), true);

    // Expected results:
    // The valueChanged signal was not emitted
    QCOMPARE(spy.count(), 0);
    // The value is still the old value
    QCOMPARE(propertyHandle->value(), QVariant(22));

    // Setup:
    // Set the type to STRING
    mockContextPropertyInfo->myType = "STRING";
    // Set an initial value
    propertyHandle->setValue(QVariant("myString"), true);
    spy.clear();

    // Test:
    // Command the PropertyHandle to change its value multiple times but with
    // incorrect types.
    propertyHandle->setValue(QVariant(5.4), true);
    propertyHandle->setValue(QVariant(-8), true);
    propertyHandle->setValue(QVariant(false), true);

    // Expected results:
    // The valueChanged signal was not emitted
    QCOMPARE(spy.count(), 0);
    // The value is still the old value
    QCOMPARE(propertyHandle->value(), QVariant("myString"));

    // Setup:
    // Set the type to TRUTH
    mockContextPropertyInfo->myType = "TRUTH";
    // Set an initial value
    propertyHandle->setValue(QVariant(false), true);
    spy.clear();

    // Test:
    // Command the PropertyHandle to change its value multiple times but with
    // incorrect types.
    propertyHandle->setValue(QVariant(5.4), true);
    propertyHandle->setValue(QVariant(-8), true);
    propertyHandle->setValue(QVariant("string"), true);

    // Expected results:
    // The valueChanged signal was not emitted
    QCOMPARE(spy.count(), 0);
    // The value is still the old value
    QCOMPARE(propertyHandle->value(), QVariant(false));
}

void PropertyHandleUnitTests::commanderAppearsAndDisappears()
{
    // Setup:
    // Create the object to be tested
    QString key = "Property." + QString(__FUNCTION__);
    propertyHandle = PropertyHandle::instance(key);
    // Note: For each test, we need to create a separate instance.
    // Otherwise the tests are dependent on each other.

    // Subscribe to the PropertyHandle
    propertyHandle->subscribe();

    // Clear the logs from the subscription
    PropertyProvider::resetLogs();

    // Test:
    // Command the DBusNameListener to tell that the Commander has appeared
    mockDBusNameListener->servicePresent = true;
    emit mockDBusNameListener->nameAppeared();

    // Expected results:
    // The handle unsubscribes from the real provider
    QCOMPARE(PropertyProvider::unsubscribeCount, 1);
    QCOMPARE(PropertyProvider::unsubscribeKeys.at(0), key);
    QCOMPARE(PropertyProvider::unsubscribeProviderNames.at(0), QString("NormalProvider"));
    // And subscribes to Commander
    QCOMPARE(PropertyProvider::subscribeCount, 1);
    QCOMPARE(PropertyProvider::subscribeKeys.at(0), key);
    QCOMPARE(PropertyProvider::subscribeProviderNames.at(0), QString("Commander"));

    // Setup:
    // Clear the logs from the subscription
    PropertyProvider::resetLogs();

    // Test:
    // Command the DBusNameListener to tell that the Commander has disappeared
    mockDBusNameListener->servicePresent = false;
    emit mockDBusNameListener->nameDisappeared();

    // Expected results:
    // The PropertyHandle unsubscribes from the commander
    QCOMPARE(PropertyProvider::unsubscribeCount, 1);
    QCOMPARE(PropertyProvider::unsubscribeKeys.at(0), key);
    QCOMPARE(PropertyProvider::unsubscribeProviderNames.at(0), QString("Commander"));
    // And subscribes to the real provider
    QCOMPARE(PropertyProvider::subscribeCount, 1);
    QCOMPARE(PropertyProvider::subscribeKeys.at(0), key);
    QCOMPARE(PropertyProvider::subscribeProviderNames.at(0), QString("NormalProvider"));
}

void PropertyHandleUnitTests::commandingDisabled()
{
    // Setup:
    // Create the object to be tested
    QString key = "Property." + QString(__FUNCTION__);
    propertyHandle = PropertyHandle::instance(key);
    // Note: For each test, we need to create a separate instance.
    // Otherwise the tests are dependent on each other.

    // Subscribe to the handle
    propertyHandle->subscribe();

    // Clear the logs from the subscription
    PropertyProvider::resetLogs();

    // Test:
    // Disable commanding
    propertyHandle->ignoreCommander();

    // Command the DBusNameListener to tell that the Commander has appeared
    mockDBusNameListener->servicePresent = true;
    emit mockDBusNameListener->nameAppeared();

    // Expected results:
    // The PropertyHandle ignores the Commander
    QCOMPARE(PropertyProvider::unsubscribeCount, 0);
    QCOMPARE(PropertyProvider::subscribeCount, 0);

    // Setup:
    // Clear the logs from the subscription
    PropertyProvider::resetLogs();

    // Test:
    // Command the DBusNameListener to tell that the Commander has disappeared
    mockDBusNameListener->servicePresent = false;
    emit mockDBusNameListener->nameDisappeared();

    // Expected results:
    // The PropertyHandle ignores the Commander
    QCOMPARE(PropertyProvider::unsubscribeCount, 0);
    QCOMPARE(PropertyProvider::subscribeCount, 0);
}



QTEST_MAIN(PropertyHandleUnitTests);
