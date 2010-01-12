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

#include "propertyadaptor.h" // mocked
#include "propertyprivate.h" // mocked
#include "servicebackend.h" // to be tested

#include <QtTest/QtTest>
#include <QtCore>
#include <stdlib.h>

using namespace ContextProvider;

QVariant *lastValue = NULL;
QDBusConnection *lastConnection = NULL;
PropertyPrivate* mockProperty;

// Mock implementations

PropertyAdaptor::PropertyAdaptor(PropertyPrivate*, QDBusConnection*)
{
}


void PropertyAdaptor::forgetClients()
{
}

QString PropertyAdaptor::objectPath() const
{
    return QString("/mock/object/path");
}

void PropertyPrivate::setValue(const QVariant& value)
{
    lastValue = new QVariant(value);
}

class ServiceBackendUnitTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void init();
    void sanity();
    void defaults();
    void setValue();
    void refCouting();

private:
    ServiceBackend *serviceBackend;

    friend class ContextProvider::ServiceBackend;
};

// Before each test
void ServiceBackendUnitTest::init()
{
    serviceBackend = new ServiceBackend(QDBusConnection::sessionBus(), "org.maemo.contextkit.test");
}

void ServiceBackendUnitTest::sanity()
{
    QVERIFY(serviceBackend != NULL);
}

void ServiceBackendUnitTest::defaults()
{
    QVERIFY(ServiceBackend::defaultServiceBackend == NULL);
    serviceBackend->setAsDefault();
    QVERIFY(ServiceBackend::defaultServiceBackend == serviceBackend);

    // Set another
    ServiceBackend *anotherOne = new ServiceBackend(QDBusConnection::sessionBus(), "another.com");
    anotherOne->setAsDefault();
    QVERIFY(ServiceBackend::defaultServiceBackend == serviceBackend);

    delete serviceBackend;
    QVERIFY(ServiceBackend::defaultServiceBackend == NULL);
}

void ServiceBackendUnitTest::refCouting()
{
    QCOMPARE(serviceBackend->refCount, 0);
    serviceBackend->ref();
    QCOMPARE(serviceBackend->refCount, 1);
    serviceBackend->unref();
    QCOMPARE(serviceBackend->refCount, 0);
}

void ServiceBackendUnitTest::setValue()
{
    mockProperty = new PropertyPrivate();
    qDebug() << "add prop";
    serviceBackend->addProperty("Battery.ChargeLevel", mockProperty);
    qDebug() << "set val";
    serviceBackend->setValue("Battery.ChargeLevel", 99);
    qDebug() << "done";
    QCOMPARE(lastValue->toInt(), 99);
}

#include "servicebackendunittest.moc"
QTEST_MAIN(ServiceBackendUnitTest);
