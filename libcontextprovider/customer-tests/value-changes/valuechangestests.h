#include <QString>
#include <QObject>

#include "context.h"

class QProcess;

namespace ContextProvider {

class Property;

class ValueChangesTests : public QObject
{
    Q_OBJECT

private:
    Service service1;
    Property test_int;
    Property test_double;

    Service service2;
    Property test_string;
    Property test_bool;

    QProcess *client;
    bool isReadyToRead;
    bool clientStarted;

    QString writeToClient(const char* input);

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void subscribedPropertyChanges();
    void nonsubscribedPropertyChanges();
    void unsubscribedPropertyChanges();

    void twoPropertiesChange();
    void sameValueSet();

public slots:
    void readStandardOutput();

public:
    ValueChangesTests();
};

} // end namespace
