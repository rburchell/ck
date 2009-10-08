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
#include "testprovider.h"

// Mock header files
#include "handlesignalrouter.h"
#include "contextkitplugin.h"

// Header file of the class to be tested
#include "provider.h"

#include <QtTest/QtTest>
#include <QDebug>

Q_DECLARE_METATYPE(QVariant);
Q_DECLARE_METATYPE(QSet<QString>);

namespace ContextSubscriber {

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

// Mock implementation of HandleSignalRouter

// singleton instance
HandleSignalRouter* mockHandleSignalRouter = new HandleSignalRouter();

HandleSignalRouter* HandleSignalRouter::instance()
{
    return mockHandleSignalRouter;
}

void HandleSignalRouter::onValueChanged(QString key)
{
}

void HandleSignalRouter::onSubscribeFinished(QString key)
{
}

// Mock implementation of QueuedInvoker


// Mock implementation of QueuedInvoker

QueuedInvoker::QueuedInvoker()
{
}

void QueuedInvoker::queueOnce(const char *method)
{
    qDebug() << "queueonce" << QString(method);
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

// Mock implementation of ContextKitPlugin
}

QMap<QString, ContextSubscriber::ContextKitPlugin*> pluginInstances;

// contextKitPluginFactory should be outside any namespace
ContextSubscriber::IProviderPlugin* contextKitPluginFactory(QString constructionString)
{
    if (!pluginInstances.contains(constructionString))
        pluginInstances[constructionString] = new ContextSubscriber::ContextKitPlugin();
    return pluginInstances[constructionString];
}

namespace ContextSubscriber {

void ContextKitPlugin::subscribe(QSet<QString> keys)
{
    subscribeRequested += keys;
}

void ContextKitPlugin::unsubscribe(QSet<QString> keys)
{
    unsubscribeRequested += keys;
}

//
// Definition of testcases
//


// Before all tests
void ProviderUnitTests::initTestCase()
{
    qRegisterMetaType<QVariant>("QVariant");
    qRegisterMetaType<QSet<QString> >("QSet<QString>");

    qInstallMsgHandler(myMessageOutput);
}

// After all tests
void ProviderUnitTests::cleanupTestCase()
{
}

// Before each test
void ProviderUnitTests::init()
{
    pluginInstances.clear();
}

// After each test
void ProviderUnitTests::cleanup()
{
}

void ProviderUnitTests::initializing()
{
    // Test:
    // Create the object to be tested
    // Note: For each test, we need to create a separate instance.
    // Otherwise the tests are dependent on each other.
    QString conStr = "session:Fake.Bus.Name." + QString(__FUNCTION__);
    Provider *provider = Provider::instance("contextkit-dbus", conStr);

    // Let the provider process the deferred events
    provider->callAllMethodsInQueue();

    // The Provider is created (as part of initTestCase)
    // Expected results:
    // Provider constructed the ManagerInterface with correct parameters
    QCOMPARE(pluginInstances.size(), 1);
    QCOMPARE(pluginInstances.keys()[0], conStr);
}

void ProviderUnitTests::pluginReadyHandled()
{
    // Test:
    // We create a new Provider with some pending subscriptions, then
    // we signal ready from the plugin.  The pending subscriptions
    // should be forwarded to the plugin then.
    QString conStr = "session:Fake.Bus.Name." + QString(__FUNCTION__);
    Provider *provider = Provider::instance("contextkit-dbus", conStr);
    provider->callAllMethodsInQueue();

    provider->subscribe("test.key1");
    provider->subscribe("test.key2");
    provider->subscribe("test.key3");
    provider->unsubscribe("test.key2");
    provider->callAllMethodsInQueue();
    QCOMPARE(pluginInstances[conStr]->subscribeRequested, QSet<QString>()); // nothing yet

    // but after ready, the pending subscriptions are requested from the plugin
    emit pluginInstances[conStr]->ready();
    provider->callAllMethodsInQueue();
    QCOMPARE(pluginInstances[conStr]->subscribeRequested, QSet<QString>() << "test.key1" << "test.key3");
}

void ProviderUnitTests::pluginFailedHandled()
{
    // same as in the ready case, but we emit failed, so the
    // subscriptions shouldn't be requested
    QString conStr = "session:Fake.Bus.Name." + QString(__FUNCTION__);
    Provider *provider = Provider::instance("contextkit-dbus", conStr);
    provider->callAllMethodsInQueue();

    provider->subscribe("test.key1");
    provider->subscribe("test.key2");
    provider->subscribe("test.key3");
    provider->unsubscribe("test.key2");
    provider->callAllMethodsInQueue();
    QCOMPARE(pluginInstances[conStr]->subscribeRequested, QSet<QString>()); // nothing yet

    // but after ready, the pending subscriptions are requested from the plugin
    emit pluginInstances[conStr]->failed("error");
    provider->callAllMethodsInQueue();
    QCOMPARE(pluginInstances[conStr]->subscribeRequested, QSet<QString>()); // we failed, no requests
}

void ProviderUnitTests::badPluginName()
{
    // Test:
    // If we pass a bad plugin name, no crashes
    QString conStr = "session:Fake.Bus.Name." + QString(__FUNCTION__);
    Provider *provider = Provider::instance("non-existent-plugin", conStr);
    provider->callAllMethodsInQueue();

    QCOMPARE(pluginInstances.size(), 0);
}

void ProviderUnitTests::unsubscribe()
{
    // Test:
    // If a key subscribed, then we unsubscribe, an unsubscribed
    // request is sent to the plugin.
    QString conStr = "session:Fake.Bus.Name." + QString(__FUNCTION__);
    Provider *provider = Provider::instance("contextkit-dbus", conStr);
    provider->callAllMethodsInQueue();
    emit pluginInstances[conStr]->ready(); // set the plugin to ready
    provider->callAllMethodsInQueue();

    provider->subscribe("test.key1");
    provider->callAllMethodsInQueue();
    provider->unsubscribe("test.key1");
    provider->callAllMethodsInQueue();
    QCOMPARE(pluginInstances[conStr]->subscribeRequested, QSet<QString>() << "test.key1");
    QCOMPARE(pluginInstances[conStr]->unsubscribeRequested, QSet<QString>() << "test.key1");

    pluginInstances[conStr]->subscribeRequested.clear();
    pluginInstances[conStr]->unsubscribeRequested.clear();

    // Test: we subscribe, then unsubscribe-subscribe without a main
    // loop between, the expected result is that we don't get
    // unsubscribed.

    provider->subscribe("test.key2");
    provider->callAllMethodsInQueue();
    provider->unsubscribe("test.key2");
    provider->subscribe("test.key2");
    provider->callAllMethodsInQueue();
    QCOMPARE(pluginInstances[conStr]->subscribeRequested, QSet<QString>() << "test.key2");
    QCOMPARE(pluginInstances[conStr]->unsubscribeRequested, QSet<QString>());
}

void ProviderUnitTests::pluginSubscriptionFinishes()
{
    // Test:
    // We subscribe to a property.  Doesn't matter if the subscription
    // succeeds or fails, the SubscribeFinished(keyname) should be emitted.

    QString conStr = "session:Fake.Bus.Name." + QString(__FUNCTION__);
    Provider *provider = Provider::instance("contextkit-dbus", conStr);
    provider->callAllMethodsInQueue();
    emit pluginInstances[conStr]->ready(); // set the plugin to ready
    provider->callAllMethodsInQueue();

    QSignalSpy spy(provider, SIGNAL(subscribeFinished(QString)));
    provider->subscribe("test.key1");
    provider->subscribe("test.key2");
    provider->callAllMethodsInQueue();
    emit pluginInstances[conStr]->subscribeFinished("test.key1");
    emit pluginInstances[conStr]->subscribeFailed("test.key2", "error");
    QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents); // signal delivery is queued
    QCOMPARE((QList<QList<QVariant> >)spy,
             QList<QList<QVariant> >()
             << (QList<QVariant>() << "test.key1")
             << (QList<QVariant>() << "test.key2"));
}

void ProviderUnitTests::pluginValueChanges()
{
    // Test:

    // We subscribe to a property.  After that if a value change is
    // signaled it has to be forwarded iff the property was subscribed
    // to.

    QString conStr = "session:Fake.Bus.Name." + QString(__FUNCTION__);
    Provider *provider = Provider::instance("contextkit-dbus", conStr);
    provider->callAllMethodsInQueue();
    emit pluginInstances[conStr]->ready(); // set the plugin to ready
    provider->callAllMethodsInQueue();

    provider->subscribe("test.key1");
    provider->callAllMethodsInQueue();
    emit pluginInstances[conStr]->subscribeFinished("test.key1");
    QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents); // signal delivery is queued

    QSignalSpy spy(provider, SIGNAL(valueChanged(QString)));
    emit pluginInstances[conStr]->valueChanged("test.key1", QVariant(42));
    emit pluginInstances[conStr]->valueChanged("test.key2", QVariant(4242));

    QCOMPARE(spy.size(), 1);
    QCOMPARE(spy.at(0).size(), 1);
    QCOMPARE(spy.at(0).at(0).value<QString>(), QString("test.key1"));
    QCOMPARE(provider->get("test.key1").value, QVariant(42));
}
} // end namespace
QTEST_MAIN(ContextSubscriber::ProviderUnitTests);
