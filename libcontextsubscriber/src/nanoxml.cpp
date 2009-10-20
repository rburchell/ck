/*
 * Copyright (C) 2009 Nokia Corporation.
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

#include "nanoxml.h"
#include <QDebug>

/*!
    \class NanoXml

    \brief Parses XML files into a nano dom.

    This class is not exported in the public API. It's used to parse a formatted
    XML into a nanodom tree. To learn about the nanadom please read the documentation
    on the desktop types.

    Short overview - the following XML is being turned into the following semantic
    structure:

    \code
    <key name="Example.Random" type="string">
        <doc>A random property.</doc>
    </key>

    ['key',
        ['name', 'Example.Random' ],
        ['type', 'string' ],
        ['doc', 'A random property.']
    ]
    \endcode

    Another example:

    \code
    <key name="Example.Random">
        <type>
            <uniform-list type="number"/>
        </type>
    </key>

    ['key',
        ['name', 'Example.Random' ],
        ['type',
            ['uniform-list', ['type', 'number' ] ]
        ]
    ]
    \endcode
*/

/// Constructor. Creates a new nanodom tree reading XML data from path. After creating
/// the object you should check the didFail to see if parsing succeded.
NanoXml::NanoXml(const QString& path) : NanoTree(QVariant())
{
    current = NULL;

    QFile f(path);
    QXmlInputSource source (&f);
    QXmlSimpleReader reader;

    reader.setContentHandler(this);
    failed = ! reader.parse(source);
}

/// Destructor.
NanoXml::~NanoXml()
{
    delete current;
}

/// Parser internal. Add a value (item) \a v to the current list on the stack.
void NanoXml::addValue(const QString& v)
{
    current->append(QVariant(v));
}

/// Parser internal. Creates a new list and pushes it to the top of the stack.
void NanoXml::pushList()
{
    if (current)
        stack.push(current);

    current = new QList<QVariant> ();
}

/// Parser internal. Pops one list from the stack. Closes the list and attaches
/// it to the previoius list on the stack.
void NanoXml::popList()
{
    if (current == NULL) {
        // FIXME: Warning goes here
        return;
    }

    QVariant currentListAsVariant(*current);
    delete current;

    if (stack.count() > 0) {
        current = stack.pop();
        current->append(currentListAsVariant);
    } else {
        // End of stack unwinding. We're done.
        current = NULL;
        rootVariant = currentListAsVariant;
    }
}

/// Called when a namespace prefix mapping starts. We use this to get the
/// xml version.
bool NanoXml::startPrefixMapping(const QString &prefix, const QString &uri)
{
    nspace = uri;
    return true;
}

/// Called by the XML parser when parsing starts.
bool NanoXml::startElement(const QString&, const QString&, const QString &name, const QXmlAttributes &attrs)
{
    pushList();
    addValue(name);

    for (int i = 0; i< attrs.count(); i++) {
        pushList();
        addValue(attrs.localName(i));
        addValue(attrs.value(i));
        popList();
    }

    return true;
}

/// Called by the XML parser when parsing starts.
bool NanoXml::endElement(const QString&, const QString&, const QString &name)
{
    popList();
    return true;
}

/// Called by the XML parser when parsing starts.
bool NanoXml::characters(const QString &chars)
{
    QString trimmed = chars.trimmed();

    if (trimmed != "")
        addValue(trimmed);

    return true;
}

/// Returns the namespace URI of the parsed (source) XML document. Empty if
/// it wasn't specified.
const QString NanoXml::namespaceUri()
{
    return nspace;
}

/// Returns true if parsing failed. Fals otherwise. Use it to check if the nanodom
/// tree is fine and usable.
bool NanoXml::didFail()
{
    return failed;
}

