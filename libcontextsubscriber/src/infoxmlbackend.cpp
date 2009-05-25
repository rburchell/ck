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

#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QMutex>
#include <QXmlSimpleReader>
#include <QXmlInputSource>
#include <QFile>
#include <stdlib.h>
#include "sconnect.h"
#include "infoxmlbackend.h"
#include "infoxmlkeysfinder.h"

InfoXmlBackend::InfoXmlBackend(QObject *parent)
    : InfoBackend(parent)
{
    /* Thinking about locking... the watcher notifications are delivered synced, 
       so asuming the changes in the dir are atomic this is all we need. */

    qDebug() << "XML backend with path" << InfoXmlBackend::registryPath();

    QDir dir = QDir(InfoXmlBackend::registryPath());
    if (! dir.exists() || ! dir.isReadable()) {
        qDebug() << "Registry path is not a directory or is not readable!";
        return;
    }

    watcher.addPath(InfoXmlBackend::registryPath());
    sconnect(&watcher, SIGNAL(directoryChanged(QString)), this, SLOT(onDirectoryChanged(QString)));
    sconnect(&watcher, SIGNAL(fileChanged(QString)), this, SLOT(onFileChanged(QString)));

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

QStringList InfoXmlBackend::listKeys(QString providername) const
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

QStringList InfoXmlBackend::listProviders() const
{
    // Again -- slow. 
    QStringList list;

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

QString InfoXmlBackend::providerDBusTypeForKey(QString key) const
{
    if (! keyDataHash.contains(key))
        return "";

    return keyDataHash.value(key).bus;
}

QString InfoXmlBackend::registryPath()
{
    const char *regpath = getenv("CONTEXT_PROVIDERS");
    if (! regpath)
        regpath = DEFAULT_CONTEXT_PROVIDERS;

    return QString(regpath);
}

/* Slots */

void InfoXmlBackend::onFileChanged(const QString &path)
{
    // If one of the watched xml files changed this it pretty much 
    // an unconditional reload message for us.

    qDebug() << path << "changed.";
    
    QStringList oldKeys = listKeys();
    regenerateKeyDataList();
    QStringList currentKeys = listKeys();
    
    // In the cdb (fast) backend we do check if anybody is watching 
    // before doing this list processing. But in xml backend the perf
    // is not an issue.

    // Emissions
    checkAndEmitKeysAdded(currentKeys, oldKeys);
    checkAndEmitKeysRemoved(currentKeys, oldKeys);
    emit keysChanged(listKeys());
    checkAndEmitKeysChanged(currentKeys, oldKeys);
}

void InfoXmlBackend::onDirectoryChanged(const QString &path)
{
    // It could be that some other file was added to the directory which 
    // we don't care about anyways. 

    QDir dir = QDir(registryPath());
    dir.setFilter(QDir::Files);
    dir.setNameFilters(QStringList("*.xml"));

    // It's enough for us to compare sizes here, not actual content (filenames). The
    // latter case is always handled by the fileChanged.

    if (dir.entryInfoList().size() == countOfFilesInLastParse)
        return;

    qDebug() << registryPath() << "directory changed.";
    
    QStringList oldKeys = listKeys();
    regenerateKeyDataList();
    QStringList currentKeys = listKeys();
    
    // In the cdb (fast) backend we do check if anybody is watching 
    // before doing this list processing. But in xml backend the perf
    // is not an issue.

    // Emissions
    checkAndEmitKeysAdded(currentKeys, oldKeys);
    checkAndEmitKeysRemoved(currentKeys, oldKeys);
    emit keysChanged(listKeys());
    checkAndEmitKeysChanged(currentKeys, oldKeys);
}

/* Private */

void InfoXmlBackend::regenerateKeyDataList()
{
    keyDataHash.clear();
    countOfFilesInLastParse = 0;

    // Stop watching all files. We do keep wathching the dir though. 
    QStringList watchedFiles = watcher.files();
    if (watchedFiles.size() > 0)
        watcher.removePaths(watchedFiles);

    qDebug() << "Re-reading xml contents from" << InfoXmlBackend::registryPath();

    // For each xml file in the registry we parse it and 
    // add it to our hash. We did some sanity checks in the constructor
    // so we skip them now.

    QDir dir = QDir(registryPath());
    dir.setFilter(QDir::Files);
    dir.setNameFilters(QStringList("*.xml"));

    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo f = list.at(i);
        readKeyDataFromXml(f);

        if (! watcher.files().contains(f.filePath()))
            watcher.addPath(f.filePath());

        countOfFilesInLastParse++;
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

    // Use insert here instead of unite to overwrite keys
    foreach (QString key, handler.keyDataHash.keys()) {
        if (keyDataHash.contains(key)) 
            qDebug() << "WARNING: key" << key << "already defined in previous xml file. Overwriting.";

        keyDataHash.insert(key, handler.keyDataHash.value(key));
    }
}


