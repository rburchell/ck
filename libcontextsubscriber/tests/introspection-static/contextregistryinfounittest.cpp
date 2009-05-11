#include <QtTest/QtTest>
#include <QtCore>
#include "contextregistryinfo.h"

class ContextRegistryInfoUnitTest : public QObject
{
    Q_OBJECT

private:
    ContextRegistryInfo *context;

private slots:
    void initTestCase();
    void listKeys();
    void listKeysForProvider();
    void listProviders();

};

void ContextRegistryInfoUnitTest::initTestCase()
{
    setenv("CONTEXT_PROVIDERS", "./", 0);

    context = ContextRegistryInfo::instance();
    QVERIFY(context != NULL);
}

void ContextRegistryInfoUnitTest::listKeys()
{
    QList <QString> list = context->listKeys();
    QCOMPARE(list.size(), 3);

    QList <QString> expectedKeys;
    expectedKeys << "Context.Battery.ChargePercentage";
    expectedKeys << "Context.Battery.LowBattery";
    expectedKeys << "Context.Battery.Charging";

    foreach (QString key, list) {
        if (expectedKeys.contains(key))
            expectedKeys.removeAll(key);
    }

    QCOMPARE(expectedKeys.size(), 0);
}

void ContextRegistryInfoUnitTest::listKeysForProvider()
{
    QList <QString> list = context->listKeys("org.freedesktop.ContextKit.contextd1");
    QCOMPARE(list.size(), 2);

    QList <QString> expectedKeys;
    expectedKeys << "Context.Battery.ChargePercentage";
    expectedKeys << "Context.Battery.LowBattery";

    foreach (QString key, list) {
        if (expectedKeys.contains(key))
            expectedKeys.removeAll(key);
    }

    QCOMPARE(expectedKeys.size(), 0);
    QCOMPARE(context->listKeys("Something.that.doesnt.exist").size(), 0);
}

void ContextRegistryInfoUnitTest::listProviders()
{
    QList <QString> list = context->listProviders();
    QCOMPARE(list.size(), 2);

    QList <QString> expectedProviders;
    expectedProviders << "org.freedesktop.ContextKit.contextd1";
    expectedProviders << "org.freedesktop.ContextKit.contextd2";

    foreach (QString key, list) {
        if (expectedProviders.contains(key))
            expectedProviders.removeAll(key);
    }

    QCOMPARE(expectedProviders.size(), 0);
}

QTEST_MAIN(ContextRegistryInfoUnitTest);
#include "moc_contextregistryinfounittest_cpp.cpp"
