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
#include <QCoreApplication>
#include <QFileInfo>
#include <QDir>
#include <QMutex>
#include <QXmlSimpleReader>
#include <QXmlInputSource>
#include <QFile>
#include <QList>
#include <stdlib.h>
#include "sconnect.h"
#include "infoxmlbackend.h"
#include "logging.h"
#include "loggingfeatures.h"
#include "contextproviderinfo.h"
#include "nanoxml.h"

/*!
    \class InfoXmlBackend

    \brief Implements the InfoBackend for reading data from a directory
    with xml files.

    This class is not exported in the public API. It keeps all the data cached
    in the memory. It's assumed that this backend is not going to be used live in
    production systems and does not need to be ultra-fast (instead, implementation
    simplicity and corectness are preffered). For fast backend see the InfoCdbBackend.
*/

InfoXmlBackend::InfoXmlBackend(QObject *parent)
    : InfoBackend(parent)
{
    /* Thinking about locking... the watcher notifications are delivered synced,
       so asuming the changes in the dir are atomic this is all we need. */
    watcher = new QFileSystemWatcher();
    contextDebug() << F_XML << "Initializing xml backend with path:" << InfoXmlBackend::registryPath();

    QDir dir = QDir(InfoXmlBackend::registryPath());
    if (! dir.exists() || ! dir.isReadable()) {
        contextWarning() << "Registry path" << InfoXmlBackend::registryPath()
                         << "is not a directory or is not readable!";
    } else {
        watcher->addPath(InfoXmlBackend::registryPath());
        sconnect(watcher, SIGNAL(directoryChanged(QString)), this, SLOT(onDirectoryChanged(QString)));
        sconnect(watcher, SIGNAL(fileChanged(QString)), this, SLOT(onFileChanged(QString)));
    }

    regenerateKeyDataList();
}

InfoXmlBackend::~InfoXmlBackend()
{
    if (QCoreApplication::instance() != 0)
	delete(watcher);
}

/// Returns 'xml'.
QString InfoXmlBackend::name() const
{
    return QString("xml");
}

QStringList InfoXmlBackend::listKeys() const
{
    QStringList list;

    // FIXME Hmm, we could return just the keyDataHash.keys itself here...
    Q_FOREACH (QString key, keyDataHash.keys()) {
        list << keyDataHash.value(key).name;
    }

    return list;
}

QString InfoXmlBackend::docForKey(QString key) const
{
    if (! keyDataHash.contains(key))
        return "";

    return keyDataHash.value(key).doc;
}

bool InfoXmlBackend::keyDeclared(QString key) const
{
    if (keyDataHash.contains(key))
        return true;
    else
        return false;
}

bool InfoXmlBackend::keyDeprecated(QString key) const
{
    if (! keyDataHash.contains(key))
        return false;

    return keyDataHash.value(key).deprecated;
}

/// Returns the full path to the registry directory. Takes the
/// \c CONTEXT_PROVIDERS env variable into account.
QString InfoXmlBackend::registryPath()
{
    const char *regpath = getenv("CONTEXT_PROVIDERS");
    if (! regpath)
        regpath = DEFAULT_CONTEXT_PROVIDERS;

    return QString(regpath);
}

/// Returns the full path to the core property declaration file. Takes
/// the \c CONTEXT_CORE_DECLARATIONS env variable into account.
QString InfoXmlBackend::coreDeclPath()
{
    const char *corepath = getenv("CONTEXT_CORE_DECLARATIONS");
    if (! corepath)
        corepath = DEFAULT_CONTEXT_CORE_DECLARATIONS;

    return QString(corepath);
}

/* Slots */

/// Called when one of the parsed XML files changed. This
/// triggers a whole registry rebuild + signal emissions.
void InfoXmlBackend::onFileChanged(const QString &path)
{
    // If one of the watched xml files changed this it pretty much
    // an unconditional reload message for us.

    contextDebug() << F_XML << path << "changed.";

    QStringList oldKeys = listKeys();
    regenerateKeyDataList();
    QStringList currentKeys = listKeys();

    // In the cdb (fast) backend we do check if anybody is watching
    // before doing this list processing. But in xml backend the perf
    // is not an issue.

    // Emissions
    checkAndEmitKeysAdded(currentKeys, oldKeys); // DEPRECATED emission
    checkAndEmitKeysRemoved(currentKeys, oldKeys); // DEPRECATED emission
    Q_EMIT keysChanged(listKeys()); // DEPRECATED emission

    Q_EMIT listChanged();
    checkAndEmitKeyChanged(currentKeys, oldKeys);
}

/// Called when the registry directory changed (ie. file removed or added).
/// Triggers a whole registry rebuild + signal emissions. It detects a situation
/// when a added/removed file was not a parsed(xml) file.
void InfoXmlBackend::onDirectoryChanged(const QString &path)
{
    // It could be that some other file was added to the directory which
    // we don't care about anyways.

    QDir dir = QDir(registryPath());
    dir.setFilter(QDir::Files);
    dir.setNameFilters(QStringList("*.context"));

    // It's enough for us to compare sizes here, not actual content (filenames). The
    // latter case is always handled by the fileChanged.

    if (dir.entryInfoList().size() == countOfFilesInLastParse)
        return;

    contextDebug() << F_XML << registryPath() << "directory changed.";

    QStringList oldKeys = listKeys();
    regenerateKeyDataList();
    QStringList currentKeys = listKeys();

    // In the cdb (fast) backend we do check if anybody is watching
    // before doing this list processing. But in xml backend the perf
    // is not an issue.

    // Emissions
    checkAndEmitKeysAdded(currentKeys, oldKeys); // DEPRECATED emission
    checkAndEmitKeysRemoved(currentKeys, oldKeys); // DEPRECATED emission
    Q_EMIT keysChanged(listKeys()); // DEPRECATED emission

    Q_EMIT listChanged();
    checkAndEmitKeyChanged(currentKeys, oldKeys);
}

/* Private */

/// Clears all the stored data about the registry and parses it
/// all over again.
void InfoXmlBackend::regenerateKeyDataList()
{
    keyDataHash.clear();
    keyProvidersHash.clear();
    countOfFilesInLastParse = 0;

    // Stop watching all files. We do keep wathching the dir though.
    QStringList watchedFiles = watcher->files();
    if (watchedFiles.size() > 0)
        watcher->removePaths(watchedFiles);

    if (QFile(InfoXmlBackend::coreDeclPath()).exists()) {
        contextDebug() << F_XML << "Reading core declarations from:" << InfoXmlBackend::coreDeclPath();
        readKeyDataFromXml (InfoXmlBackend::coreDeclPath());
    } else {
        contextDebug() << F_XML << "Core declarations file" << InfoXmlBackend::coreDeclPath() << "does not exist.";
    }

    contextDebug() << F_XML << "Re-reading xml contents from" << InfoXmlBackend::registryPath();

    // Read the core property declarations.
    // For each xml file in the registry we parse it and
    // add it to our hash. We did some sanity checks in the constructor
    // so we skip them now.

    QDir dir = QDir(registryPath());

    // Bail out now if no directory
    if (! dir.exists() || ! dir.isReadable())
        return;

    dir.setFilter(QDir::Files);
    dir.setNameFilters(QStringList("*.context"));

    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo f = list.at(i);
        readKeyDataFromXml(f.filePath());

        if (! watcher->files().contains(f.filePath()))
            watcher->addPath(f.filePath());

        countOfFilesInLastParse++;
    }
}

/// Parse the given QVariant tree which is supposed to be a key tree.
void InfoXmlBackend::parseKey(const AssocTree &keyTree, const AssocTree &providerTree)
{
    QString key = keyTree.value("name").toString();
    QString plugin = providerTree.value("plugin").toString();
    QString constructionString = providerTree.value("constructionString").toString();
    QString doc = keyTree.value("doc").toString();
    QVariant deprecated_node = keyTree.node("deprecated");

    ContextTypeInfo typeInfo = keyTree.value("type");
    typeInfo = typeInfo.ensureNewTypes(); // Make sure to get rid of old names (INTEGER...)

    // Warn about description mismatch or add new
    if (keyDataHash.contains(key)) {
        if (typeInfo.name() != "" && typeInfo != keyDataHash[key].typeInfo)
            contextWarning() << F_XML << key << ": type mismatch in core property list and provider property list";
    } else {
        InfoKeyData keyData;
        keyData.name = key;
        keyData.typeInfo = typeInfo;
        keyData.doc = doc;
        keyData.deprecated = deprecated_node.isValid();

        contextDebug() << F_XML << "Adding new key" << key << "with type:" << keyData.typeInfo.name();
        keyDataHash.insert(key, keyData);
    }

    // Add provider details
    ContextProviderInfo providerInfo(plugin, constructionString);

    // Suport old-style XML...
    if (providerInfo.plugin == "") {
        QString currentProvider = providerTree.value("service").toString();
        QString currentBus = providerTree.value("bus").toString();

        if (currentBus != "" && currentProvider != "") {
            providerInfo.plugin = "contextkit-dbus";
            providerInfo.constructionString = currentBus + ":" + currentProvider;
        } else
            providerInfo.constructionString = "";
    }

    // If providerInfo is empty, do not add to the list
    if (providerInfo.plugin == "") {
        contextDebug() << F_XML << "Not adding provider info for key" << key << "no data";
        return;
    } else
        contextDebug() << F_XML << "Adding provider info for key" << key << "plugin:" << providerInfo.plugin << "constructionString:" << providerInfo.constructionString;

    // Add to the list of providers
    QList<ContextProviderInfo> list;
    if (keyProvidersHash.contains(key))
        list = keyProvidersHash.value(key);

    list.append(providerInfo);
    keyProvidersHash.insert(key, list);
}

/// Parses a given \a path file and adds it's contents to the hash.
/// Also adds the file to the watcher (starts observing it).
void InfoXmlBackend::readKeyDataFromXml(const QString &path)
{
    contextDebug() << F_XML << "Reading keys from" << path;

    NanoXml parser(path);

    // Check if format is all ok
    if (parser.didFail()) {
        contextWarning() << F_XML << "Reading" << path << "failed, parsing error.";
        return;
    }

    // Check the version of the file
    if (parser.namespaceUri() != "" && parser.namespaceUri() != BACKEND_COMPATIBILITY_NAMESPACE) {
        contextWarning() << F_XML << "Reading" << path << "failed, invalid version:" << parser.namespaceUri();
        return;
    }

    AssocTree rootTree = parser.result();

    if (rootTree.toList().at(0).toString() == "provider" ||
        rootTree.toList().at(0).toString() == "properties") {
        // One provider. Iterate over each key.
        Q_FOREACH (AssocTree keyTree, rootTree.nodes()) {
            if (keyTree.name() == "key")
                parseKey(keyTree, rootTree);
        }
    } else {
        // Multiple providers... iterate over providers and keys
        Q_FOREACH (AssocTree providerTree, rootTree.nodes())
            if (providerTree.name() == "provider")
                Q_FOREACH (AssocTree keyTree, providerTree.nodes())
                    if (keyTree.name() == "key")
                        parseKey(keyTree, providerTree);
    }
}

const QList<ContextProviderInfo> InfoXmlBackend::providersForKey(QString key) const
{
    return keyProvidersHash.value(key);
}

ContextTypeInfo InfoXmlBackend::typeInfoForKey(QString key) const
{
    if (! keyDataHash.contains(key))
        return ContextTypeInfo();

    return keyDataHash.value(key).typeInfo;
}
