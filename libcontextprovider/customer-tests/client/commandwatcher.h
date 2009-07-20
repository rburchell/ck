#ifndef COMMANDWATCHER_H
#define COMMANDWATCHER_H

#include <QObject>
#include <QString>
#include <QDBusConnection>
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

private slots:
    void onActivated();

private:
    QString subscriberPath;
    QString busName;
    QDBusConnection connection;
};
#endif
