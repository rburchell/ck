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
    /* Thinking about locking... the watcher notifications are delivered synced, 
       so asuming the changes in the dir are atomic this is all we need. */

    watcher.addPath(registryPath());
    connect(&watcher, SIGNAL(directoryChanged(QString)), this, SLOT(onDirectoryChanged(QString)));

	regenerateKeyDataList();
}

QString InfoXmlBackend::name() const
{
    return QString("xml backend");
}

QStringList InfoXmlBackend::listKeys() const
{
	QStringList list;

    // FIXME Hmm, we could return just the keyDataHash.keys itself here...
    foreach (QString key, keyDataHash.keys()) {
        list << keyDataHash.value(key).name;
    }
		
	return list;
}

QList<QString> InfoXmlBackend::listKeys(QString providername) const
{
    // This is slow and not nice, but we're an xml backend and 
    // we can afford to not be the first in the run
   	QStringList list;

    foreach (QString key, keyDataHash.keys()) {
        if (keyDataHash.value(key).provider == providername)
            list << keyDataHash.value(key).name;
    }

    return list;
}

QList<QString> InfoXmlBackend::listProviders() const
{
    // Again -- slow. 
    QList <QString> list;

    foreach (QString key, keyDataHash.keys()) {
        QString provider = keyDataHash.value(key).provider;
        if (! list.contains(provider))
            list << provider;
    }

    return list;
}

QString InfoXmlBackend::typeForKey(QString key) const
{
    if (! keyDataHash.contains(key))
        return "";

    return keyDataHash.value(key).type;
}

QString InfoXmlBackend::docForKey(QString key) const
{
    if (! keyDataHash.contains(key))
        return "";

    return keyDataHash.value(key).doc;
}

QString InfoXmlBackend::providerForKey(QString key) const
{
    if (! keyDataHash.contains(key))
        return "";

    return keyDataHash.value(key).provider;
}

/* Slots */

void InfoXmlBackend::onDirectoryChanged(const QString &path)
{
    regenerateKeyDataList();
    emit keysChanged(listKeys());
}

/* Private */

void InfoXmlBackend::regenerateKeyDataList()
{
    keyDataHash.clear();

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
		readKeyDataFromXml(f);
    }
}

void InfoXmlBackend::readKeyDataFromXml(const QFileInfo &finfo)
{
    qDebug() << "Reading keys from" << finfo.filePath();

    InfoXmlKeysFinder handler;
    QFile f(finfo.filePath());
    QXmlInputSource source (&f);
    QXmlSimpleReader reader;

    reader.setContentHandler(&handler);
    reader.parse(source);

    keyDataHash.unite(handler.keyDataHash);
}

QString InfoXmlBackend::registryPath() const
{
    const char *regpath = getenv("CONTEXT_PROVIDERS");
    if (! regpath)
        regpath = DEFAULT_REGISTRY_PATH;

    return QString(regpath);
}

