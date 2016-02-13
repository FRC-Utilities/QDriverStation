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

#ifndef _QDS_MW_DIAGNOSTICS_H
#define _QDS_MW_DIAGNOSTICS_H

#include <QWidget>

class QLabel;
class QCheckBox;
class QGroupBox;
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QGridLayout;

class Diagnostics : public QWidget
{
    Q_OBJECT

public:
    explicit Diagnostics (QWidget* parent);
    ~Diagnostics();

private slots:
    void createWidgets();
    void createLayouts();
    void connectSlots();

    void updateCpuUsage (int usage);
    void updateRamUsage (int usage);
    void updateDiskUsage (int usage);

private:
    QLabel* m_networkDiagnostics;
    QCheckBox* m_ethernetLink;
    QCheckBox* m_radio;
    QCheckBox* m_bridge;
    QCheckBox* m_robot;
    QCheckBox* m_fms;

    QGroupBox* m_systemMonitor;
    QWidget* m_resetButtons;
    QWidget* m_indicatorsWidget;
    QWidget* m_diagnosticsWidget;
    QWidget* m_networkDiagWidget;
    QGroupBox* m_softwareVersions;

    QLabel* m_cpuUsage;
    QLabel* m_ramUsage;
    QLabel* m_diskUsage;
    QLabel* m_pdpVersion;
    QLabel* m_pcmVersion;
    QLabel* m_libVersion;
    QLabel* m_rioVersion;

    QLabel* m_cpuUsageLabel;
    QLabel* m_ramUsageLabel;
    QLabel* m_diskUsageLabel;
    QLabel* m_pdpVersionLabel;
    QLabel* m_pcmVersionLabel;
    QLabel* m_libraryVersionLabel;
    QLabel* m_controllerVersionLabel;

    QPushButton* m_reboot;
    QPushButton* m_restart;

    QHBoxLayout* m_mainLayout;
    QVBoxLayout* m_resetLayout;
    QVBoxLayout* m_indicatorsLayout;
    QVBoxLayout* m_diagnosticsLayout;
    QVBoxLayout* m_networkDiagLayout;
    QGridLayout* m_systemMonitorLayout;
    QGridLayout* m_softwareVersionsLayout;
};

#endif
