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

#ifndef _QDS_MW_DIAGNOSTICS_H
#define _QDS_MW_DIAGNOSTICS_H

#include <QWidget>
#include <QPointer>

class QLabel;
class QCheckBox;
class QGroupBox;
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QGridLayout;

class Diagnostics : public QWidget {
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
    QPointer<QLabel> m_networkDiagnostics;
    QPointer<QCheckBox> m_ethernetLink;
    QPointer<QCheckBox> m_radio;
    QPointer<QCheckBox> m_bridge;
    QPointer<QCheckBox> m_robot;
    QPointer<QCheckBox> m_fms;

    QPointer<QGroupBox> m_systemMonitor;
    QPointer<QWidget> m_resetButtons;
    QPointer<QWidget> m_indicatorsWidget;
    QPointer<QWidget> m_diagnosticsWidget;
    QPointer<QWidget> m_networkDiagWidget;
    QPointer<QGroupBox> m_softwareVersions;

    QPointer<QLabel> m_cpuUsage;
    QPointer<QLabel> m_ramUsage;
    QPointer<QLabel> m_diskUsage;
    QPointer<QLabel> m_pdpVersion;
    QPointer<QLabel> m_pcmVersion;
    QPointer<QLabel> m_libVersion;
    QPointer<QLabel> m_rioVersion;

    QPointer<QLabel> m_cpuUsageLabel;
    QPointer<QLabel> m_ramUsageLabel;
    QPointer<QLabel> m_diskUsageLabel;
    QPointer<QLabel> m_pdpVersionLabel;
    QPointer<QLabel> m_pcmVersionLabel;
    QPointer<QLabel> m_libraryVersionLabel;
    QPointer<QLabel> m_controllerVersionLabel;

    QPointer<QPushButton> m_reboot;
    QPointer<QPushButton> m_restart;

    QPointer<QHBoxLayout> m_mainLayout;
    QPointer<QVBoxLayout> m_resetLayout;
    QPointer<QVBoxLayout> m_indicatorsLayout;
    QPointer<QVBoxLayout> m_diagnosticsLayout;
    QPointer<QVBoxLayout> m_networkDiagLayout;
    QPointer<QGridLayout> m_systemMonitorLayout;
    QPointer<QGridLayout> m_softwareVersionsLayout;
};

#endif
