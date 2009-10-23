#ifndef COMMANDWATCHER_H
#define COMMANDWATCHER_H

#include <QObject>
#include <QString>
#include <QDBusConnection>
#include <QTextStream>
#include <QStringList>

class QSocketNotifier;

class CommandWatcher : public QObject
{
    Q_OBJECT
public:
    CommandWatcher(int commandfd, QObject *parent = 0);

private:
    int commandfd;
    QSocketNotifier *commandNotifier;
    void interpret(const QString& command);
    static void help();

    // Processing commands
    void callGet(const QString& name, const QString& key);
    void callSubscribe(const QString& name, const QString& key);
    void callUnsubscribe(const QString& name, const QString& key);
    void resetSignalStatus();
    void waitForChanged(int timeout);

    // Helpers
    QDBusConnection getConnection(const QString& busType);
    QString describeValue(QList<QVariant> value, quint64 timestamp);
    QString describeQVariant(QVariant value);
    void listenToChanged(const QString& name);
    QString keyToPath(QString key);

private slots:
    void onActivated();
    void onValueChanged(QList<QVariant> value, quint64 timestamp, QDBusMessage& msg);

private:
    // Connection types and bus names for each custom name
    QMap<QString, QPair<QString, QString> > connectionMap;

    QTextStream out;

    // Stored data about recevied Changed signals
    bool changedSignalReceived;
    QStringList changedSignalParameters;
};
#endif
