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
    void basicChange();
    void cleanupTestCase();
};

