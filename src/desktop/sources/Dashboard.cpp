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

#include <QDir>
#include <QApplication>

#include "Settings.h"
#include "Dashboard.h"

/* Decide whenever to use 'Program Files' or 'Program Files (x86)' */
#if defined _WIN32 || defined _WIN64
#include <windows.h>
#define is64Bits true
#if _WIN32
#undef is64Bits
#define is64Bits \
    GetProcAddress(GetModuleHandle(TEXT("kernel32")),("IsWow64Process"))
#endif
#endif

Dashboard* Dashboard::m_instance = nullptr;

Dashboard::Dashboard()
{
    m_process = new QProcess (this);
    connect (qApp, SIGNAL (aboutToQuit()), this, SLOT (quitDashboard()));

    loadDashboard();
}

Dashboard* Dashboard::getInstance()
{
    if (m_instance == nullptr)
        m_instance = new Dashboard();

    return m_instance;
}

void Dashboard::loadDashboard()
{
    QString path;
    m_current = Settings::get ("Dashboard", None).toInt();

    if (m_current == SfxDashboard) {
        QDir dir;
        QStringList files;

        dir.cd (QString ("%1/wpilib/tools/").arg (QDir::homePath()));
        files = dir.entryList (QDir::Dirs);

        if (files.count() >= 3)
            path = QString ("java -jar \"%1/%2/sfx.jar\"")
                   .arg (dir.absolutePath())
                   .arg (files.at (2));
    }

    else if (m_current == SmartDashboard) {
        path = QString ("java -jar \"%1/wpilib/tools/SmartDashboard.jar\"")
               .arg (QDir::homePath());
    }

#if defined _WIN32 || defined _WIN64
    else if (m_current == LabVIEW) {
        QString pF = is64Bits ? "C:/Program Files (x86)" : "C:/Program Files";
        path = QString ("%1/FRC Dashboard/Dashboard.exe").arg (pF);
        path = "\"" + path + "\"";
        break;
    }
#endif

    m_process->start (path);
}

void Dashboard::quitDashboard()
{
    m_process->close();
}

void Dashboard::reloadDashboard()
{
    quitDashboard();
    loadDashboard();
}

int Dashboard::getCurrentDashboard()
{
    return m_current;
}

QStringList Dashboard::getAvailableDashboards()
{
    QStringList list;
    list.append ("None");
    list.append ("SFX Dashboard");
    list.append ("SmartDashboard");

#if defined _WIN32 || defined _WIN64
    list.append ("LabVIEW Dashboard");
#endif

    return list;
}
