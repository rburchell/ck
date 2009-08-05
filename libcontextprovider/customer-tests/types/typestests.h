#include <QString>
#include <QObject>

class QProcess;

namespace ContextProvider {

class Property;

class TypesTests : public QObject
{
    Q_OBJECT

private:
    Property *intItem;
    Property *boolItem;
    Property *doubleItem;
    Property *stringItem;
    Property *stringListItem;
    Property *charItem;
    Property *dateItem;
    Property *timeItem;
    QProcess *client;
    bool isReadyToRead;

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void typesInReturnValueOfSubscribe();
    void typesInChangeSignal();

public slots:
    void readStandardOutput();

private:
    QString writeToClient(const char* input);

private:
    bool clientStarted;
};

} // end namespace
