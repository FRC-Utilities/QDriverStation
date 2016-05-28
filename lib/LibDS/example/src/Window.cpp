/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#include "Window.h"
#include "ui_Window.h"

#include <QMessageBox>

Window::Window (QWidget* parent) : QMainWindow (parent)
{
    m_ui = new Ui::Window;
    m_driverStation = DriverStation::getInstance();
    m_ui->setupUi (this);

    /* Set window geometry */
    move (100, 100);
    setFixedSize (minimumSizeHint());

    /* Set window title to {OS} Driver Station */
#if defined (Q_OS_LINUX)
    setWindowTitle (tr ("Linux Driver Station"));
#elif defined (Q_OS_MAC)
    setWindowTitle (tr ("Mac OSX Driver Station"));
#elif defined (Q_OS_WIN)
    setWindowTitle (tr ("Windows Driver Station"));
#endif

    /* Fill the UI items with LibDS options */
    m_ui->Protocols->addItems (m_driverStation->protocols());
    m_ui->TeamStation->addItems (m_driverStation->teamStations());

    /* Update team number when spinbox value is changed */
    connect (m_ui->TeamNumber, SIGNAL (valueChanged (int)),
             m_driverStation,    SLOT (setTeam      (int)));

    /* Enable or disable the robot when enable/disable buttons are clicked */
    connect (m_ui->Enable,  SIGNAL (clicked()),
             this,            SLOT (onEnabledChanged()));
    connect (m_ui->Disable, SIGNAL (clicked()),
             this,            SLOT (onEnabledChanged()));

    /* Update battery voltage */
    connect (m_driverStation, SIGNAL (voltageChanged (QString)),
             this,              SLOT (updateVoltage  (QString)));

    /* Update robot status */
    connect (m_driverStation, SIGNAL (statusChanged (QString)),
             m_ui->Status,      SLOT (setText       (QString)));

    /* Update NetConsole widget */
    connect (m_driverStation, SIGNAL (newMessage (QString)),
             m_ui->Console,     SLOT (append     (QString)));

    /* Update protocol when combobox index is changed */
    connect (m_ui->Protocols, SIGNAL (currentIndexChanged (int)),
             m_driverStation,   SLOT (setProtocolType     (int)));

    /* Update control mode */
    connect (m_ui->Test,         SIGNAL (clicked()),
             this,                 SLOT (updateControlMode()));
    connect (m_ui->Autonomous,   SIGNAL (clicked()),
             this,                 SLOT (updateControlMode()));
    connect (m_ui->Teleoperated, SIGNAL (clicked()),
             this,                 SLOT (updateControlMode()));

    /* Update team station when combobox index is changed */
    connect (m_ui->TeamStation, SIGNAL (currentIndexChanged (int)),
             m_driverStation,     SLOT (setTeamStation      (int)));

    /* Initialize protocol & DS */
    m_driverStation->init();
    m_driverStation->setProtocolType (m_ui->Protocols->currentIndex());
}

Window::~Window()
{
    delete m_ui;
}

void Window::onEnabledChanged()
{
    /* User clicked on the enable button */
    if (m_ui->Enable->isChecked()) {
        /* The DS allows us to enable the robot */
        if (m_driverStation->canBeEnabled())
            m_driverStation->enableRobot();

        /* There is something that does not permit us to enable the robot */
        else {
            m_driverStation->disableRobot();
            m_ui->Enable->setChecked (false);
            m_ui->Disable->setChecked (true);
            QMessageBox::warning (this,
                                  windowTitle(),
                                  tr ("You cannot enable the robot with the "
                                      "current conditions!"),
                                  QMessageBox::Ok);
        }
    }

    /* User clicked on disable button */
    else
        m_driverStation->disableRobot();
}

void Window::updateControlMode()
{
    DS_Common::ControlMode mode;

    if (m_ui->Test->isChecked())
        mode = DS_Common::kControlTest;
    if (m_ui->Autonomous->isChecked())
        mode = DS_Common::kControlAutonomous;
    if (m_ui->Teleoperated->isChecked())
        mode = DS_Common::kControlTeleoperated;

    m_driverStation->setControlMode (mode);
}

void Window::updateVoltage (QString voltage)
{
    /* We are connected to robot, display voltage */
    if (m_driverStation->isConnectedToRobot())
        m_ui->Voltage->setText (voltage);

    /* No robot comms, display "--.--" */
    else
        m_ui->Voltage->setText ("--.--");
}
