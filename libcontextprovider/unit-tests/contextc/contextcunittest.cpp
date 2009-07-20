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
#include "context.h"
#include "contextc.h"

QString *lastBusName = NULL;
QStringList *lastKeysList = NULL;
QDBusConnection::BusType lastConnectionType;
QVariant *lastVariantSet = NULL;

/* Mocked implementation of ContextC */

void resetVariants()
{
    delete lastVariantSet;
    lastVariantSet = NULL;
}

bool Context::initService(QDBusConnection::BusType busType, const QString &busName, const QStringList &keys)
{
    delete lastBusName;
    lastBusName = new QString(busName);

    delete lastKeysList;
    lastKeysList = new QStringList(keys);

    lastConnectionType = busType;
    return true;
}

void Context::set(const QVariant &v)
{
    delete lastVariantSet;
    lastVariantSet = new QVariant(v);
}

void Context::unset()
{
    delete lastVariantSet;
    lastVariantSet = NULL;
}

QVariant Context::get()
{
    if (! lastVariantSet)
        return QVariant();
    else
        return QVariant(*lastVariantSet);
}

bool Context::isValid() const
{
    return (lastKeysList->contains(key));
}

bool Context::isSet() const
{
    if (lastVariantSet == NULL)
        return false;
    else
        return (*lastVariantSet != QVariant());
}

QString Context::getKey() const
{
    return key;
}

Context::Context(const QString &name, QObject *obj) : QObject(obj), key(name)
{
    delete lastVariantSet;
    lastVariantSet = NULL;
}

class ContextCUnitTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void isValid();
    void isSet();
    void unset();
    void getKey();
    void setGetInt();
    void setGetBool();
    void setGetString();
    void setGetDouble();
};

// Before all tests
void ContextCUnitTest::initTestCase()
{
    const char* keys[1];
    keys[0] = "Battery.OnBattery";

    QCOMPARE(context_init_service(0, "com.test.provider", keys, 1), 1);
    QCOMPARE(*lastBusName, QString("com.test.provider"));
    QCOMPARE(lastKeysList->size(), 1);
    QVERIFY(lastKeysList->contains("Battery.OnBattery"));
    QCOMPARE(lastConnectionType, QDBusConnection::SessionBus);
}

void ContextCUnitTest::isValid()
{
    ContextPtr *c1 = context_new("Battery.OnBattery");
    QCOMPARE(context_is_valid(c1), 1);
    context_free(c1);

    ContextPtr *c2 = context_new("Battery.SomethingWeird");
    QCOMPARE(context_is_valid(c2), 0);
    context_free(c2);
}

void ContextCUnitTest::isSet()
{
    resetVariants();
    ContextPtr *c = context_new("Battery.OnBattery");
    QCOMPARE(context_is_set(c), 0);
    context_set_int(c, 999);
    QCOMPARE(context_is_set(c), 1);
    context_free(c);
}

void ContextCUnitTest::unset()
{
    resetVariants();
    ContextPtr *c = context_new("Battery.OnBattery");
    QCOMPARE(context_is_set(c), 0);
    context_set_int(c, 999);
    QCOMPARE(context_is_set(c), 1);
    context_unset(c);
    QCOMPARE(context_is_set(c), 0);
    context_free(c);
}

void ContextCUnitTest::getKey()
{
    ContextPtr *c = context_new("Battery.OnBattery");
    char *k = context_get_key(c);
    QVERIFY(strcmp(k, "Battery.OnBattery") == 0);
    context_free(c);
}

void ContextCUnitTest::setGetInt()
{
    int v;
    ContextPtr *c = context_new("Battery.OnBattery");
    QCOMPARE(context_get_int(c, &v), 0);
    QCOMPARE(v, 0);
    
    context_set_int(c, 666);
    QCOMPARE(*lastVariantSet, QVariant(666));
    
    QCOMPARE(context_get_int(c, &v), 1);
    QCOMPARE(v, 666);
    
    context_free(c);
}

void ContextCUnitTest::setGetDouble()
{
    double v;
    ContextPtr *c = context_new("Battery.OnBattery");
    QCOMPARE(context_get_double(c, &v), 0);
    QCOMPARE(v, 0.0);
    
    context_set_double(c, 0.666);
    QCOMPARE(*lastVariantSet, QVariant(0.666));

    QCOMPARE(context_get_double(c, &v), 1);
    QCOMPARE(v, 0.666);

    context_free(c);
}

void ContextCUnitTest::setGetString()
{
    char *v;
    ContextPtr *c = context_new("Battery.OnBattery");
    QCOMPARE(context_get_string(c, &v), 0);
    QVERIFY(v == NULL);

    context_set_string(c, "Hello!");
    QCOMPARE(*lastVariantSet, QVariant(QString("Hello!")));

    QCOMPARE(context_get_string(c, &v), 1);
    QVERIFY(v != NULL);
    QVERIFY(strcmp(v, "Hello!") == 0);

    free(v);
    context_free(c);
}

void ContextCUnitTest::setGetBool()
{
    int v;
    ContextPtr *c = context_new("Battery.OnBattery");
    QCOMPARE(context_get_bool(c, &v), 0);
    QCOMPARE(v, 0);
    
    context_set_bool(c, 1);
    QCOMPARE(*lastVariantSet, QVariant(true));
    
    QCOMPARE(context_get_bool(c, &v), 1);
    QCOMPARE(v, 1);
    
    context_free(c);
}

#include "contextcunittest.moc"
QTEST_MAIN(ContextCUnitTest);
