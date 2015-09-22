/*
 * Copyright (c) 2015 WinT 3794 <http://wint3794.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <QNetworkReply>
#include <QNetworkRequest>

#include "VersionAnalyzer.h"

const QString _PCM_FILE ("/tmp/frc_versions/PCM-0-versions.ini");
const QString _PDP_FILE ("/tmp/frc_versions/PDP-0-versions.ini");
const QString _LIB_FILE ("/tmp/frc_versions/FRC_Lib_Version.ini");

DS_VersionAnalyzer::DS_VersionAnalyzer()
{
    connect (&m_manager, SIGNAL (finished (QNetworkReply*)),
             this,      SLOT   (downloadFinished (QNetworkReply*)));
}

void DS_VersionAnalyzer::downloadRobotInformation (QString address)
{
    QString host = "ftp://" + address;
    m_manager.get (QNetworkRequest (host + _LIB_FILE));
    m_manager.get (QNetworkRequest (host + _PCM_FILE));
    m_manager.get (QNetworkRequest (host + _PDP_FILE));
}

void DS_VersionAnalyzer::downloadFinished (QNetworkReply* reply)
{
    /* Get URL and downloaded data */
    QString url = reply->url().toString();
    QString data = QString::fromUtf8 (reply->readAll());

    /* Data or URL is invalid */
    if (data.isEmpty() || url.isEmpty())
        return;

    /* This is the PCM information file */
    else if (url.contains (_PCM_FILE, Qt::CaseInsensitive)) {
        QString version;
        QString key = "currentVersion";

        version.append (data.at (data.indexOf (key) + key.length() + 1));
        version.append (data.at (data.indexOf (key) + key.length() + 2));
        version.append (data.at (data.indexOf (key) + key.length() + 3));
        version.append (data.at (data.indexOf (key) + key.length() + 4));

        emit pcmVersionChanged (version);
    }

    /* This is the PDP information file */
    else if (url.contains (_PDP_FILE, Qt::CaseInsensitive)) {
        QString version;
        QString key = "currentVersion";

        version.append (data.at (data.indexOf (key) + key.length() + 1));
        version.append (data.at (data.indexOf (key) + key.length() + 2));
        version.append (data.at (data.indexOf (key) + key.length() + 3));
        version.append (data.at (data.indexOf (key) + key.length() + 4));

        emit pdpVersionChanged (version);
    }

    /* This is the library version file */
    else if (url.contains (_LIB_FILE, Qt::CaseInsensitive))
        emit libVersionChanged (data);
}
