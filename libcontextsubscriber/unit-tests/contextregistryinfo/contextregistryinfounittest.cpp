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
#include "contextregistryinfo.h"
#include "fileutils.h"
#include "infobackend.h"

/* Mocked infobackend */

InfoBackend* InfoBackend::instance(const QString &backendName)
{
    return new InfoBackend();
}

QString InfoBackend::name() const
{
    return "test";
}

QStringList InfoBackend::listKeys() const
{
    QStringList l;
    l << QString("Battery.Charging");
    l << QString("Battery.Capacity");
    return l;
}

QStringList InfoBackend::listKeysForPlugin(QString plugin) const
{
    return QStringList();
}

QStringList InfoBackend::listPlugins() const
{
    return QStringList();
}

QString InfoBackend::constructionStringForKey(QString key) const
{
    return QString();
}

/* ContextRegistryInfoUnitTest */

class ContextRegistryInfoUnitTest : public QObject
{
    Q_OBJECT
    ContextRegistryInfo *registry;

private:
    ContextRegistryInfo *context;

private slots:
    void initTestCase();
    void listKeys();
};

void ContextRegistryInfoUnitTest::initTestCase()
{
    registry = ContextRegistryInfo::instance();
}

void ContextRegistryInfoUnitTest::listKeys()
{
    QStringList keys = registry->listKeys();
    QCOMPARE(keys.count(), 2);
    QVERIFY(keys.contains("Battery.Charging"));
    QVERIFY(keys.contains("Battery.Capacity"));
}

#include "contextregistryinfounittest.moc"
QTEST_MAIN(ContextRegistryInfoUnitTest);
