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

#include <QDebug>
#include "infoxmlkeysfinder.h"

/*!
    \class InfoXmlKeysFinder

    \brief Implements a SAX parser to parse xml files with provider/key data.

    This class is not exported in the public API. Traditional old-school context-based
    parsing logic here.
*/

/// Called when parsing starts.
bool InfoXmlKeysFinder::startDocument()
{
    keyDataHash.clear();
    inProvider = false;
    inKey = false;
    inKeyType = false;
    inKeyDoc = false;
}

/// Called for each <element> when we start parsing it.
bool InfoXmlKeysFinder::startElement(const QString&, const QString&, const QString &name, const QXmlAttributes &attrs)
{
    // <provider> ...
    if (inProvider == false && name == "provider") {
        inProvider = true;
        currentProvider = getAttrValue(attrs, "service");
        currentBus = getAttrValue(attrs, "bus");
        return true;
    }

    // <key> ...
    if (inKey == false && inProvider == true && name == "key") {
        // Reset all potential key data
        currentKeyName = "";
        currentKeyType = canonicalizeType (getAttrValue(attrs, "type"));
        currentKeyDoc = "";
        currentKeyName = getAttrValue(attrs, "name");

        inKey = true;
            
        return true;
    }

    // <type> ...
    if (inKeyType == false && inKey == true && name == "type") {
        inKeyType = true;
        currentKeyType = ""; // Reset type data
        return true;
    }

    // <doc> ...
    if (inKeyDoc == false && inKey == true && name == "doc") {
        inKeyDoc = true;
        currentKeyDoc = ""; // Reset doc data
        return true;
    }

    return true;
}

/// Called for each </element> when we stop parsing it.
bool InfoXmlKeysFinder::endElement(const QString&, const QString&, const QString &name)
{
    // ... </provider>
    if (inProvider == true && name == "provider") {        
        inProvider = false;
        inKey = false;
        inKeyDoc = false;
        inKeyType = false;
        return true;
    } 

    // ... </key>
    if (inKey == true && name == "key") {
        // If at least name is ok, add to list
        if (currentKeyName != "") {
            qDebug() << "Got key:" << currentKeyName;

            InfoKeyData data;
            data.name = currentKeyName;
            data.type = currentKeyType;
            data.doc = currentKeyDoc;
            data.provider = currentProvider;
            data.bus = currentBus;

            if (keyDataHash.contains(currentKeyName)) 
                qDebug() << "WARNING: key" << currentKeyName << "already defined in this xml file. Overwriting.";
            keyDataHash.insert(currentKeyName, data);
        }

        inKey = false;
        inKeyDoc = false;
        inKeyType = false;
        return true;
    } 

    // ... </doc>
    if (inKeyDoc == true && name == "doc") {
        inKeyDoc = false;
        return true;
    } 

    // ... </type>
    if (inKeyType == true && name == "type") {
        inKeyType = false;
        return true;
    }

    return true;
}

/// Called for each bit of textual data. We trim spaces here.
bool InfoXmlKeysFinder::characters(const QString &chars)
{
    // <type> CHARS ...
    if (inKeyType == true) {
        if (currentKeyType != "")
            qDebug() << "WARNING: key" << currentKeyName << "already has a type. Overwriting.";
        currentKeyType = canonicalizeType (chars.trimmed());
        return true;
    }

    // <doc> CHARS ...
    if (inKeyDoc == true) {
        currentKeyDoc += chars.trimmed();
        return true;
    }

    return true;
}

/* Private */

/// In the given \a attrs atribute list, find the one with \a attrName and return it's value.
/// Returns empty string if attribute not found.
QString InfoXmlKeysFinder::getAttrValue(const QXmlAttributes &attrs, const QString &attrName)
{
    for (int i = 0; i< attrs.count(); i++) {
        if (attrs.localName(i) == attrName) 
            return attrs.value(i);
    }
    
    return "";
}

// Convert a subset of new-style type names to the currently used
// old-style type names.  This way we can slowely fade in new-style
// types.

QString InfoXmlKeysFinder::canonicalizeType (const QString &type)
{
    if (type == "bool")
        return "TRUTH";
    else if (type == "int32")
        return "INT";
    else if (type == "string")
        return "STRING";
    else if (type == "double")
        return "DOUBLE";
    else
        return type;
}
