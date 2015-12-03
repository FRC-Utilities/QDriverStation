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

#pragma once
#ifndef _QDRIVER_STATION_MAINWINDOW_DASHBOARDS_H
#define _QDRIVER_STATION_MAINWINDOW_DASHBOARDS_H

#include <QProcess>

/**
 * @class Dashboard
 * @brief Allows the application to launch different FRC Dashboards
 * @warning Support for the LabVIEW dashboard is not yet complete!
 *
 * The \c Dashboard class implements a simple way to launch the different
 * FRC Dashboards that are availale to each operating system.
 * The Dashboard application is launched as a child process, which allows
 * us to quit the Dashboard when the Open DriverStation is closed or when the
 * user selects another Dashboard type.
 */
class Dashboard : public QObject {
    Q_OBJECT

  public:
    /**
     * Represents the available dashboard options to the user
     */
    enum Dashboards {
        kNone = 0x0,
        kSfxDashboard = 0x1,
        kSmartDashboard = 0x2,
        kLabVIEW = 0x3
    };

    /**
     * Returns the only instance of this class
     */
    static Dashboard* getInstance();

  public slots:
    /**
     * Opens a Dashboard using \c QProcess based on the saved settings
     */
    void loadDashboard();

    /**
     * Quits the current dashboard by using the \c QProcess library
     */
    void quitDashboard();

    /**
     * Quits the current dashboard and opens a new one
     */
    void reloadDashboard();

    /**
     * Returns the currently selected Dashboard
     */
    Dashboards getCurrentDashboard();

    /**
     * Returns an ordered list with the matching Dashboards
     * Modify as needed!
     */
    QStringList getAvailableDashboards();

  protected:
    explicit Dashboard();
    ~Dashboard();

    static Dashboard* s_instance;

  private:
    /**
     * Represents the current dashboard index.
     * This variable can be read with \c getCurrentDashboard()
     */
    Dashboards m_current;

    /**
     * Used for starting - and quiting - the dashboards
     */
    QProcess m_process;
};

#endif
