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

//=============================================================================
// System includes
//=============================================================================

#include <QDir>
#include <QApplication>

//=============================================================================
// Application includes
//=============================================================================

#include "Utilities/Settings.h"
#include "Dashboards/Dashboards.h"

//=============================================================================
// Use "Program Files" or "Program Files (x86)"?
//=============================================================================

#if defined Q_OS_WIN
#include <windows.h>
#define IS_64_BIT true
#if _WIN32
#undef  IS_64_BIT
#define IS_64_BIT GetProcAddress (GetModuleHandle (TEXT ("kernel32")), "IsWow64Process")
#endif
#endif

//=============================================================================
// Global variables
//=============================================================================

Dashboards* Dashboards::s_instance = Q_NULLPTR;

//=============================================================================
// Dashboard::Dashboard
//=============================================================================

Dashboards::Dashboards()
{
    connect (qApp, SIGNAL (aboutToQuit()), this, SLOT (closeDashboard()));
    openDashboard();
}

//=============================================================================
// Dashboard::~Dashboard
//=============================================================================

Dashboards::~Dashboards()
{
    delete s_instance;
}

//=============================================================================
// Dashboard::getInstance
//=============================================================================

Dashboards* Dashboards::getInstance()
{
    if (s_instance == Q_NULLPTR)
        s_instance = new Dashboards();

    return s_instance;
}

//=============================================================================
// Dashboard::currentDashboard
//=============================================================================

Dashboards::DashboardTypes Dashboards::currentDashboard()
{
    return m_current;
}

//=============================================================================
// Dashboard::dashboardList
//=============================================================================

QStringList Dashboards::dashboardList()
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

//=============================================================================
// Dashboard::openDashboard
//=============================================================================

void Dashboards::openDashboard()
{
    QString path;
    m_current = (DashboardTypes) Settings::get ("Dashboard", kNone).toInt();

    /* Open the SFX Dashboard */
    if (m_current == kSfxDashboard)
        {
            QDir dir;
            QStringList files;

            /* Go to the WPILib directory and get the available folders */
            dir.cd (QString ("%1/wpilib/tools/").arg (QDir::homePath()));
            files = dir.entryList (QDir::Dirs);

            /* In theory, the only folder should contain the SFX DB, open it */
            if (files.count() >= 3)
                path = QString ("java -jar \"%1/%2/sfx.jar\"")
                       .arg (dir.absolutePath())
                       .arg (files.at (2));
        }

    /* Open the SmartDashboard, easy as cake */
    else if (m_current == kSmartDashboard)
        {
            path = QString ("java -jar \"%1/wpilib/tools/SmartDashboard.jar\"")
                   .arg (QDir::homePath());
        }

    /* Open the LabVIEW Dashboard */
#if defined _WIN32 || defined _WIN64
    else if (m_current == kLabVIEW)
        {
            QString pF = IS_64_BIT ? "C:/Program Files (x86)" : "C:/Program Files";
            path = QString ("%1/FRC Dashboard/Dashboard.exe").arg (pF);
            path = "\"" + path + "\"";
        }
#endif

    m_process.start (path);
}

//=============================================================================
// Dashboard::closeDashboard
//=============================================================================

void Dashboards::closeDashboard()
{
    m_process.close();
}

//=============================================================================
// Dashboard::reloadDashboard
//=============================================================================

void Dashboards::reloadDashboard()
{
    closeDashboard();
    openDashboard();
}
