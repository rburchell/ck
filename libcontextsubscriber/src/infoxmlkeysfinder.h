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

#ifndef INFOXMLKEYSFINDER_H
#define INFOXMLKEYSFINDER_H

#include <QObject>
#include <QXmlDefaultHandler>
#include <QHash>
#include <QString>
#include "infokeydata.h"

class InfoXmlKeysFinder : public QXmlDefaultHandler
{
public:
    virtual bool startDocument();
    virtual bool startElement(const QString&, const QString&, const QString &name, const QXmlAttributes &attrs);
    virtual bool endElement(const QString&, const QString&, const QString &name);
    virtual bool characters(const QString &ch);

    QHash <QString, InfoKeyData> keyDataHash; //< A hash containing keyname -> InfoKeyData mapping. This is parse output.

private:
    bool inProvider; //< Are we in the <provider>...
    bool inKey; //< Are we in the <key>...
    bool inKeyType; //< Are we in the <key><type>...
    bool inKeyDoc; //< Are we in the <key><doc>...
    QString currentProvider; //< For our current parse position, the <provider service=?>.
    QString currentKeyName; //< For our current parse position, the <key>? name.
    QString currentKeyType; //< For our current parse position, the <key><type>? type.
    QString currentKeyDoc; //< For our current parse position, the <key><doc>? documentation.
    QString currentBus; //< For our current parse position, the <provider bus=?>.
    
    QString getAttrValue(const QXmlAttributes &attrs, const QString &attrName);

    QString canonicalizeType(const QString &type);
};

#endif // INFOXMLKEYSFINDER_H
