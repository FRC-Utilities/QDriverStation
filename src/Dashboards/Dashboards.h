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

#ifndef _QDS_DASHBOARDS_H
#define _QDS_DASHBOARDS_H

#include <QProcess>

class Dashboards : public QObject
{
    Q_OBJECT

public:
    enum DashboardTypes
    {
        kNone           = 0x00,
        kQDashboard     = 0x01,
        kSfxDashboard   = 0x02,
        kSmartDashboard = 0x03,
        kLabVIEW        = 0x04,
    };

    static Dashboards* getInstance();

    QStringList dashboardList();
    DashboardTypes currentDashboard();

public slots:
    void openDashboard();
    void closeDashboard();
    void reloadDashboard();

signals:
    void dashboardChanged();

protected:
    explicit Dashboards();

private:
    QProcess m_process;
    DashboardTypes m_current;
};

#endif
