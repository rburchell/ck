/*
 * This file was generated by dbusxml2cpp version 0.6
 * Command line was: dbusxml2cpp -c ManagerInterface -p manageriface_p.h:manageriface.cpp -i dbusvariantmap.h -N Manager.xml
 *
 * dbusxml2cpp is Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#ifndef MANAGERIFACE_P_H_1241787963
#define MANAGERIFACE_P_H_1241787963

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>
#include "dbusvariantmap.h"

/*
 * Proxy class for interface org.freedesktop.ContextKit.Manager
 */
class ManagerInterface: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "org.freedesktop.ContextKit.Manager"; }

public:
    ManagerInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

    ~ManagerInterface();

public Q_SLOTS: // METHODS
    inline QDBusReply<DBusVariantMap> Get(const QStringList &keys, QStringList &undeterminable_keys)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(keys);
        QDBusMessage reply = callWithArgumentList(QDBus::Block, QLatin1String("Get"), argumentList);
        if (reply.type() == QDBusMessage::ReplyMessage && reply.arguments().count() == 2) {
            undeterminable_keys = qdbus_cast<QStringList>(reply.arguments().at(1));
        }
        return reply;
    }

    inline QDBusReply<QDBusObjectPath> GetSubscriber()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("GetSubscriber"), argumentList);
    }

Q_SIGNALS: // SIGNALS
};

#endif
