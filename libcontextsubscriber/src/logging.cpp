/*
 * Copyright (C) 2008, 2009 Nokia Corporation.
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

#include "logging.h"
#include <stdio.h>

/* NullIODevice */

qint64 NullIODevice::readData(char*, qint64)
{
    return 0;
}

qint64 NullIODevice::writeData(const char*, qint64)
{ 
    return 0; 
}

/* ContextRealLogger */

bool ContextRealLogger::showTest = true;
bool ContextRealLogger::showDebug = true;
bool ContextRealLogger::showWarning = true;
bool ContextRealLogger::showCritical = true;
bool ContextRealLogger::hideTimestamps = false;
bool ContextRealLogger::useColor = false;
char* ContextRealLogger::showModule = NULL;
char* ContextRealLogger::hideModule = NULL;
bool ContextRealLogger::initialized = false;

void ContextRealLogger::initialize()
{
    const char *verbosity = getenv("CONTEXT_LOG_VERBOSITY");
    if (! verbosity)
        return;
        
    if (getenv("CONTEXT_LOG_HIDE_TIMESTAMPS") != NULL)
        hideTimestamps = true;

    if (getenv("CONTEXT_LOG_USE_COLOR") != NULL)
        useColor = true;
        
    showModule = getenv("CONTEXT_LOG_SHOW_MODULE");
    hideModule = getenv("CONTEXT_LOG_HIDE_MODULE");
        
    if (strcmp(verbosity, "TEST") == 0) {
        // Do nothing, all left true
    } else if (strcmp(verbosity, "DEBUG") == 0) {
        showTest = false;
    } else if (strcmp(verbosity, "WARNING") == 0) {
        showTest = false;
        showDebug = false;
    } else if (strcmp(verbosity, "CRITICAL") == 0) {
        showTest = false;
        showDebug = false;
        showWarning = false;
    }
    
    initialized = true;
}

ContextRealLogger::ContextRealLogger(int msgType, const char *func, const char *file, int line) 
    : QTextStream(stderr)
{
    nullDevice = NULL;
    
    if (! initialized) {
        // This is not thread safe, but our initialization depends on 
        // non-mutable parts anyways, so we should be ok.
        initialize();
    }
    
    const char *msgTypeString = NULL;
    
    // Killing by msg type
    if (msgType == CONTEXT_LOG_MSG_TYPE_DEBUG) {
        if (! showDebug) {
            killOutput();
            return;
        } else
            msgTypeString = "DEBUG";
    } else if (msgType == CONTEXT_LOG_MSG_TYPE_WARNING) {
        if (! showWarning) {
            killOutput();
            return;
        } else
            msgTypeString = (useColor) ? "\033[1;33mWARNING\033[0m" : "WARNING";
    } else if (msgType == CONTEXT_LOG_MSG_TYPE_CRITICAL) {
        if (! showCritical) {
            killOutput();
            return;
        } else
            msgTypeString = (useColor) ? "\033[1;31mCRITICAL\033[0m" : "CRITICAL";
    } else if (msgType == CONTEXT_LOG_MSG_TYPE_TEST) {
        if (! showTest) {
            killOutput();
            return;
        } else
            msgTypeString = "TEST";
    }
    
    // Killing if we're not the module we're interested in
    if (showModule && strcmp(showModule, CONTEXT_LOG_MODULE_NAME) != 0) {
        killOutput();
        return;
    }
    
    // Killing if we're the module we're NOT interested in
    if (hideModule && strcmp(hideModule, CONTEXT_LOG_MODULE_NAME) == 0) {
        killOutput();
        return;
    }
    
    if (! hideTimestamps)
        *this << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << " ";
        
    *this << "[" << CONTEXT_LOG_MODULE_NAME << "]" << " ";
    *this << msgTypeString << " ";
    *this << file << "[" << line << "]" << ":" << func << " ";
}

void ContextRealLogger::killOutput()
{
    if (nullDevice)
        return;
            
    nullDevice = new NullIODevice();
    setDevice(new NullIODevice());
    return;
}
    
ContextRealLogger::~ContextRealLogger()
{
    *this << "\n";
    if (nullDevice)
        delete nullDevice;
}