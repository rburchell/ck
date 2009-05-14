/*
 * Copyright (C) 2008 Nokia Corporation.
 *
 * Contact: Marius Vollmer <marius.vollmer@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef CONTEXTREGISTRYINFO_H
#define CONTEXTREGISTRYINFO_H

#include <QVariant>
#include <QStringList>
#include <QObject>
#include <QMutex>

/*!
  \class ContextRegistryInfo

  \brief A class to introspect the registry contents.

  This is a singelton class used to obtain information about the keys (properties)
  in the registry database. The information can be provided either from xml files 
  or from a cdb database. It's possible to list all the keys in the registry and 
  also list all keys belonging to a one particular provider. 

*/

class ContextRegistryInfo : public QObject
{
    Q_OBJECT 

public:

    static ContextRegistryInfo* instance(const QString &backendName = "");
 
    QStringList listKeys() const;
    QStringList listKeys(QString providername) const;
    QStringList listProviders() const;

private:
    ContextRegistryInfo() {};
    ContextRegistryInfo(const ContextRegistryInfo&);

    ContextRegistryInfo& operator=(const ContextRegistryInfo&);
    
    static ContextRegistryInfo* registryInstance;

private slots:
    void onKeysChanged(QStringList currentKeys);

signals:
    void keysChanged(QStringList currentKeys);
};

#endif
