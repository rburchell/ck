#include <QtTest/QtTest>
#include <QtCore>
#include "contextregistryinfounittest.h"

int main(int argc, char **argv)
{
    setenv("CONTEXT_PROVIDERS", "./", 0);

    QCoreApplication app(argc, argv);
    
    ContextRegistryInfoUnitTest test1;
    QTest::qExec(&test1);

    return 0;
}
