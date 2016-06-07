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

#include <QDir>
#include <QDebug>
#include <QApplication>

#include "dashboards.h"

// *INDENT-OFF*
#if defined Q_OS_WIN
#include <windows.h>
#define IS_64_BIT true
#if _WIN32
#undef  IS_64_BIT
#define IS_64_BIT GetProcAddress (GetModuleHandle (TEXT ("kernel32")), "IsWow64Process")
#endif
#define PROGRAM_FILES IS_64_BIT ? "C:/Program Files (x86)" : "C:/Program Files"
#endif
// *INDENT-ON*

/* Dashboard open commands */
const QString LVD_COMMAND = "\"%1/FRC Dashboard/Dashboard.exe\"";
const QString SFX_COMMAND = "java -jar \"%2/wpilib/tools/sfx.jar\"";
const QString SBD_COMMAND = "java -jar \"%2/wpilib/tools/SmartDashboard.jar\"";

/**
 * Configures the application to close the dashboard when it quits
 */
Dashboards::Dashboards()
{
    connect (qApp, SIGNAL (aboutToQuit()), &m_process, SLOT (terminate()));
}

/**
 * Returns a list with the available dashboards.
 * \note This list may differ from operating system to operating system
 */
QStringList Dashboards::dashboardList()
{
    QStringList list;
    list.append (tr ("None"));
    list.append (tr ("SFX Dashboard"));
    list.append (tr ("SmartDashboard"));

#if defined Q_OS_WIN
    list.append (tr ("LabVIEW Dashboard"));
#endif

    return list;
}

/**
 * Opens the given \a dashboard process
 */
void Dashboards::openDashboard (int dashboard)
{
    m_process.close();

    switch (dashboard) {
    case kSFXDashboard:
        m_process.start (SFX_COMMAND.arg (QDir::homePath()), QIODevice::ReadOnly);
        break;
    case kSmartDashboard:
        m_process.start (SBD_COMMAND.arg (QDir::homePath()), QIODevice::ReadOnly);
        break;
#if defined Q_OS_WIN
    case kLabVIEWDashboard:
        m_process.start (LVD_COMMAND.arg (PROGRAM_FILES), QIODevice::ReadOnly);
        break;
#endif
    }
}
