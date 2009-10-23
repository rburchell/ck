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
    void callGet(const QString& busName, const QStringList& args);
    void callSubscribe(const QString& busName, const QStringList& args);
    void callUnsubscribe(const QString& busName, const QStringList& args);
    void resetSignalStatus();
    void waitForChanged(int timeout);

    // Helpers
    QDBusConnection getConnection(const QString& busType);
    QString describeValuesAndUnknowns(const QMap<QString, QVariant>& knownValues, QStringList unknownKeys);
    QString describeQVariant(QVariant value);

private slots:
    void onActivated();
    void onChanged(QMap<QString, QVariant> knownValues, QStringList unknownKeys);

private:
    // The subscriber paths we get from different connections. Note: a
    // restriction: we cannot be connected to [session bus, name x]
    // and [system bus, name x] at the same time.
    QMap<QString, QString> subscriberPaths;

    // Assign custom Name to bus types and provider bus name
    QMap<QString,QPair<QString, QString> > connectionsMap;

    QTextStream out;

    // Stored data about recevied Changed signals
    bool changedSignalReceived;
    QStringList changedSignalParameters;
};
#endif
