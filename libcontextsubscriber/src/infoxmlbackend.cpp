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

#include "infoxmlbackend.h"
#include "infoxmlbackendconfig.h"
#include "infoxmlkeysfinder.h"
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QXmlSimpleReader>
#include <QXmlInputSource>
#include <QFile>
#include <stdlib.h>

InfoXmlBackend::InfoXmlBackend(QObject *parent)
    : InfoBackend(parent)
{
    regenerateKeyToXmlMappings();
}

QString InfoXmlBackend::name() const
{
    return QString("xml backend");
}

/* Private */

void InfoXmlBackend::regenerateKeyToXmlMappings()
{
    keyToXmlFileHash.clear();

    qDebug() << "Re-reading xml contents from" << registryPath();

    // For each xml file in the registry we parse it and 
    // add it to our hash

    // FIXME: More sanity checks here
    QDir dir = QDir(registryPath());
    if (! dir.exists() || ! dir.isReadable()) {
        qDebug() << "Registry path is not a directory or is not readable!";
        return;
    }

    // FIXME: Locking will go somewhere here...

    dir.setFilter(QDir::Files);
    dir.setNameFilters(QStringList("*.xml"));

    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo f = list.at(i);
        readKeysFromXmlToHash(f);
    }
}

void InfoXmlBackend::readKeysFromXmlToHash(const QFileInfo &finfo)
{
    qDebug() << "Reading keys from" << finfo.filePath();

    InfoXmlKeysFinder handler;
    QFile f(finfo.filePath());
    QXmlInputSource source (&f);
    QXmlSimpleReader reader;

    reader.setContentHandler(&handler);
    reader.parse(source);

    // Now put all the keys obtained by our parser into the hash
    for (int i = 0; i < handler.keys.size(); i++)
        keyToXmlFileHash.insert(handler.keys.at(i), finfo.filePath());
}

QString InfoXmlBackend::registryPath() const
{
    const char *regpath = getenv("CONTEXT_PROVIDERS");
    if (! regpath)
        regpath = DEFAULT_REGISTRY_PATH;

    return QString(regpath);
}

