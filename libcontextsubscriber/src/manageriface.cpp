/*
 * This file was generated by dbusxml2cpp version 0.6
 * Command line was: dbusxml2cpp -c ManagerInterface -p manageriface_p.h:manageriface.cpp -i valuesetmaptype.h -N Manager.xml
 *
 * dbusxml2cpp is Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#include "manageriface_p.h"

/*
 * Implementation of interface class ManagerInterface
 */

ManagerInterface::ManagerInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent)
    : QDBusAbstractInterface(service, path, staticInterfaceName(), connection, parent)
{
}

ManagerInterface::~ManagerInterface()
{
}

