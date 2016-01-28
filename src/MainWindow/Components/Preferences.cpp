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
#include <QSpinBox>
#include <QGroupBox>
#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>

//=============================================================================
// Application includes
//=============================================================================

#include "Preferences.h"
#include "Utilities/Global.h"
#include "Utilities/Settings.h"
#include "Dashboards/Dashboards.h"

//=============================================================================
// Preferences::Preferences
//=============================================================================

Preferences::Preferences (QWidget* parent) : QWidget (parent)
{
    createWidgets();
    createLayouts();

    readSettings();
    connectSlots();
}

//=============================================================================
// Preferences::createWidgets
//=============================================================================

void Preferences::createWidgets()
{
    m_protocolLabel   = new QLabel (tr ("Protocol"),    this);
    m_dashboardLabel  = new QLabel (tr ("Dashboard"),   this);
    m_teamNumberLabel = new QLabel (tr ("Team Number"), this);

    m_teamNumber      = new QSpinBox  (this);
    m_protocols       = new QComboBox (this);
    m_dashboards      = new QComboBox (this);

    m_practiceTimings = new QGroupBox (tr ("Practice Timings"), this);

    m_delayLabel      = new QLabel (tr ("Delay"));
    m_teleopLabel     = new QLabel (tr ("Teleop"));
    m_endGameLabel    = new QLabel (tr ("End game"));
    m_countdownLabel  = new QLabel (tr ("Countdown"));
    m_autonomousLabel = new QLabel (tr ("Autonomous"));

    m_delay           = new QSpinBox (this);
    m_teleop          = new QSpinBox (this);
    m_endGame         = new QSpinBox (this);
    m_countdown       = new QSpinBox (this);
    m_autonomous      = new QSpinBox (this);

    m_delay->setRange      (0, 500);
    m_teleop->setRange     (0, 500);
    m_endGame->setRange    (0, 500);
    m_countdown->setRange  (0, 500);
    m_autonomous->setRange (0, 500);
    m_teamNumber->setRange (0, 9999);

    m_protocols->addItems  (DS()->protocols());
    m_dashboards->addItems (Dashboards::getInstance()->dashboardList());
    m_dashboards->setCurrentIndex (Settings::get ("Dashboard", 0).toInt());
}

//=============================================================================
// Preferences::createLayouts
//=============================================================================

void Preferences::createLayouts()
{
    QSpacerItem* spacer = new QSpacerItem  (5, 5,
                                            QSizePolicy::Minimum,
                                            QSizePolicy::MinimumExpanding);
    /* Left widgets */
    m_leftContainer  = new QWidget         (this);
    m_leftLayout     = new QVBoxLayout     (m_leftContainer);
    m_leftLayout->setSpacing               (DPI_SCALE (5));
    m_leftLayout->addWidget                (m_teamNumberLabel);
    m_leftLayout->addWidget                (m_teamNumber);
    m_leftLayout->addWidget                (m_dashboardLabel);
    m_leftLayout->addWidget                (m_dashboards);
    m_leftLayout->addWidget                (m_protocolLabel);
    m_leftLayout->addWidget                (m_protocols);
    m_leftLayout->addSpacerItem            (spacer);

    /* Practice timings */
    m_practiceLayout = new QGridLayout     (m_practiceTimings);
    m_practiceLayout->setVerticalSpacing   (0);
    m_practiceLayout->setHorizontalSpacing (DPI_SCALE (5));
    m_practiceLayout->addWidget            (m_countdownLabel,  0, 0);
    m_practiceLayout->addWidget            (m_autonomousLabel, 1, 0);
    m_practiceLayout->addWidget            (m_delayLabel,      2, 0);
    m_practiceLayout->addWidget            (m_teleopLabel,     3, 0);
    m_practiceLayout->addWidget            (m_endGameLabel,    4, 0);
    m_practiceLayout->addWidget            (m_countdown,       0, 1);
    m_practiceLayout->addWidget            (m_autonomous,      1, 1);
    m_practiceLayout->addWidget            (m_delay,           2, 1);
    m_practiceLayout->addWidget            (m_teleop,          3, 1);
    m_practiceLayout->addWidget            (m_endGame,         4, 1);
    m_practiceLayout->setContentsMargins   (MAIN_MARGINS());

    /* Main layout */
    m_mainLayout = new QHBoxLayout         (this);
    m_mainLayout->setSpacing               (DPI_SCALE (5));
    m_mainLayout->addWidget                (m_leftContainer);
    m_mainLayout->addWidget                (m_practiceTimings);
    m_mainLayout->setContentsMargins       (MAIN_MARGINS());
}

//=============================================================================
// Preferences::connectSlots
//=============================================================================

void Preferences::connectSlots()
{
    connect (m_protocols,  SIGNAL (currentIndexChanged (int)),
             this,           SLOT (setProtocol         (int)));
    connect (m_dashboards, SIGNAL (currentIndexChanged (int)),
             this,           SLOT (setDashboard        (int)));
    connect (m_teamNumber, SIGNAL (valueChanged        (int)),
             this,           SLOT (setTeamNumber       (int)));
    connect (m_delay,      SIGNAL (valueChanged        (int)),
             this,           SLOT (savePracticeValues  (int)));
    connect (m_teleop,     SIGNAL (valueChanged        (int)),
             this,           SLOT (savePracticeValues  (int)));
    connect (m_endGame,    SIGNAL (valueChanged        (int)),
             this,           SLOT (savePracticeValues  (int)));
    connect (m_countdown,  SIGNAL (valueChanged        (int)),
             this,           SLOT (savePracticeValues  (int)));
    connect (m_autonomous, SIGNAL (valueChanged        (int)),
             this,           SLOT (savePracticeValues  (int)));
    connect (DS(),         SIGNAL (teamChanged         (int)),
             m_teamNumber,   SLOT (setValue            (int)));
}

//=============================================================================
// Preferences::readSettings
//=============================================================================

void Preferences::readSettings()
{
    /* Load saved values in widgets */
    m_teamNumber->setValue       (Settings::get ("Team",     0).toInt());
    m_protocols->setCurrentIndex (Settings::get ("Protocol", 0).toInt());

    /* Redundant but safer, configure DS again directly */
    setProtocol (m_protocols->currentIndex());
    setTeamNumber (m_teamNumber->value());

    /* Load the practice timings into the UI */
    loadPracticeValues();
}

//=============================================================================
// Preferences::setProtocol
//=============================================================================

void Preferences::setProtocol (int index)
{
    Settings::set ("Protocol", index);
    DS()->setProtocol ((DriverStation::ProtocolType) index);
}

//=============================================================================
// Preferences::setDashboard
//=============================================================================

void Preferences::setDashboard (int index)
{
    Settings::set ("Dashboard", index);
    Dashboards::getInstance()->reloadDashboard();
}

//=============================================================================
// Preferences::setTeamNumber
//=============================================================================

void Preferences::setTeamNumber (int team)
{
    DS()->setTeamNumber (team);
    Settings::set ("Team", team);
}

//=============================================================================
// Preferences::loadPracticeValues
//=============================================================================

void Preferences::loadPracticeValues()
{
    m_delay->setValue      (Settings::get ("Practice delay",       1).toInt());
    m_teleop->setValue     (Settings::get ("Practice teleop",    100).toInt());
    m_endGame->setValue    (Settings::get ("Practice end game",   20).toInt());
    m_countdown->setValue  (Settings::get ("Practice countdown",   5).toInt());
    m_autonomous->setValue (Settings::get ("Practice autonomous", 15).toInt());
}

//=============================================================================
// Preferences::savePracticeValues
//=============================================================================

void Preferences::savePracticeValues (int unused)
{
    Q_UNUSED (unused);
    Settings::set ("Practice delay",       m_delay->value());
    Settings::set ("Practice teleop",      m_teleop->value());
    Settings::set ("Practice end game",    m_endGame->value());
    Settings::set ("Practice countdown",   m_countdown->value());
    Settings::set ("Practice autonomous",  m_autonomous->value());
}
