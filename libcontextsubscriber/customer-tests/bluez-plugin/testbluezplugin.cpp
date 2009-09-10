/*
 * Copyright (C) 2009 Nokia Corporation.
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
#include "testbluezplugin.h"

// Plugin interface definition
#include "iproviderplugin.h"

// Temporary: plugin header files
#include "bluezplugin.h"

#include <QtTest/QtTest>
#include <QLibrary>
#include <QProcess>
#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QSignalSpy>
#include <QDebug>

typedef ContextSubscriber::IProviderPlugin* (*PluginFactoryFunc)(const QString& constructionString);

//
// Definition of testcases
//

// Before all tests
void BluezPluginTests::initTestCase()
{
}

// After all tests
void BluezPluginTests::cleanupTestCase()
{
}

// Before each test
void BluezPluginTests::init()
{
    library = new QLibrary("libcontextsubscriberbluez");
    bluezProcess = new QProcess();
}

// After each test
void BluezPluginTests::cleanup()
{
    if (bluezProcess) {
        bluezProcess->kill();
        bluezProcess->waitForFinished();
    }
    delete library;
    library = 0;
    delete bluezProcess;
    bluezProcess = 0;
}

// Helpers
bool BluezPluginTests::startBluez()
{
    // Run the bluez stub
    bluezProcess->start("python bluez_stub.py"); // FIXME: srcdir!
    bool ok = bluezProcess->waitForStarted();

    if (!ok) {
        qDebug() << "Cannot start process";
        return false;
    }
    sleep(3);
    // Unfortunately, we cannot guarantee that the stub process gets
    // enough time to start its D-Bus operations before the test
    // proceeds.

    // Make it create a default adapter
    QDBusConnection connection = QDBusConnection::systemBus();
    QDBusMessage message = QDBusMessage::createMethodCall ("org.bluez", "/", "org.bluez.Manager.Testing", "AddAdapter");

    QList<QVariant> argumentList;
    QVariant s("dummy-bt-address");
    argumentList.append(s);
    message.setArguments(argumentList);

    QDBusMessage reply = connection.call(message);
    if (reply.type() == QDBusMessage::ReplyMessage) {
        return true;
    }
    qDebug() << "Invalid reply:" << reply.errorMessage();

    return false;
}


// Test cases

void BluezPluginTests::loading()
{
    /*library->load();
    qDebug() << library->errorString();
    QVERIFY(library->isLoaded());
    PluginFactoryFunc f = (PluginFactoryFunc) library->resolve("bluezPluginFactory");
    QVERIFY(f);
    */
    // FIXME: implement the test when plugin loading / building is more clear
}

void BluezPluginTests::normalOperation()
{
    // Start BlueZ stub
    QVERIFY(startBluez());

    // Test: Create plugin
    ContextSubscriberBluez::BluezPlugin* bluezPlugin = new ContextSubscriberBluez::BluezPlugin();
    // FIXME: use the same way to create the plugin than libcontextsubscriber does
    QSignalSpy readySpy(bluezPlugin, SIGNAL(ready()));
    QSignalSpy subscribeFinishedSpy(bluezPlugin, SIGNAL(subscribeFinished(QString)));
    QSignalSpy subscribeFailedSpy(bluezPlugin, SIGNAL(subscribeFailed(QString, QString)));

    // Expected result: the plugin emits ready() at some point.
    QDateTime start = QDateTime::currentDateTime();
    while (readySpy.count() != 1 && start.secsTo(QDateTime::currentDateTime()) < 3) {
        QCoreApplication::processEvents();
    }
    QCOMPARE(readySpy.count(), 1);

    // Test: subscribe to keys
    QSet<QString> keys;
    keys << "Bluetooth.Visible";
    keys << "Bluetooth.Enabled";
    bluezPlugin->subscribe(keys);

    // Expected result: the plugin emits subscribeFinished for those keys
    start = QDateTime::currentDateTime();
    while (subscribeFinishedSpy.count() < 2 && start.secsTo(QDateTime::currentDateTime()) < 3) {
        QCoreApplication::processEvents();
    }
    QCOMPARE(subscribeFinishedSpy.count(), 2);
    QCOMPARE(subscribeFailedSpy.count(), 0);
}


QTEST_MAIN(BluezPluginTests);
