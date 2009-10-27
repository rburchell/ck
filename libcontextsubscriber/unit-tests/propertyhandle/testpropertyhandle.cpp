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
#include "provider.h"
#include "dbusnamelistener.h"
#include "contextpropertyinfo.h"
#include "contextregistryinfo.h"

// Header file of the class to be tested
#include "propertyhandle.h"

#include <QtTest/QtTest>
#include <QDebug>
#include <QDBusConnection>

ContextRegistryInfo* mockContextRegistryInfo;
ContextPropertyInfo* mockContextPropertyInfo;

// Mock implementation of ContextPropertyInfo

ContextPropertyInfo::ContextPropertyInfo(const QString &key, QObject *parent)
    : myType("faketype")
{
    // Store the object created by the class to be tested
    mockContextPropertyInfo = this;
}

QString ContextPropertyInfo::type() const
{
    return myType;
}

bool ContextPropertyInfo::provided() const
{
    return true;
}

QList<ContextProviderInfo> ContextPropertyInfo::providers() const
{
    return QList<ContextProviderInfo>() << ContextProviderInfo("fakeplugin", "fakeconstructionstring");
}

// Mock implementation of the ContextRegistryInfo

ContextRegistryInfo* ContextRegistryInfo::instance(const QString& backendName)
{
    return mockContextRegistryInfo;
}

namespace ContextSubscriber {

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
Provider* mockProvider;
Provider* mockCommanderProvider;
DBusNameListener* mockDBusNameListener;

// Mock implementation of the Provider
int Provider::instanceCount = 0;
QStringList Provider::instancePluginNames;
QStringList Provider::instancePluginConstructionStrings;
int Provider::subscribeCount = 0;
QStringList Provider::subscribeKeys;
QStringList Provider::subscribeProviderNames;

int Provider::unsubscribeCount = 0;
QStringList Provider::unsubscribeKeys;
QStringList Provider::unsubscribeProviderNames;

TimedValue Provider::cachedValue = TimedValue(QVariant());

Provider* Provider::instance(const ContextProviderInfo& providerInfo)
{
    ++ instanceCount;
    instancePluginNames << providerInfo.plugin;
    instancePluginConstructionStrings << providerInfo.constructionString;
    if (providerInfo.constructionString.contains("Commander")) {
        return mockCommanderProvider;
    }
    return mockProvider;
}

Provider::Provider(QString name)
    : myName(name)
{
}

bool Provider::subscribe(const QString& key)
{
    qDebug() << "subscribe" << key << myName;
    ++subscribeCount;
    subscribeKeys << key;
    subscribeProviderNames << myName;
    return true;
}

void Provider::unsubscribe(const QString& key)
{
    qDebug() << "unsubscribe" << key << myName;
    ++unsubscribeCount;
    unsubscribeKeys << key;
    unsubscribeProviderNames << myName;
}

void Provider::clearValues()
{
    cachedValue = TimedValue(QVariant());
}

void Provider::setValue(const QString &key, const QVariant &value)
{
    cachedValue = TimedValue(value);
    PropertyHandle::instance(key)->onValueChanged();
}

TimedValue Provider::get(const QString &key) const
{
    return cachedValue;
}

void Provider::resetLogs()
{
    instanceCount = 0;
    instancePluginConstructionStrings.clear();
    instancePluginNames.clear();
    subscribeCount = 0;
    subscribeKeys.clear();
    subscribeProviderNames.clear();
    unsubscribeCount = 0;
    unsubscribeKeys.clear();
    unsubscribeProviderNames.clear();
}

// Mock implementation of the DBusNameListener

DBusNameListener::DBusNameListener(QDBusConnection::BusType busType,
                                   const QString &busName, QObject* parent)
 : servicePresent(NotPresent)
{
    // Store the object created by the class to be tested
    mockDBusNameListener = this;
}

DBusNameListener::~DBusNameListener()
{
}

DBusNameListener::ServicePresence DBusNameListener::isServicePresent() const
{
    return servicePresent;
}

void DBusNameListener::startListening(bool nameHasOwnerCheck)
{
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
    mockProvider = new Provider("NormalProvider");
    mockCommanderProvider = new Provider("Commander");
    mockContextRegistryInfo = new ContextRegistryInfo();
    // Reset the logs
    Provider::resetLogs();
    // Reset the DBusNameListener (it is created only once, by the class to be tested)
    mockDBusNameListener->servicePresent = DBusNameListener::NotPresent;
}

// After each test
void PropertyHandleUnitTests::cleanup()
{
    delete mockProvider;
    mockProvider = 0;
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
    // Note: For each test, we need to create a separate instance.
    // Otherwise the tests are dependent on each other.
    QString key = "Property." + QString(__FUNCTION__);
    propertyHandle = PropertyHandle::instance(key);

    // Expected results:
    // The Provider with the correct DBusName and DBusType was created.
    QCOMPARE(Provider::instanceCount, 1);
    QCOMPARE(Provider::instancePluginNames.at(0), QString("fakeplugin"));
    QCOMPARE(Provider::instancePluginConstructionStrings.at(0), QString("fakeconstructionstring"));
}

void PropertyHandleUnitTests::key()
{
    // Setup:
    // Create the object to be tested
    // Note: For each test, we need to create a separate instance.
    // Otherwise the tests are dependent on each other.
    QString key = "Property." + QString(__FUNCTION__);
    propertyHandle = PropertyHandle::instance(key);

    // Test and expected results:
    // The key() function returns the correct key
    QCOMPARE(propertyHandle->key(), key);
}

void PropertyHandleUnitTests::info()
{
    // Setup:
    // Create the object to be tested
    // Note: For each test, we need to create a separate instance.
    // Otherwise the tests are dependent on each other.
    QString key = "Property." + QString(__FUNCTION__);
    propertyHandle = PropertyHandle::instance(key);

    // Test and expected results:
    // The info() function returns the correct ContextPropertyInfo
    QCOMPARE(propertyHandle->info(), mockContextPropertyInfo);
}

void PropertyHandleUnitTests::subscribe()
{
    // Setup:
    // Create the object to be tested
    // Note: For each test, we need to create a separate instance.
    // Otherwise the tests are dependent on each other.
    QString key = "Property." + QString(__FUNCTION__);
    propertyHandle = PropertyHandle::instance(key);

    // Test:
    // Command the PropertyHandle to subscribe
    propertyHandle->subscribe();

    // Expected results:
    // The PropertyHandle calls the Provider::subscribe
    QCOMPARE(Provider::subscribeCount, 1);
    QCOMPARE(Provider::subscribeKeys.at(0), key);
    QCOMPARE(Provider::unsubscribeCount, 0);
}

void PropertyHandleUnitTests::subscribeAndUnsubscribe()
{
    // Setup:
    // Create the object to be tested
    // Note: For each test, we need to create a separate instance.
    // Otherwise the tests are dependent on each other.
    QString key = "Property." + QString(__FUNCTION__);
    propertyHandle = PropertyHandle::instance(key);

    // Test:
    // Command the PropertyHandle to subscribe
    propertyHandle->subscribe();
    // Command the PropertyHandle to unsubscribe
    propertyHandle->unsubscribe();

    // Expected results:
    // The PropertyHandle calls the Provider::unsubscribe
    QCOMPARE(Provider::subscribeCount, 1);
    QCOMPARE(Provider::subscribeKeys.at(0), key);
    QCOMPARE(Provider::unsubscribeCount, 1);
    QCOMPARE(Provider::unsubscribeKeys.at(0), key);
}

void PropertyHandleUnitTests::subscribeTwice()
{
    // Setup:
    // Create the object to be tested
    // Note: For each test, we need to create a separate instance.
    // Otherwise the tests are dependent on each other.
    QString key = "Property." + QString(__FUNCTION__);
    propertyHandle = PropertyHandle::instance(key);

    // Test:
    // Command the PropertyHandle to subscribe
    propertyHandle->subscribe();
    // and subscribe again
    propertyHandle->subscribe();

    // Expected results:
    // The PropertyHandle calls the Provider::subscribe
    // but only once.
    QCOMPARE(Provider::subscribeCount, 1);
    QCOMPARE(Provider::subscribeKeys.at(0), key);
    QCOMPARE(Provider::unsubscribeCount, 0);
}

void PropertyHandleUnitTests::subscriptionPendingAndFinished()
{
    // Setup:
    // Create the object to be tested
    // Note: For each test, we need to create a separate instance.
    // Otherwise the tests are dependent on each other.
    QString key = "Property." + QString(__FUNCTION__);
    propertyHandle = PropertyHandle::instance(key);

    // Test:
    // Command the PropertyHandle to subscribe
    propertyHandle->subscribe();

    // Expected results:
    // The subscription is marked as pending
    QVERIFY(propertyHandle->isSubscribePending());

    // finished
    propertyHandle->setSubscribeFinished(mockProvider);

    // Expected results:
    // The subscription is no longer marked as pending
    QCOMPARE(propertyHandle->isSubscribePending(), false);
}

void PropertyHandleUnitTests::subscribeTwiceAndUnsubscribe()
{
    // Setup:
    // Create the object to be tested
    // Note: For each test, we need to create a separate instance.
    // Otherwise the tests are dependent on each other.
    QString key = "Property." + QString(__FUNCTION__);
    propertyHandle = PropertyHandle::instance(key);

    // Test:
    // Command the PropertyHandle to subscribe
    propertyHandle->subscribe();
    // and subscribe again
    propertyHandle->subscribe();
    // and then unsubscribe.
    propertyHandle->unsubscribe();

    // Expected results:
    // The PropertyHandle calls the Provider::subscribe
    // but only once. The unsubscribe is not called, since one of the
    // subscriptions is still valid.
    QCOMPARE(Provider::subscribeCount, 1);
    QCOMPARE(Provider::subscribeKeys.at(0), key);
    QCOMPARE(Provider::unsubscribeCount, 0);
}

void PropertyHandleUnitTests::subscribeTwiceAndUnsubscribeTwice()
{
    // Setup:
    // Create the object to be tested
    // Note: For each test, we need to create a separate instance.
    // Otherwise the tests are dependent on each other.
    QString key = "Property." + QString(__FUNCTION__);
    propertyHandle = PropertyHandle::instance(key);

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
    // The PropertyHandle calls the Provider::subscribe
    // but only once. Also the unsubscription is called once.
    QCOMPARE(Provider::subscribeCount, 1);
    QCOMPARE(Provider::subscribeKeys.at(0), key);
    QCOMPARE(Provider::unsubscribeCount, 1);
    QCOMPARE(Provider::unsubscribeKeys.at(0), key);
}

void PropertyHandleUnitTests::onValueChangedWithoutTypeCheck()
{
    // Setup:
    // Create the object to be tested
    // Note: For each test, we need to create a separate instance.
    // Otherwise the tests are dependent on each other.
    QString key = "Property." + QString(__FUNCTION__);
    propertyHandle = PropertyHandle::instance(key);

    // Start listening to the valueChanged signal
    QSignalSpy spy(propertyHandle, SIGNAL(valueChanged()));

    // Test:
    spy.clear();
    // Command the PropertyHandle to change its value
    // The new value is a double
    Provider::setValue(key, QVariant(1.4));

    // Expected results:
    // The valueChanged signal was emitted
    QCOMPARE(spy.count(), 1);
    // The new value is now accessible
    QCOMPARE(propertyHandle->value(), QVariant(1.4));

    // Test:
    spy.clear();
    // Command the PropertyHandle to set its value to the same value it already has
    Provider::setValue(key, QVariant(1.4));

    // Expected results:
    // The valueChanged signal is not emitted
    QCOMPARE(spy.count(), 0);

    // Test:
    spy.clear();
    // Command the PropertyHandle to change its value
    // The new value is an integer
    Provider::setValue(key, QVariant(-8));

    // Expected results:
    // The valueChanged signal was emitted
    QCOMPARE(spy.count(), 1);
    // The new value is now accessible
    QCOMPARE(propertyHandle->value(), QVariant(-8));
    QCOMPARE(propertyHandle->value().isNull(), false);

    // Test:
    spy.clear();
    // Command the PropertyHandle to set its value to the same value it already has
    Provider::setValue(key, QVariant(-8));

    // Expected results:
    // The valueChanged signal is not emitted
    QCOMPARE(spy.count(), 0);

    // Test:
    spy.clear();
    // Command the PropertyHandle to change its value
    // The new value is a string
    Provider::setValue(key, QVariant("myValue"));

    // Expected results:
    // The valueChanged signal was emitted
    QCOMPARE(spy.count(), 1);
    // The new value is now accessible
    QCOMPARE(propertyHandle->value(), QVariant("myValue"));
    QCOMPARE(propertyHandle->value().isNull(), false);

    // Test:
    spy.clear();
    // Command the PropertyHandle to set its value to the same value it already has
    Provider::setValue(key, QVariant("myValue"));

    // Expected results:
    // The valueChanged signal is not emitted
    QCOMPARE(spy.count(), 0);

    // Test:
    spy.clear();
    // Command the PropertyHandle to change its value
    // The new value is a boolean
    Provider::setValue(key, QVariant(true));

    // Expected results:
    // The valueChanged signal was emitted
    QCOMPARE(spy.count(), 1);
    // The new value is now accessible
    QCOMPARE(propertyHandle->value(), QVariant(true));
    QCOMPARE(propertyHandle->value().isNull(), false);

    // Test:
    spy.clear();
    // Command the PropertyHandle to set its value to the same value it already has
    Provider::setValue(key, QVariant(true));

    // Expected results:
    // The valueChanged signal is not emitted
    QCOMPARE(spy.count(), 0);

    // Test:
    spy.clear();
    // Command the PropertyHandle to change its value
    // The new value is a null
    Provider::setValue(key, QVariant());

    // Expected results:
    // The valueChanged signal was emitted
    QCOMPARE(spy.count(), 1);
    // The new value is now accessible
    QCOMPARE(propertyHandle->value(), QVariant());
    QCOMPARE(propertyHandle->value().isNull(), true);

    // Test:
    spy.clear();
    // Command the PropertyHandle to set its value to the same value it already has
    Provider::setValue(key, QVariant());

    // Expected results:
    // The valueChanged signal is not emitted
    QCOMPARE(spy.count(), 0);

}

void PropertyHandleUnitTests::onValueChangedWithTypeCheckAndCorrectTypes()
{
    // Setup:
    // Create the object to be tested
    // Note: For each test, we need to create a separate instance.
    // Otherwise the tests are dependent on each other.
    QString key = "Property." + QString(__FUNCTION__);
    propertyHandle = PropertyHandle::instance(key);

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
    Provider::setValue(key, QVariant(1.4));

    // Expected results:
    // The valueChanged signal was emitted
    QCOMPARE(spy.count(), 1);
    // The new value is now accessible
    QCOMPARE(propertyHandle->value(), QVariant(1.4));

    // Test:
    spy.clear();
    // Command the PropertyHandle to change its value
    // The new value is a null
    Provider::setValue(key, QVariant());

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
    Provider::setValue(key, QVariant(-8));

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
    Provider::setValue(key, QVariant());

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
    Provider::setValue(key, QVariant("myValue"));

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
    Provider::setValue(key, QVariant());

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
    Provider::setValue(key, QVariant(true));

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
    Provider::setValue(key, QVariant());

    // Expected results:
    // The NULL value is always accepcted (not depending on the type)
    // The valueChanged signal was emitted
    QCOMPARE(spy.count(), 1);
    // The new value is now accessible
    QCOMPARE(propertyHandle->value(), QVariant());
    QCOMPARE(propertyHandle->value().isNull(), true);

}

void PropertyHandleUnitTests::onValueChangedWithTypeCheckAndIncorrectTypes()
{
    // Setup:
    // Create the object to be tested
    // Note: For each test, we need to create a separate instance.
    // Otherwise the tests are dependent on each other.
    QString key = "Property." + QString(__FUNCTION__);
    propertyHandle = PropertyHandle::instance(key);

    // Enable the type checks
    PropertyHandle::setTypeCheck(true);

    // Start listening to the valueChanged signal
    QSignalSpy spy(propertyHandle, SIGNAL(valueChanged()));

    // Setup:
    // Set the type to DOUBLE
    mockContextPropertyInfo->myType = "DOUBLE";
    // Set an initial value
    Provider::setValue(key, QVariant(4.2));
    spy.clear();

    // Test:
    // Command the PropertyHandle to change its value multiple times but with
    // incorrect types.
    Provider::setValue(key, QVariant(5));
    Provider::setValue(key, QVariant("string"));
    Provider::setValue(key, QVariant(false));

    // Expected results:
    // The valueChanged signal was not emitted
    QCOMPARE(spy.count(), 0);
    // The value is still the old value
    QCOMPARE(propertyHandle->value(), QVariant(4.2));

    // Setup:
    // Set the type to INT
    mockContextPropertyInfo->myType = "INT";
    // Set an initial value
    Provider::setValue(key, QVariant(22));
    spy.clear();

    // Test:
    // Command the PropertyHandle to change its value multiple times but with
    // incorrect types.
    Provider::setValue(key, QVariant(5.6));
    Provider::setValue(key, QVariant("string"));
    Provider::setValue(key, QVariant(false));

    // Expected results:
    // The valueChanged signal was not emitted
    QCOMPARE(spy.count(), 0);
    // The value is still the old value
    QCOMPARE(propertyHandle->value(), QVariant(22));

    // Setup:
    // Set the type to STRING
    mockContextPropertyInfo->myType = "STRING";
    // Set an initial value
    Provider::setValue(key, QVariant("myString"));
    spy.clear();

    // Test:
    // Command the PropertyHandle to change its value multiple times but with
    // incorrect types.
    Provider::setValue(key, QVariant(5.4));
    Provider::setValue(key, QVariant(-8));
    Provider::setValue(key, QVariant(false));

    // Expected results:
    // The valueChanged signal was not emitted
    QCOMPARE(spy.count(), 0);
    // The value is still the old value
    QCOMPARE(propertyHandle->value(), QVariant("myString"));

    // Setup:
    // Set the type to TRUTH
    mockContextPropertyInfo->myType = "TRUTH";
    // Set an initial value
    Provider::setValue(key, QVariant(false));
    spy.clear();

    // Test:
    // Command the PropertyHandle to change its value multiple times but with
    // incorrect types.
    Provider::setValue(key, QVariant(5.4));
    Provider::setValue(key, QVariant(-8));
    Provider::setValue(key, QVariant("string"));

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
    // Note: For each test, we need to create a separate instance.
    // Otherwise the tests are dependent on each other.
    QString key = "Property." + QString(__FUNCTION__);
    propertyHandle = PropertyHandle::instance(key);

    // Subscribe to the PropertyHandle
    propertyHandle->subscribe();

    // Clear the logs from the subscription
    Provider::resetLogs();

    // Test:
    // Command the DBusNameListener to tell that the Commander has appeared
    mockDBusNameListener->servicePresent = DBusNameListener::Present;
    emit mockDBusNameListener->nameAppeared();

    // Expected results:
    // The handle unsubscribes from the real provider
    QCOMPARE(Provider::unsubscribeCount, 1);
    QCOMPARE(Provider::unsubscribeKeys.at(0), key);
    QCOMPARE(Provider::unsubscribeProviderNames.at(0), QString("NormalProvider"));
    // And subscribes to Commander
    QCOMPARE(Provider::subscribeCount, 1);
    QCOMPARE(Provider::subscribeKeys.at(0), key);
    QCOMPARE(Provider::subscribeProviderNames.at(0), QString("Commander"));

    // Setup:
    // Clear the logs from the subscription
    Provider::resetLogs();

    // Test:
    // Command the DBusNameListener to tell that the Commander has disappeared
    mockDBusNameListener->servicePresent = DBusNameListener::NotPresent;
    emit mockDBusNameListener->nameDisappeared();

    // Expected results:
    // The PropertyHandle unsubscribes from the commander
    QCOMPARE(Provider::unsubscribeCount, 1);
    QCOMPARE(Provider::unsubscribeKeys.at(0), key);
    QCOMPARE(Provider::unsubscribeProviderNames.at(0), QString("Commander"));
    // And subscribes to the real provider
    QCOMPARE(Provider::subscribeCount, 1);
    QCOMPARE(Provider::subscribeKeys.at(0), key);
    QCOMPARE(Provider::subscribeProviderNames.at(0), QString("NormalProvider"));
}

void PropertyHandleUnitTests::commandingDisabled()
{
    // Setup:
    // Create the object to be tested
    // Note: For each test, we need to create a separate instance.
    // Otherwise the tests are dependent on each other.
    QString key = "Property." + QString(__FUNCTION__);
    propertyHandle = PropertyHandle::instance(key);

    // Subscribe to the handle
    propertyHandle->subscribe();

    // Clear the logs from the subscription
    Provider::resetLogs();

    // Test:
    // Disable commanding
    propertyHandle->ignoreCommander();

    // Command the DBusNameListener to tell that the Commander has appeared
    mockDBusNameListener->servicePresent = DBusNameListener::Present;
    emit mockDBusNameListener->nameAppeared();

    // Expected results:
    // The PropertyHandle unsubscribes from and resubscribes to the same key,
    // practically ignoring commander appearance.
    QCOMPARE(Provider::unsubscribeCount, 1);
    QCOMPARE(Provider::subscribeCount, 1);
    QVERIFY(Provider::subscribeKeys == Provider::unsubscribeKeys);
    QVERIFY(Provider::subscribeProviderNames == Provider::unsubscribeProviderNames);

    // Setup:
    // Clear the logs from the subscription
    Provider::resetLogs();

    // Test:
    // Command the DBusNameListener to tell that the Commander has disappeared
    mockDBusNameListener->servicePresent = DBusNameListener::NotPresent;
    emit mockDBusNameListener->nameDisappeared();

    // Expected results:
    // Same thing as above.
    QCOMPARE(Provider::unsubscribeCount, 1);
    QCOMPARE(Provider::subscribeCount, 1);
    QVERIFY(Provider::subscribeKeys == Provider::unsubscribeKeys);
    QVERIFY(Provider::subscribeProviderNames == Provider::unsubscribeProviderNames);
}

} // end namespace

QTEST_MAIN(ContextSubscriber::PropertyHandleUnitTests);
