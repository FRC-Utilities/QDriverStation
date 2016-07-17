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

#include <QProcess>

#if defined Q_OS_LINUX
    #include <QPair>
#endif

class QSettings;

/**
 * \brief Provides CPU and Battery information to the QML interface
 */
class Utilities : public QObject {
    Q_OBJECT

  public:
    explicit Utilities();

    Q_INVOKABLE int cpuUsage();
    Q_INVOKABLE int batteryLevel();
    Q_INVOKABLE qreal scaleRatio();
    Q_INVOKABLE bool isConnectedToAC();

  public slots:
    void copy (const QVariant& data);
    void setAutoScaleEnabled (const bool enabled);

  private slots:
    void updateCpuUsage();
    void updateBatteryLevel();
    void updateConnectedToAC();
    void calculateScaleRatio();
    void readCpuUsageProcess (int exit_code = 0);
    void readBatteryLevelProcess (int exit_code = 0);
    void readConnectedToACProcess (int exit_code = 0);

  private:
#if defined (Q_OS_LINUX)
    QPair<quint64, quint64> getCpuJiffies();
    QPair<quint64, quint64> m_pastCpuJiffies {0, 0};
#endif

    qreal m_ratio;
    int m_cpuUsage;
    int m_batteryLevel;
    bool m_connectedToAC;

    QSettings* m_settings;
    QProcess m_cpuProcess;
    QProcess m_batteryLevelProcess;
    QProcess m_connectedToACProcess;
};

#endif
