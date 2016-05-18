/*
 * Copyright (c) 2015-2016 WinT 3794 <http://wint3794.org>
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

//==============================================================================
// System includes
//==============================================================================

#include <QtMath>
#include <QMessageBox>
#include <QApplication>
#include <QNetworkRequest>

//==============================================================================
// Application includes
//==============================================================================

#include "updater.h"

//==============================================================================
// Updater::Updater
//==============================================================================

Updater::Updater() {
    m_version = "";
    m_platform = "";
    m_downloadLink = "";
    m_updateAvailable = false;

#if defined Q_OS_WIN
    m_platform = "windows";
#elif defined Q_OS_MAC
    m_platform = "macintosh";
#elif defined Q_OS_LINUX
    m_platform = "linux";
#elif defined Q_OS_ANDROID
    m_platform = "android";
#elif defined Q_OS_IOS
    m_platform = "ios";
#endif

    connect (&m_accessManager, &QNetworkAccessManager::finished,
             this,             &Updater::onServerReply);
}

//==============================================================================
// Updater::showUpdateMessages
//==============================================================================

void Updater::showUpdateMessages() {
    if (m_updateAvailable) {
        QMessageBox box;
        box.setTextFormat (Qt::RichText);
        box.setIcon (QMessageBox::Information);
        box.setStandardButtons (QMessageBox::Ok);
        box.setWindowTitle (tr ("Update Available"));

        box.setText ("<h3>"
                     + tr ("New update available!")
                     + "</h3>");

        box.setInformativeText (tr ("A new version of %1 has been released! "
                                    "Version %2 is now available to download."
                                    "<br/><br/>"
                                    "You can download this version using the link:"
                                    "<br/><br/>"
                                    "%3")
                                .arg (QApplication::applicationName())
                                .arg (QString ("<strong>%1</strong>").arg (m_version))
                                .arg (QString ("<a href=%1>%2</a>").arg (m_downloadLink,
                                        m_downloadLink)));

        box.exec();
    }
}

//==============================================================================
// Updater::checkForUpdates
//==============================================================================

void Updater::checkForUpdates (QString url) {
    m_accessManager.get (QNetworkRequest (QUrl (url)));
}

//==============================================================================
// Updater::onServerReply
//==============================================================================

void Updater::onServerReply (QNetworkReply* reply) {
    QByteArray data = reply->readAll();

    getDownloadLink (data);
    getApplicationVersion (data);
    showUpdateMessages();
}

//==============================================================================
// Updater::getDownloadLink
//==============================================================================

void Updater::getDownloadLink (QByteArray data) {
    m_downloadLink = readKey (QString::fromUtf8 (data),
                              QString ("download-%1").arg (m_platform));
}

//==============================================================================
// Updater::getApplicationVersion
//==============================================================================

void Updater::getApplicationVersion (QByteArray data) {
    m_version = readKey (QString::fromUtf8 (data),
                         QString ("latest-%1").arg (m_platform));

    m_updateAvailable = compare (m_version, QApplication::applicationVersion());
}

//==============================================================================
// Updater::compare
//==============================================================================

bool Updater::compare (QString a, QString b) {
    QStringList versions_a = a.split (".");
    QStringList versions_b = b.split (".");

    int count = qMin (versions_a.count(), versions_b.count());

    for (int i = 0; i < count; ++i) {
        int version_a = QString (versions_a.at (i)).toInt();
        int version_b = QString (versions_b.at (i)).toInt();

        if (version_a > version_b)
            return true;

        else if (version_b > version_a)
            return false;
    }

    return versions_b.count() < versions_a.count();
}

//==============================================================================
// Updater::readKey
//==============================================================================

QString Updater::readKey (QString data, QString key) {
    QString value;
    int startIndex = -1;
    int finishIndex = -1;

    if (data.isEmpty() || key.isEmpty())
        return value;

    startIndex = data.indexOf (QString ("<%1>").arg (key));
    finishIndex = data.indexOf (QString ("</%1>").arg (key));

    if (startIndex != -1 && finishIndex != -1) {
        for (int i = startIndex + key.length() + 2; i < finishIndex; ++i)
            value.append (data.at (i));
    }

    return value;
}
