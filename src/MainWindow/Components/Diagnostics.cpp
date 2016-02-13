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

#include <QLabel>
#include <QGroupBox>
#include <QCheckBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>

//=============================================================================
// System includes
//=============================================================================

#include "Diagnostics.h"
#include "Global/Global.h"

//=============================================================================
// Diagnostics::Diagnostics
//=============================================================================

Diagnostics::Diagnostics (QWidget* parent) : QWidget (parent)
{
    createWidgets();
    createLayouts();
    connectSlots();
}

//=============================================================================
// Diagnostics::createWidgets
//=============================================================================

void Diagnostics::createWidgets()
{
    /* Diagnostic checkboxes */
    m_ethernetLink           = new QCheckBox (tr ("Enet Link"), this);
    m_radio                  = new QCheckBox (tr ("DS Radio"), this);
    m_bridge                 = new QCheckBox (tr ("Bridge"), this);
    m_robot                  = new QCheckBox (tr ("Robot"), this);
    m_fms                    = new QCheckBox (tr ("FMS"), this);
    m_networkDiagnostics     = new QLabel (tr ("Network Diagnostics"), this);

    /* Robot information boxes */
    m_indicatorsWidget       = new QWidget (this);
    m_resetButtons           = new QWidget (this);
    m_diagnosticsWidget      = new QWidget (this);
    m_networkDiagWidget      = new QWidget (this);
    m_systemMonitor          = new QGroupBox (tr ("System Monitor"), this);
    m_softwareVersions       = new QGroupBox (tr ("Software Versions"), this);

    /* Robot information indicators */
    m_cpuUsage               = new QLabel (NO_DATA, this);
    m_ramUsage               = new QLabel (NO_DATA, this);
    m_diskUsage              = new QLabel (NO_DATA, this);
    m_pdpVersion             = new QLabel (NO_DATA, this);
    m_pcmVersion             = new QLabel (NO_DATA, this);
    m_libVersion             = new QLabel (NO_DATA, this);
    m_rioVersion             = new QLabel (NO_DATA, this);

    /* Robot information caption labels */
    m_cpuUsageLabel          = new QLabel (tr ("CPU")     + ":", this);
    m_ramUsageLabel          = new QLabel (tr ("RAM")     + ":", this);
    m_diskUsageLabel         = new QLabel (tr ("Disk")    + ":", this);
    m_pdpVersionLabel        = new QLabel (tr ("PDB/PDP") + ":", this);
    m_pcmVersionLabel        = new QLabel (tr ("PCM")     + ":", this);
    m_libraryVersionLabel    = new QLabel (tr ("LIB")     + ":", this);
    m_controllerVersionLabel = new QLabel (tr ("RIO")     + ":", this);

    /* Reboot & restart code buttons */
    m_reboot                 = new QPushButton (tr ("Reboot RIO"));
    m_restart                = new QPushButton (tr ("Restart Code"));

    /* Disable the checkboxes */
    m_ethernetLink->setEnabled             (false);
    m_radio->setEnabled                    (false);
    m_bridge->setEnabled                   (false);
    m_robot->setEnabled                    (false);
    m_fms->setEnabled                      (false);

    /* Align caption labels */
    m_cpuUsageLabel->setAlignment          (Qt::AlignRight);
    m_ramUsageLabel->setAlignment          (Qt::AlignRight);
    m_diskUsageLabel->setAlignment         (Qt::AlignRight);
    m_pdpVersionLabel->setAlignment        (Qt::AlignRight);
    m_pcmVersionLabel->setAlignment        (Qt::AlignRight);
    m_libraryVersionLabel->setAlignment    (Qt::AlignRight);
    m_controllerVersionLabel->setAlignment (Qt::AlignRight);

    /* Center robot information indicators */
    m_cpuUsage->setAlignment               (Qt::AlignCenter);
    m_ramUsage->setAlignment               (Qt::AlignCenter);
    m_diskUsage->setAlignment              (Qt::AlignCenter);
    m_pdpVersion->setAlignment             (Qt::AlignCenter);
    m_pcmVersion->setAlignment             (Qt::AlignCenter);
    m_libVersion->setAlignment             (Qt::AlignCenter);
    m_rioVersion->setAlignment             (Qt::AlignCenter);
}

//=============================================================================
// Diagnostics::createLayouts
//=============================================================================

void Diagnostics::createLayouts()
{
    /* Arrange network diagnostics widgets */
    m_networkDiagLayout = new QVBoxLayout          (m_networkDiagWidget);
    m_networkDiagLayout->setSpacing                (DPI_SCALE (1));
    m_networkDiagLayout->addWidget                 (m_networkDiagnostics);
    m_networkDiagLayout->addWidget                 (m_ethernetLink);
    m_networkDiagLayout->addWidget                 (m_radio);
    m_networkDiagLayout->addWidget                 (m_bridge);
    m_networkDiagLayout->addWidget                 (m_robot);
    m_networkDiagLayout->addWidget                 (m_fms);

    /* Arrange the buttons */
    m_resetLayout = new QVBoxLayout                (m_resetButtons);
    m_resetLayout->setSpacing                      (DPI_SCALE (3));
    m_resetLayout->addWidget                       (m_reboot);
    m_resetLayout->addWidget                       (m_restart);

    /* Arrange software versions indicators */
    m_softwareVersionsLayout = new QGridLayout     (m_softwareVersions);
    m_softwareVersionsLayout->setVerticalSpacing   (0);
    m_softwareVersionsLayout->setHorizontalSpacing (DPI_SCALE (5));
    m_softwareVersionsLayout->addWidget            (m_controllerVersionLabel, 0, 0);
    m_softwareVersionsLayout->addWidget            (m_libraryVersionLabel,    1, 0);
    m_softwareVersionsLayout->addWidget            (m_pcmVersionLabel,        2, 0);
    m_softwareVersionsLayout->addWidget            (m_pdpVersionLabel,        3, 0);
    m_softwareVersionsLayout->addWidget            (m_rioVersion,             0, 1);
    m_softwareVersionsLayout->addWidget            (m_libVersion,             1, 1);
    m_softwareVersionsLayout->addWidget            (m_pcmVersion,             2, 1);
    m_softwareVersionsLayout->addWidget            (m_pdpVersion,             3, 1);

    /* Arrange system monitor indicators */
    m_systemMonitorLayout = new QGridLayout        (m_systemMonitor);
    m_systemMonitorLayout->setVerticalSpacing      (0);
    m_systemMonitorLayout->setHorizontalSpacing    (DPI_SCALE (5));
    m_systemMonitorLayout->addWidget               (m_cpuUsageLabel,  0, 0);
    m_systemMonitorLayout->addWidget               (m_ramUsageLabel,  1, 0);
    m_systemMonitorLayout->addWidget               (m_diskUsageLabel, 2, 0);
    m_systemMonitorLayout->addWidget               (m_cpuUsage,       0, 1);
    m_systemMonitorLayout->addWidget               (m_ramUsage,       1, 1);
    m_systemMonitorLayout->addWidget               (m_diskUsage,      2, 1);

    /* Arrange the robot diagnostics layout */
    m_diagnosticsLayout = new QVBoxLayout          (m_diagnosticsWidget);
    m_diagnosticsLayout->setSpacing                (DPI_SCALE (1));
    m_diagnosticsLayout->addWidget                 (m_networkDiagWidget);
    m_diagnosticsLayout->addWidget                 (m_resetButtons);

    /* Arrange the robot indicators layout */
    m_indicatorsLayout = new QVBoxLayout           (m_indicatorsWidget);
    m_indicatorsLayout->setSpacing                 (DPI_SCALE (1));
    m_indicatorsLayout->addWidget                  (m_softwareVersions);
    m_indicatorsLayout->addWidget                  (m_systemMonitor);

    /* Arrange the main layout */
    m_mainLayout = new QHBoxLayout                 (this);
    m_mainLayout->setSpacing                       (DPI_SCALE (0));
    m_mainLayout->addWidget                        (m_diagnosticsWidget);
    m_mainLayout->addWidget                        (m_indicatorsWidget);

    /* Set container margins */
    m_mainLayout->setContentsMargins               (SMALL_MARGINS());
    m_resetLayout->setContentsMargins              (SMALL_MARGINS());
    m_networkDiagLayout->setContentsMargins        (SMALL_MARGINS());
    m_softwareVersionsLayout->setContentsMargins   (SMALL_MARGINS());
    m_systemMonitorLayout->setContentsMargins      (SMALL_MARGINS());
}

//=============================================================================
// Diagnostics::connectsSlots
//=============================================================================

void Diagnostics::connectSlots()
{
    connect (DS(),         &DriverStation::radioChanged,
             m_radio,      &QCheckBox::setChecked);
    connect (DS(),         &DriverStation::radioChanged,
             m_bridge,     &QCheckBox::setChecked);
    connect (DS(),         &DriverStation::fmsChanged,
             m_fms,        &QCheckBox::setChecked);
    connect (DS(),         &DriverStation::rioVersionChanged,
             m_rioVersion, &QLabel::setText);
    connect (DS(),         &DriverStation::pdpVersionChanged,
             m_pdpVersion, &QLabel::setText);
    connect (DS(),         &DriverStation::pcmVersionChanged,
             m_pcmVersion, &QLabel::setText);
    connect (DS(),         &DriverStation::libVersionChanged,
             m_libVersion, &QLabel::setText);
    connect (DS(),         &DriverStation::cpuUsageChanged,
             this,         &Diagnostics::updateCpuUsage);
    connect (DS(),         &DriverStation::ramUsageChanged,
             this,         &Diagnostics::updateRamUsage);
    connect (DS(),         &DriverStation::diskUsageChanged,
             this,         &Diagnostics::updateDiskUsage);
    connect (m_reboot,     &QPushButton::clicked,
             DS(),         &DriverStation::rebootRobot);
    connect (m_restart,    &QPushButton::clicked,
             DS(),         &DriverStation::restartCode);

    /* Lambda-functions */
    connect (DS(), &DriverStation::communicationsChanged,
             [ = ] (DS_CommStatus status)
    {
        m_robot->setChecked (status == kFull);
    });
}

//=============================================================================
// Diagnostics::updateCpuUsage
//=============================================================================

void Diagnostics::updateCpuUsage (int usage)
{
    m_cpuUsage->setText (tr ("%1 %").arg (usage));
}

//=============================================================================
// Diagnostics::updateRamUsage
//=============================================================================

void Diagnostics::updateRamUsage (int usage)
{
    m_ramUsage->setText (tr ("%1 MB").arg (usage));
}

//=============================================================================
// Diagnostics::updateDiskUsage
//=============================================================================

void Diagnostics::updateDiskUsage (int usage)
{
    m_diskUsage->setText (tr ("%1 MB").arg (usage));
}
