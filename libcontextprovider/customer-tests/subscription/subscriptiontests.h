#include <QObject>

class QProcess;

namespace ContextProvider {

class Service;
class Property;

class SubscriptionTests : public QObject
{
    Q_OBJECT

private:
    Service* service1;
    Property* test_int;
    Property* test_double;

    Service* service2;
    Property* test_string;
    Property* test_bool;

    QProcess *client;
    bool isReadyToRead;

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void subscribeReturnValueForUnknownProperty();
    void subscribeReturnValueForKnownProperty();

    void subscriberNotifications();

    void multiSubscribe();
    void illegalUnsubscribe();

public Q_SLOTS:
    void readStandardOutput();

private:
    QString writeToClient(const char* input);

private:
    bool clientStarted;
};

} // end namespace
