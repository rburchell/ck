#include <QObject>

class QProcess;

namespace ContextProvider {

class ServiceTests : public QObject
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

public slots:
    void readStandardOutput();

private:
    QString writeToClient(const char* input);

private:
    bool clientStarted;
};

} // end namespace
