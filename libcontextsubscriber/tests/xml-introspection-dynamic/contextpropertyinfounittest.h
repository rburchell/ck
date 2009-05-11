#include <QtTest/QtTest>
#include <QtCore>
#include "contextpropertyinfo.h"

class ContextPropertyInfoUnitTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void checkKeyTypeChanging();
    void cleanupTestCase();
};

