#include <QString>
#include <QObject>

class QProcess;

namespace ContextProvider {

class Property;

class ValueChangesTests : public QObject
{
    Q_OBJECT

private:
    Property *intItem;
    Property *boolItem;
    Property *doubleItem;
    Property *stringItem;
    QProcess *client;
    bool isReadyToRead;

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void subscribedPropertyChanges();
    void nonsubscribedPropertyChanges();
    void unsubscribedPropertyChanges();

public slots:
    void readStandardOutput();

private:
    QString writeToClient(const char* input);

private:
    bool clientStarted;
};

} // end namespace
