#ifndef TESTCLASS_H
#define TESTCLASS_H

#include <QObject>

class TestClass : public QObject
{
    Q_OBJECT

public:
    TestClass() : QObject() {}; 
    bool testMe(void);
};

#endif // TESTCLASS_H
