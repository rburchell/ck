#include <QtTest/QtTest>
#include <QtCore>
#include "contextregistryinfo.h"

class ContextRegistryInfoUnitTest : public QObject
{
    Q_OBJECT

private slots:
    void listKeys();
};

void ContextRegistryInfoUnitTest::listKeys()
{
    setenv("CONTEXT_PROVIDERS", "./", 0);

    ContextRegistryInfo *context = ContextRegistryInfo::instance();
    QVERIFY(context != NULL);

    QList <QString> list = context->listKeys();
    QVERIFY(list.size() == 3);

    QList <QString> expectedProviders;
    expectedProviders << "Context.Battery.ChargePercentage";
    expectedProviders << "Context.Battery.LowBattery";
    expectedProviders << "Context.Battery.Charging";

    foreach (QString key, list) {
        if (expectedProviders.contains(key))
            expectedProviders.removeAll(key);
    }

    QVERIFY(expectedProviders.size() == 0);
}

QTEST_MAIN(ContextRegistryInfoUnitTest);
#include "moc_contextregistryinfounittest_cpp.cpp"
