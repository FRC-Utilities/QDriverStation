/*
 * Copyright (C) 2015-2016 Alex Spataru <alex_spataru@outlook>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "EventLogger.h"

#include <QFile>
#include <QDebug>
#include <QTimer>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#define LOG qDebug() << "DS Events:"

/**
 * Connects the signals/slots between the \c DriverStation and the logger
 */
DSEventLogger::DSEventLogger()
{
    saveData();
    connectSlots();
}

/**
 * Closes the log file
 */
DSEventLogger::~DSEventLogger()
{

}

/**
 * Returns a pointer to the only instance of this class
 */
DSEventLogger* DSEventLogger::getInstance()
{
    static DSEventLogger instance;
    return &instance;
}

/**
 * Saves the current log information to a local file using the JSON format
 * \note This function is called every 5 seconds
 */
void DSEventLogger::saveData()
{
    QTimer::singleShot (500, Qt::PreciseTimer, this, SLOT (saveData()));
}

/**
 * Called when the DS reports a change of the robot's CAN utilization
 */
void DSEventLogger::onCANUsageChanged (int usage)
{
    Q_UNUSED (usage);
}

/**
 * Called when the DS reports a change of the robot's CPU usage
 */
void DSEventLogger::onCPUUsageChanged (int usage)
{
    Q_UNUSED (usage);
}

/**
 * Called when the DS reports a change of the robot's RAM usage
 */
void DSEventLogger::onRAMUsageChanged (int usage)
{
    Q_UNUSED (usage);
}

/**
 * Called when the DS reports a new NetConsole message
 */
void DSEventLogger::onNewMessage (QString message)
{
    Q_UNUSED (message);
}

/**
 * Called when the DS reports a change of the robot's disk usage
 */
void DSEventLogger::onDiskUsageChanged (int usage)
{
    Q_UNUSED (usage);
}

/**
 * Called when the DS reports a change of the enabled status
 */
void DSEventLogger::onEnabledChanged (bool enabled)
{
    LOG << "Robot enabled state set to" << enabled;
}

/**
 * Called when the DS reports a change of the team number
 */
void DSEventLogger::onTeamNumberChanged (int number)
{
    LOG << "Team number set to" << number;
}

/**
 * Called when the DS reports a change in the robot voltage
 */
void DSEventLogger::onVoltageChanged (float voltage)
{
    Q_UNUSED (voltage);
}

/**
 * Called when the DS reports a change in the robot code status
 */
void DSEventLogger::onRobotCodeChanged (bool robotCode)
{
    LOG << "Robot code status set to" << robotCode;
}

/**
 * Called when the DS reports a change in the FMS communications status
 */
void DSEventLogger::onFMSCommunicationsChanged (bool connected)
{
    LOG << "FMS communications set to" << connected;
}

/**
 * Called when the DS reports a change in the radio communications status
 */
void DSEventLogger::onRadioCommunicationsChanged (bool connected)
{
    LOG << "Radio communications set to" << connected;
}

/**
 * Called when the DS reports a change in the robot communications status
 */
void DSEventLogger::onRobotCommunicationsChanged (bool connected)
{
    LOG << "Robot communications set to" << connected;
}

/**
 * Called when the DS reports a change in the emergency stop status
 */
void DSEventLogger::onEmergencyStoppedChanged (bool emergencyStopped)
{
    LOG << "ESTOP set to" << emergencyStopped;
}

/**
 * Called when the DS reports a change in the robot control mode
 */
void DSEventLogger::onControlModeChanged (DriverStation::Control mode)
{
    LOG << "Robot control mode set to" << mode;
}

/**
 * Called when the DS reports a change in the team alliance
 */
void DSEventLogger::onAllianceChanged (DriverStation::Alliance alliance)
{
    LOG << "Team alliance set to" << alliance;
}

/**
 * Called when the DS reports a change in the team position
 */
void DSEventLogger::onPositionChanged (DriverStation::Position position)
{
    LOG << "Team position set to" << position;
}

/**
 * Allows the logger class to react when the DriverStation receives a
 * new event from the LibDS library
 */
void DSEventLogger::connectSlots()
{
    DriverStation* ds = DriverStation::getInstance();

    connect (ds,   &DriverStation::canUsageChanged,
             this, &DSEventLogger::onCANUsageChanged);
    connect (ds,   &DriverStation::cpuUsageChanged,
             this, &DSEventLogger::onCPUUsageChanged);
    connect (ds,   &DriverStation::ramUsageChanged,
             this, &DSEventLogger::onRAMUsageChanged);
    connect (ds,   &DriverStation::newMessage,
             this, &DSEventLogger::onNewMessage);
    connect (ds,   &DriverStation::diskUsageChanged,
             this, &DSEventLogger::onDiskUsageChanged);
    connect (ds,   &DriverStation::enabledChanged,
             this, &DSEventLogger::onEnabledChanged);
    connect (ds,   &DriverStation::teamNumberChanged,
             this, &DSEventLogger::onTeamNumberChanged);
    connect (ds,   &DriverStation::voltageChanged,
             this, &DSEventLogger::onVoltageChanged);
    connect (ds,   &DriverStation::robotCodeChanged,
             this, &DSEventLogger::onRobotCodeChanged);
    connect (ds,   &DriverStation::fmsCommunicationsChanged,
             this, &DSEventLogger::onFMSCommunicationsChanged);
    connect (ds,   &DriverStation::radioCommunicationsChanged,
             this, &DSEventLogger::onRadioCommunicationsChanged);
    connect (ds,   &DriverStation::robotCommunicationsChanged,
             this, &DSEventLogger::onRobotCommunicationsChanged);
    connect (ds,   &DriverStation::emergencyStoppedChanged,
             this, &DSEventLogger::onEmergencyStoppedChanged);
    connect (ds,   &DriverStation::controlModeChanged,
             this, &DSEventLogger::onControlModeChanged);
    connect (ds,   &DriverStation::allianceChanged,
             this, &DSEventLogger::onAllianceChanged);
    connect (ds,   &DriverStation::positionChanged,
             this, &DSEventLogger::onPositionChanged);
}
