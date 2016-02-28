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

#ifndef _QDS_DASHBOARDS_H
#define _QDS_DASHBOARDS_H

#include <QProcess>

///
/// This class is in charge of opening, managing and closing
/// an FRC dashboard process.
///
/// There is no hidden magic here, we just use the \c QProcess
/// class to open and close each Dashboard.
///
class Dashboards : public QObject {
    Q_OBJECT

  public:
    ///
    /// Defines the available dashboards to the QDriverStation
    ///
    enum DashboardTypes {
        kNone           = 0,
        kSFXDashboard   = 1,
        kSmartDashboard = 2,
#if defined Q_OS_WIN
        kLabVIEW        = 3,
#endif
    };

    ///
    /// Returns the only instance of this class
    ///
    static Dashboards* getInstance();

    ///
    /// Returns a list with the available dashboards.
    /// This list must have the same order as the
    /// values specified in the \c DashboardTypes enum.
    ///
    QStringList dashboardList();

    ///
    /// Returns the current dashboard being used.
    ///
    DashboardTypes currentDashboard();

  public slots:
    ///
    /// Uses \c QProcess to open the selected dashboard
    /// process.
    ///
    /// Note that the dashboard to open is determined based
    /// on the saved settings.
    ///
    void openDashboard();

    ///
    /// Closes the current dashboard process.
    ///
    void closeDashboard();

    ///
    /// Calls \c closeDashboard() and subsequently \c openDashboard().
    /// This function is called after the user selected another
    /// dashboard in the QDriverStation UI.
    ///
    void reloadDashboard();

  signals:
    ///
    /// Emitted when the dashboard process is reloaded (duh)
    ///
    void dashboardChanged();

  protected:
    explicit Dashboards();

  private:
    QProcess m_process;
    DashboardTypes m_current;
};

#endif
