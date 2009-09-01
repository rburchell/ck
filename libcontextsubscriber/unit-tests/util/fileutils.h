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

#include <QDir>
#include <QFile>
#include <QTest>
#include <stdlib.h>

#define LOCAL_DIR (utilPathForLocalDir())
#define LOCAL_FILE(f) (utilPathForLocalFile(f))
#define DEFAULT_WAIT_PERIOD 100

QString utilPathForLocalFile(QString fname)
{
    const char *srcdir = getenv("srcdir");
    if (srcdir) {
        return QString(srcdir) + QDir::separator() + fname;
    } else {
        return QString("./") + QString(fname);
    }
}

QString utilPathForLocalDir()
{
    const char *srcdir = getenv("srcdir");
    if (srcdir) {
        QDir dir = QDir(srcdir);
        return dir.absolutePath() + "/";
    } else {
        return QString("./");
    }
}

void utilSetEnv(QString env, QString val)
{
    setenv(env.toUtf8().constData(), val.toUtf8().constData(), 0);
}

void utilCopyLocalWithRemove(QString src, QString dest, int waitPeriod = DEFAULT_WAIT_PERIOD)
{
    src = LOCAL_FILE(src);

    QFile::remove(dest);
    QFile::copy(src, dest);

    if (waitPeriod > 0)
        QTest::qWait(waitPeriod);
}

void utilCopyLocalAtomically(QString src, QString dest, int waitPeriod = DEFAULT_WAIT_PERIOD)
{
    src = LOCAL_FILE(src);
    QString temp = "temp.file";

    QFile::copy(src, temp);
    rename(temp.toUtf8().constData(), dest.toUtf8().constData());

    if (waitPeriod > 0)
        QTest::qWait(waitPeriod);
}
