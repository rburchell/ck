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
QList<Context*> contexts;
BoolSysFsPooler *lowWatermark;
BoolSysFsPooler *highWatermark;

/* Helpers */

void emitFirstOn(const QString &name)
{
    foreach(Context* context, contexts) {
        if (context->getKey() == name)
            context->fakeFirst();
    }
}

void emitLastOn(const QString &name)
{
    foreach(Context* context, contexts) {
        if (context->getKey() == name)
            context->fakeLast();
    }
}

/* Mocked BoolSysFsPooler */

BoolSysFsPooler::BoolSysFsPooler(const QString &f)
{
    if (f == "/sys/kernel/low_watermark")
        lowWatermark = this;

    if (f == "/sys/kernel/high_watermark")
        highWatermark = this;
}

BoolSysFsPooler::TriState BoolSysFsPooler::getState()
{
    return TriStateUnknown;
}

void BoolSysFsPooler::setState(TriState s)
{
    state = s;
    emit stateChanged(s);
}

/* Mocked Context */

Context::Context(const QString &k, QObject *parent) : key(k)
{
    contexts.append(this);
}

Context::~Context()
{
    contexts.removeAll(this);
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

QString Context::getKey()
{
    return key;
}

void Context::fakeFirst()
{
    emit firstSubscriberAppeared(key);
}


void Context::fakeLast()
{
    emit lastSubscriberDisappeared(key);
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
