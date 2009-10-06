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
#include "servicebackend.h"
#include "property.h"
#include "manageradaptor.h"

using namespace ContextProvider;

#define STATE_UNDEFINED 0
#define STATE_STOPPED 1
#define STATE_STARTED 2
#define STATE_RESTARTED 3

int lastState = STATE_UNDEFINED;

QString *lastKey = NULL;
QVariant *lastValue = NULL;
Manager *lastManager = NULL;
QDBusConnection *lastConnection = NULL;

/* Mocked ServiceBackend */

ServiceBackend* ServiceBackend::defaultServiceBackend = NULL;

ServiceBackend::ServiceBackend(QDBusConnection connection, const QString &busName) :
    connection(connection)
{
    lastState = STATE_UNDEFINED;
}

Manager* ServiceBackend::manager()
{
    return new Manager();
}

void ServiceBackend::setAsDefault()
{
    defaultServiceBackend = this;
}

void ServiceBackend::ref()
{
}

void ServiceBackend::unref()
{
}

bool ServiceBackend::start()
{
    if (lastState <= STATE_STOPPED) {
        lastState = STATE_STARTED;
        return true;
    } else
        return false;
}

void ServiceBackend::stop()
{
    lastState = STATE_STOPPED;
}

bool ServiceBackend::sharedConnection()
{
    return false;
}

ServiceBackend* ServiceBackend::instance(QDBusConnection connection)
{
    return new ServiceBackend(connection);
}

ServiceBackend* ServiceBackend::instance(QDBusConnection::BusType busType,
                                         const QString &busName, bool autoStart)
{
    ServiceBackend *r = new ServiceBackend(QDBusConnection::sessionBus(), busName);
    if (autoStart)
        r->start();

    return r;
}

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
    void setConnection();

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
    QVERIFY(service->backend != NULL);
}

void ServiceUnitTest::defaults()
{
    QVERIFY(service->backend->defaultServiceBackend == NULL);
    service->setAsDefault();
    QVERIFY(service->backend->defaultServiceBackend == service->backend);

    Service *otherService = new Service(QDBusConnection::SessionBus, "test2");
    otherService->setAsDefault();
    QVERIFY(service->backend->defaultServiceBackend == otherService->backend);
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
    QVERIFY(lastState == STATE_STARTED);

    service->stop();
    QCOMPARE(lastState, STATE_STOPPED);

    QCOMPARE(service->start(), true);
    QCOMPARE(lastState, STATE_STARTED);

    QCOMPARE(service->start(), false);
    QCOMPARE(lastState, STATE_STARTED);

    service->restart();
    QCOMPARE(lastState, STATE_STARTED);
}

void ServiceUnitTest::setConnection()
{
    QCOMPARE(service->backend->connection.name(), QString("qt_default_session_bus"));
    QDBusConnection conn = QDBusConnection::connectToBus(QDBusConnection::SessionBus, "test_bus_name");
    service->setConnection(conn);
    QCOMPARE(service->backend->connection.name(), QString("test_bus_name"));
}


#include "serviceunittest.moc"
QTEST_MAIN(ServiceUnitTest);
