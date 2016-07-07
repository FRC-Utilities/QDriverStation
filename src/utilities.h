/*
 * Copyright (c) 2015-2016 Alex Spataru <alex_spataru@outlook.com>
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

#ifndef _QDS_UTILITIES_H
#define _QDS_UTILITIES_H

#include <stdint.h>

#include <utility>

#include <QProcess>

class Utilities : public QObject {
    Q_OBJECT

  public:
    explicit Utilities();

    Q_INVOKABLE int cpuUsage();
    Q_INVOKABLE int batteryLevel();
    Q_INVOKABLE bool isConnectedToAC();

  public slots:
    void copy (const QVariant& data);

  private slots:
    void updateCpuUsage();
    void updateBatteryLevel();
    void updateConnectedToAC();
    void readCpuUsageProcess (int exit_code = 0);
    void readBatteryLevelProcess (int exit_code = 0);
    void readConnectedToACProcess (int exit_code = 0);

  private:
#if defined (Q_OS_LINUX)
    std::pair<uint64_t, uint64_t> getCpuJiffies();
    std::pair<uint64_t, uint64_t> m_pastCpuJiffies{0, 0};
#endif

    int m_cpuUsage;
    int m_batteryLevel;
    bool m_connectedToAC;

    QProcess m_cpuProcess;
    QProcess m_batteryLevelProcess;
    QProcess m_connectedToACProcess;
};

#endif
