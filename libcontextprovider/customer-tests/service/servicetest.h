#include <QObject>

class QProcess;

namespace ContextProvider {

class ServiceTest : public QObject
{
    Q_OBJECT

    QProcess *client;
    bool isReadyToRead;

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void startStopStart();
    void recreate();
    void multiStart();
    void defaultService();
    void recreateProperty();

public slots:
    void readStandardOutput();

private:
    QString writeToClient(const char* input);

private:
    bool clientStarted;
};

} // end namespace
