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
#include "lowmemprovider.h"
#include "boolsysfspooler.h"

QHash<QString, QVariant> values;

/* Mocked BoolSysFsPooler */

BoolSysFsPooler::BoolSysFsPooler(const QString &f)
{
}

BoolSysFsPooler::TriState BoolSysFsPooler::getState()
{
    return TriStateUnknown;
}

/* Mocked Context */

Context::Context(const QString &k, QObject *parent) : key(k)
{
}

void Context::set(const QVariant &v)
{
    values.insert(key, v);
}

void Context::unset()
{
    values.insert(key, QVariant());
}

QVariant Context::get()
{
    if (values.contains(key))
        return values.value(key);
    else
        return QVariant();
}

/* LowMemProviderUnitTest */

class LowMemProviderUnitTest : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();
    void keys();
    void initialValues();

private:
    LowMemProvider *provider;
};

// Before each test
void LowMemProviderUnitTest::init()
{
    provider = new LowMemProvider();
    provider->initialize();
}

// After each test
void LowMemProviderUnitTest::cleanup()
{
    delete provider;
}

void LowMemProviderUnitTest::keys()
{
    QStringList keys = provider->keys();
    QVERIFY(keys.contains("System.MemoryPressure"));
}

void LowMemProviderUnitTest::initialValues()
{
    QCOMPARE(Context("System.MemoryPressure").get(), QVariant());
}

#include "lowmemproviderunittest.moc"
QTEST_MAIN(LowMemProviderUnitTest);
