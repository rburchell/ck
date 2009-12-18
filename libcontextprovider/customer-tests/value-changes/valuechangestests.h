#include <QString>
#include <QObject>

class QProcess;

namespace ContextProvider {

class Property;
class Service;

class ValueChangesTests : public QObject
{
    Q_OBJECT

private:
    Service* service;
    Property* test_int;
    Property* test_double;

    QProcess *client;
    bool isReadyToRead;
    bool clientStarted;

    QString writeToClient(const char* input);

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void subscribedPropertyChanges();
    void nonsubscribedPropertyChanges();
    void unsubscribedPropertyChanges();

    void sameValueSet();

    void changesBetweenZeroAndUnknown();

public Q_SLOTS:
    void readStandardOutput();
};

} // end namespace
