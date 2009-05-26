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

/*!
    \class InfoXmlKeysFinder

    \brief Implements a SAX parser to parse xml files with provider/key data.

    This class is not exported in the public API. Traditional old-school context-based
    parsing logic here.
*/

class InfoXmlKeysFinder : public QXmlDefaultHandler
{
public:
    virtual bool startDocument();
    virtual bool startElement(const QString&, const QString&, const QString &name, const QXmlAttributes &attrs);
    virtual bool endElement(const QString&, const QString&, const QString &name);
    virtual bool characters(const QString &ch);

    /// A hash containing keyname -> InfoKeyData mapping.
    QHash <QString, InfoKeyData> keyDataHash;

private:
    /// Are we in the <provider>...
    bool inProvider;

    /// Are we in the <key>...
    bool inKey;

    /// Are we in the <key><type>...
    bool inKeyType;

    /// Are we in the <key><doc>...
    bool inKeyDoc;

    /// For our current parse position, the <provider service=?>.
    QString currentProvider;

    /// For our current parse position, the <key>? name.
    QString currentKeyName;

    /// For our current parse position, the <key><type>? type.
    QString currentKeyType;
    
    /// For our current parse position, the <key><doc>? documentation.
    QString currentKeyDoc;

    /// For our current parse position, the <provider bus=?>.
    QString currentBus;
    
    QString getAttrValue(const QXmlAttributes &attrs, const QString &attrName);
};

#endif // INFOXMLKEYSFINDER_H
