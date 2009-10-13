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
#include "infobackend.h"

class InfoTestBackend : public InfoBackend
{
public:

    QString name() const
    {
        return "test";
    }

    QStringList listKeys() const
    {
        return QStringList();
    }

    QStringList listKeysForPlugin(QString plugin) const
    {
        return QStringList();
    }

    QStringList listPlugins() const
    {
        return QStringList();
    }

    QString typeForKey(QString key) const
    {
        return QString();
    }

    QString docForKey(QString key) const
    {
        return QString();
    }

    bool keyDeclared(QString key) const
    {
        return false;
    }

    bool keyProvided(QString key) const
    {
        return false;
    }

    const QList<ContextProviderInfo> listProviders(QString key) const
    {
        return QList<ContextProviderInfo> ();
    }
};

class InfoBackendUnitTest : public QObject
{
    Q_OBJECT

    InfoBackend *backend;

private slots:
    void initTestCase();
    void checkAndEmitKeysAdded();
    void checkAndEmitKeysRemoved();
    void checkAndEmitKeyChanged();
    void connectNotify();
    void instance();
    void cleanupTestCase();
};

void InfoBackendUnitTest::initTestCase()
{
    backend = new InfoTestBackend();
    InfoBackend::backendInstance = backend;
}

void InfoBackendUnitTest::checkAndEmitKeysAdded()
{
    QSignalSpy spy(backend, SIGNAL(keysAdded(QStringList)));

    QStringList currentKeys;
    QStringList oldKeys;

    currentKeys << "Key.One";
    currentKeys << "Key.Two";
    oldKeys << "Key.One";

    backend->checkAndEmitKeysAdded(currentKeys, oldKeys);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();
    QCOMPARE(args.at(0).toList().size(), 1);
    QCOMPARE(args.at(0).toStringList().at(0), QString("Key.Two"));
}

void InfoBackendUnitTest::checkAndEmitKeysRemoved()
{
    QSignalSpy spy(backend, SIGNAL(keysRemoved(QStringList)));

    QStringList currentKeys;
    QStringList oldKeys;

    currentKeys << "Key.One";
    oldKeys << "Key.Two";
    oldKeys << "Key.One";

    backend->checkAndEmitKeysRemoved(currentKeys, oldKeys);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();
    QCOMPARE(args.at(0).toList().size(), 1);
    QCOMPARE(args.at(0).toStringList().at(0), QString("Key.Two"));
}

void InfoBackendUnitTest::checkAndEmitKeyChanged()
{
    QSignalSpy spy(backend, SIGNAL(keyChanged(QString)));

    QStringList currentKeys;
    QStringList oldKeys;

    currentKeys << "Key.One";
    currentKeys << "Key.Three";
    oldKeys << "Key.Two";
    oldKeys << "Key.One";

    backend->checkAndEmitKeyChanged(currentKeys, oldKeys);

    QCOMPARE(spy.count(), 3);
}

void InfoBackendUnitTest::connectNotify()
{
    QCOMPARE(backend->connectCount, 0);
    backend->connectNotify("-");
    QCOMPARE(backend->connectCount, 1);
    backend->disconnectNotify("-");
    QCOMPARE(backend->connectCount, 0);
}

void InfoBackendUnitTest::instance()
{
    InfoBackend::destroyInstance();
    backend = NULL;

    InfoBackend *instance;

    InfoBackend::backendInstance = NULL;
    instance = InfoBackend::instance("xml");
    QCOMPARE(instance, InfoBackend::backendInstance);
    QCOMPARE(instance->name(), QString("xml"));
    InfoBackend::destroyInstance();
    QVERIFY(InfoBackend::backendInstance == NULL);

    InfoBackend::backendInstance = NULL;
    instance = InfoBackend::instance("cdb");
    QCOMPARE(instance, InfoBackend::backendInstance);
    QCOMPARE(instance->name(), QString("cdb"));
    InfoBackend::destroyInstance();
    QVERIFY(InfoBackend::backendInstance == NULL);

    InfoBackend::backendInstance = new InfoTestBackend();
    InfoBackend::destroyInstance();
    QVERIFY(InfoBackend::backendInstance == NULL);
}

void InfoBackendUnitTest::cleanupTestCase()
{
    QVERIFY(InfoBackend::backendInstance == NULL);
    InfoBackend::destroyInstance();
}

#include "infobackendunittest.moc"
QTEST_MAIN(InfoBackendUnitTest);
