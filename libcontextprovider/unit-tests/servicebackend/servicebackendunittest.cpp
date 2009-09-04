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
#include "servicebackend.h"
#include "manageradaptor.h"

using namespace ContextProvider;

QString *lastKey = NULL;
QVariant *lastValue = NULL;
Manager *lastManager = NULL;
QDBusConnection *lastConnection = NULL;

/* Mocked manager adaptor */

ManagerAdaptor::ManagerAdaptor(Manager *m, QDBusConnection *c)
{
    lastManager = m;
    lastConnection = c;
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



class ServiceBackendUnitTest : public QObject
{
    Q_OBJECT

private slots:
    void init();

private:
    ServiceBackend *serviceBackend;

    friend class ContextProvider::ServiceBackend;
};

// Before each test
void ServiceBackendUnitTest::init()
{
    serviceBackend = new ServiceBackend(QDBusConnection::SessionBus, "test.com");
}

#include "servicebackendunittest.moc"
QTEST_MAIN(ServiceBackendUnitTest);
