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
#include "service.h"
#include "property.h"
#include "manageradaptor.h"

using namespace ContextProvider;

QString *lastKey = NULL;
QVariant *lastValue = NULL;
Manager *lastManager = NULL;
QDBusConnection *lastConnection = NULL;

/* Mocked Manager */

Manager::Manager()
{
}

void Manager::addKey(const QString &key)
{
}

void Manager::setKeyValue(const QString &key, const QVariant &v)
{
    lastValue = new QVariant(v);
    lastKey = new QString(key);
}

QVariant Manager::getKeyValue(const QString &key)
{
    return QVariant();
}

/* Mocked manager adaptor */

ManagerAdaptor::ManagerAdaptor(Manager *m, QDBusConnection *c) 
{
    lastManager = m;
    lastConnection = c;
}

/* Mocked Property */

void Property::setManager(Manager *)
{
}

QString Property::key()
{
    return "XXX";
}

/* Service unit test */

class ServiceUnitTest : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();
    void sanityCheck();
    void defaults();
    void setValue();
    void start();
    void restart();

private:
    Service *service;

    friend class ContextProvider::Service;
};

// Before each test
void ServiceUnitTest::init()
{
    service = new Service(QDBusConnection::SessionBus, "test.com");
}

// After each test
void ServiceUnitTest::cleanup()
{
    delete service;

    lastKey = NULL;
    lastValue = NULL;
    lastManager = NULL;
    lastConnection = NULL;
}

void ServiceUnitTest::sanityCheck()
{
    QVERIFY(service->manager() != NULL);
}

void ServiceUnitTest::defaults()
{
    QVERIFY(service->defaultService == NULL);
    service->setAsDefault();
    QVERIFY(service->defaultService == service);

    Service *otherService = new Service(QDBusConnection::SessionBus, "test2");
    otherService->setAsDefault();
    QVERIFY(service->defaultService == service);
    delete otherService;
}

void ServiceUnitTest::setValue()
{
    service->setValue("Battery.ChargeLevel", 99);
    QCOMPARE(*lastKey, QString("Battery.ChargeLevel"));
    QCOMPARE(lastValue->toInt(), 99);
}

void ServiceUnitTest::start()
{
    QVERIFY(lastManager == NULL);
    QVERIFY(lastConnection == NULL);

    QCOMPARE(service->start(), true);
    QVERIFY(lastManager);
    QVERIFY(lastConnection);

    QCOMPARE(service->start(), false);
    QVERIFY(lastManager);
    QVERIFY(lastConnection);
}

void ServiceUnitTest::restart()
{
    QVERIFY(lastManager == NULL);
    QVERIFY(lastConnection == NULL);

    QCOMPARE(service->start(), true);
    QVERIFY(lastManager);
    QVERIFY(lastConnection);

    Manager *cachedManager;
    QDBusConnection *cachedConnection;

    service->restart();
    QVERIFY(lastManager);
    QVERIFY(lastConnection);
    QVERIFY(lastManager != cachedManager);
    QVERIFY(lastConnection != cachedConnection);
}

#include "serviceunittest.moc"
QTEST_MAIN(ServiceUnitTest);
