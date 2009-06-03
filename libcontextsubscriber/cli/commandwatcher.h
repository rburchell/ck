#ifndef COMMANDWATCHER_H
#define COMMANDWATCHER_H

#include <QObject>
class QFile;
class QTextStream;
class QSocketNotifier;
class ContextProperty;
class QString;
template <typename K, typename V> class QMap;

class CommandWatcher : public QObject
{
    Q_OBJECT
public:
    CommandWatcher(int commandfd, QMap<QString, ContextProperty*> *properties, QObject *parent = 0);
private:
    int commandfd;
    QSocketNotifier *commandNotifier;
    void interpret(const QString& command) const;
    QMap<QString, ContextProperty*> *properties;
    static void help();

private slots:
    void onActivated();
};
#endif
