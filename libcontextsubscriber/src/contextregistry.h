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

#ifndef CONTEXTREGISTRY_H
#define CONTEXTREGISTRY_H

#include <QVariant>
#include <QStringList>
#include <QObject>
#include <QMutex>

class ContextRegistry : public QObject
{
    Q_OBJECT 

public:

    static ContextRegistry* instance()
    {
      static QMutex mutex;
      if (!registryInstance)
      {
          mutex.lock();
 
          if (! registryInstance)
              registryInstance = new ContextRegistry;
 
          mutex.unlock();
      }
 
      return registryInstance;
    }
 
    QList<QString> listKeys() const;
    QList<QString> listKeys(QString providername) const;
    QList<QString> listProviders() const;

private:
    ContextRegistry() {};
    ContextRegistry(const ContextRegistry&);

    ContextRegistry& operator=(const ContextRegistry&);
    
    static ContextRegistry* registryInstance;

signals:
    void keysChanged(QStringList currentKeys);
};

#endif
