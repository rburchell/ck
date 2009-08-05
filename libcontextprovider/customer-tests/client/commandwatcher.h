#ifndef COMMANDWATCHER_H
#define COMMANDWATCHER_H

#include <QObject>
#include <QString>
#include <QDBusConnection>
#include <QTextStream>

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
    void callGetSubscriber(QDBusConnection connection, const QString& busName);
    void callSubscribe(const QString& busName, const QStringList& args);
    void callUnsubscribe(const QString& busName, const QStringList& args);
    void resetSignalStatus();
    void waitForChanged(int timeout);

    // Helpers
    QDBusConnection getConnection(const QString& busType);

private slots:
    void onActivated();
    void onChanged();

private:
    // The subscriber paths we get from different connections. Note: a
    // restriction: we cannot be connected to [session bus, name x]
    // and [system bus, name x] at the same time.
    QMap<QString, QString> subscriberPaths;

    // Connection types for each provider bus name
    QMap<QString, QString> connectionTypes;

    QTextStream out;
    bool changedSignalReceived;
};
#endif
