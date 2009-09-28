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
NanoXml::NanoXml(const QString& path)
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

/// Returns the root of the nanodom tree. If parsing XML failed, it will return
/// and empty QVariant. Otherwise it returns a QVariant containing a list of other
/// QVariants.
const QVariant& NanoXml::root()
{
    return rootVariant;
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

/// Dumps a QVariant into a multi-line string for debugging purposes.
QString NanoXml::dumpTree(const QVariant &tree, int level)
{
    QString s;
    for (int i = 0; i < level; i++)
        s += "  ";

    if (tree.type() == QVariant::String)
        s += tree.toString() + "\n";
    else if (tree.type() == QVariant::List) {
        s += "[\n";

        foreach(QVariant v, tree.toList())
            s += NanoXml::dumpTree(v, level + 1);

        for (int i = 0; i < level; i++)
            s += "  ";

        s += "]\n";
    }

    return s;
}

/* Local accessors */

/// Returns the sub (the trailing) after a given \a key in the root nanodom tree.
QVariant NanoXml::keySub(const QString &key)
{
	return NanoXml::keySub(key, rootVariant);
}

/// 1st level accessor. Returns a value for a \a key in the root nanodom tree.
QVariant NanoXml::keyValue(const QString &key)
{
	return NanoXml::keyValue(key, rootVariant);
}

/// Returns the list of QVariants matching the \a key in the root tree. A QVariant
/// node matches if it's a list by and it's first element is \a key.
QVariantList NanoXml::keyValues(const QString &key)
{
    return NanoXml::keyValues(key, rootVariant);
}

/// 2nd level accessor. Returns a value for a \a key1 \a key2 in the root nanodom tree.
QVariant NanoXml::keyValue(const QString &key1, const QString &key2)
{
	return NanoXml::keyValue(key1, key2, rootVariant);
}

/// 3rd level accessor. Returns a value for a \a key1 \a key2 \a key3 in the root nanodom tree.
QVariant NanoXml::keyValue(const QString &key1, const QString &key2, const QString &key3)
{
	return NanoXml::keyValue(key1, key2, key3, rootVariant);
}

/// 4rd level accessor. Returns a value for a \a key1 \a key2 \a key3 \a key4 in the root nanodom tree.
QVariant NanoXml::keyValue(const QString &key1, const QString &key2, const QString &key3,
                           const QString &key4)
{
	return NanoXml::keyValue(key1, key2, key3, key4, rootVariant);
}

/// 5th level accessor. Returns a value for a \a key1 \a key2 \a key3 \a key4 \a key5 in the root nanodom tree.
QVariant NanoXml::keyValue(const QString &key1, const QString &key2, const QString &key3,
                           const QString &key4, const QString &key5)
{
	return NanoXml::keyValue(key1, key2, key3, key4, key5, rootVariant);
}

/* Static accessors */

/// Returns the sub (the trailing) after a given \a key in the specified \a dom tree.
QVariant NanoXml::keySub(const QString &key, const QVariant &dom)
{
    const QVariant keyVariant(key); // So we can directly compare...

    // We expect the dom to be a list...
    if (dom.type() != QVariant::List)
        return QVariant();

    foreach(QVariant child, dom.toList())
    {
        if (child.type() == QVariant::List && child.toList().count() > 0 &&
            child.toList().at(0) == keyVariant) {
            QVariantList lst = child.toList();
            lst.removeAt(0);
            return QVariant(lst);
        }
    }

    return QVariant();
}

/// Returns the list of QVariants matching the \a key in a given tree. A QVariant
/// node matches if it's a list by and it's first element is \a key.
QVariantList NanoXml::keyValues(const QString &key, const QVariant &dom)
{
    QVariantList lst;
    const QVariant keyVariant(key); // So we can directly compare...

    // We expect the dom to be a list...
    if (dom.type() != QVariant::List)
        return lst;

    foreach(QVariant child, dom.toList())
    {
        if (child.type() == QVariant::List && child.toList().count() > 0 &&
            child.toList().at(0) == keyVariant) {
            lst.append(child);
        }
    }

    return lst;
}

/// 1st level accessor. Returns a value for a \a key in the given \a dom tree.
QVariant NanoXml::keyValue(const QString &key, const QVariant &dom)
{
    const QVariant keyVariant(key); // So we can directly compare...

    // We expect the dom to be a list...
    if (dom.type() != QVariant::List)
        return QVariant();

    foreach(QVariant child, dom.toList())
    {
        if (child.type() == QVariant::List && child.toList().count() == 2 &&
            child.toList().at(0) == keyVariant)
            return child.toList().at(1);
    }

    return QVariant();
}

/// 2nd level accessor. Returns a value for a \a key1 \a key2 in the given \a dom tree.
QVariant NanoXml::keyValue(const QString &key1, const QString &key2, const QVariant &dom)
{
    return NanoXml::keyValue(key2, keySub(key1, dom));
}

/// 3rd level accessor. Returns a value for a \a key1 \a key2 \a key3 in the given \a dom tree.
QVariant NanoXml::keyValue(const QString &key1, const QString &key2, const QString &key3,
                                 const QVariant &dom)
{
    return NanoXml::keyValue(key2, key3, keySub(key1, dom));
}

/// 4rd level accessor. Returns a value for a \a key1 \a key2 \a key3 \a key4 in the given \a dom tree.
QVariant NanoXml::keyValue(const QString &key1, const QString &key2, const QString &key3,
                           const QString &key4, const QVariant &dom)
{
    return NanoXml::keyValue(key2, key3, key4, keySub(key1, dom));
}

/// 5th level accessor. Returns a value for a \a key1 \a key2 \a key3 \a key4 \a key5 in the given \a dom tree.
QVariant NanoXml::keyValue(const QString &key1, const QString &key2, const QString &key3,
                           const QString &key4, const QString &key5, const QVariant &dom)
{
    return NanoXml::keyValue(key2, key3, key4, key5, keySub(key1, dom));
}

