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

ContextRealLogger::ContextRealLogger(int msgType, const char *func, const char *file, int line) 
    : QTextStream(stderr)
{
    nullDevice = NULL;
    
    static bool hideDebug = (getenv("CONTEXT_LOG_HIDE_DEBUG") != NULL);
    static bool hideWarning = (getenv("CONTEXT_LOG_HIDE_WARNING") != NULL);
    static bool hideCritical = (getenv("CONTEXT_LOG_HIDE_CRITICAL") != NULL);
    static bool hideTest = (getenv("CONTEXT_LOG_HIDE_TEST") != NULL);
    static const char* showModule = getenv("CONTEXT_LOG_SHOW_MODULE");
    static const char* hideModule = getenv("CONTEXT_LOG_HIDE_MODULE");
    static bool hideTimestamps = (getenv("CONTEXT_LOG_HIDE_TIMESTAMPS") != NULL);
    static bool useColor = (getenv("CONTEXT_LOG_USE_COLOR") != NULL);
    
    const char *msgTypeString = NULL;
    
    // Killing by msg type
    if (msgType == CONTEXT_LOG_MSG_TYPE_DEBUG) {
        if (hideDebug) {
            killOutput();
            return;
        } else
            msgTypeString = "DEBUG";
    } else if (msgType == CONTEXT_LOG_MSG_TYPE_WARNING) {
        if (hideWarning) {
            killOutput();
            return;
        } else
            msgTypeString = (useColor) ? "\033[1;33mWARNING\033[0m" : "WARNING";
    } else if (msgType == CONTEXT_LOG_MSG_TYPE_CRITICAL) {
        if (hideCritical) {
            killOutput();
            return;
        } else
            msgTypeString = (useColor) ? "\033[1;31mCRITICAL\033[0m" : "CRITICAL";
    } else if (msgType == CONTEXT_LOG_MSG_TYPE_TEST) {
        if (hideTest) {
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
        *this << QDateTime::currentDateTime().toString() << " ";
        
    *this << "[" << CONTEXT_LOG_MODULE_NAME << "]" << " ";
    *this << msgTypeString << " ";
    *this << func << ":" << file << "[" << line << "]" << " ";
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