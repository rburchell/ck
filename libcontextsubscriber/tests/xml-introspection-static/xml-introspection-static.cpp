#include <QtTest/QtTest>
#include <QtCore>
#include "contextpropertyinfounittest.h"
#include "contextregistryinfounittest.h"

int main(int argc, char **argv)
{
    setenv("CONTEXT_PROVIDERS", "./", 0);

    QCoreApplication app(argc, argv);
    
    ContextRegistryInfoUnitTest test1;
    QTest::qExec(&test1);

    ContextPropertyInfoUnitTest test2;
    QTest::qExec(&test2);

    return 0;
}
