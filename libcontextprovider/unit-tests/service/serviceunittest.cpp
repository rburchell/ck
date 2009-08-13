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

/* Mocked Manager */

Manager::Manager()
{
}

void Manager::addKey(const QString &key)
{
}

void Manager::setKeyValue(const QString &key, const QVariant &v)
{
}

QVariant Manager::getKeyValue(const QString &key)
{
    return QVariant();
}

/* Mocked manager adaptor */

ManagerAdaptor::ManagerAdaptor(Manager *m, QDBusConnection *c) 
{
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

private:
    Service *service;
};

// Before each test
void ServiceUnitTest::init()
{
    service = new Service(QDBusConnection::SessionBus, "test");
}

// After each test
void ServiceUnitTest::cleanup()
{
    delete service;
}

#include "serviceunittest.moc"
QTEST_MAIN(ServiceUnitTest);
