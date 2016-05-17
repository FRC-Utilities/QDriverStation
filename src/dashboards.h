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
    Q_ENUMS (DashboardTypes)

  public:
    explicit Dashboards();

    ///
    /// Represents the available dashboard types
    ///
    enum DashboardTypes {
        kNone = 0,
        kSFXDashboard = 1,
        kSmartDashboard = 2,
        kLabVIEWDashboard = 3,
    };

    ///
    /// Returns a list with the available dashboards.
    /// This list must have the same order as the
    /// values specified in the \c DashboardTypes enum.
    ///
    Q_INVOKABLE QStringList dashboardList();

  public slots:
    ///
    /// Uses \c QProcess to open the selected dashboard process.
    ///
    void openDashboard (int dashboard = 0);

    ///
    /// Closes the current dashboard process.
    ///
    void closeDashboard();

  private:
    QProcess m_process;
};

#endif
