/*
 * Copyright (c) 2015-2017 Alex Spataru <alex_spataru@outlook>
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

#include "DriverStation.h"

#include <math.h>
#include <LibDS.h>

#include <QTime>
#include <QTimer>
#include <QDebug>
#include <QHostAddress>
#include <QApplication>

#define LOG qDebug() << "DS Client:"

/**
 * Thar shall be only one tavern that manages
 * th' Driver Station interface
 *
 * \returns the only instance of this class
 */
DriverStation* DriverStation::getInstance()
{
    static DriverStation instance;
    return &instance;
}

/**
 * Returns the current CPU usage of the robot
 */
int DriverStation::cpuUsage() const
{
    return DS_GetRobotCPUUsage();
}

/**
 * Returns the current CAN utilization of the robot
 */
int DriverStation::canUsage() const
{
    return DS_GetRobotCANUtilization();
}

/**
 * Returns the current RAM usage of the robot
 */
int DriverStation::ramUsage() const
{
    return DS_GetRobotRAMUsage();
}

/**
 * Returns the current disk usage of the robot
 */
int DriverStation::diskUsage() const
{
    return DS_GetRobotDiskUsage();
}

/**
 * Returns the packet loss percentage between the FMS and the client
 */
int DriverStation::fmsPacketLoss() const
{
    qreal sent = (qreal) DS_SentFMSPackets();
    qreal recv = (qreal) DS_ReceivedFMSPackets();

    if (sent > 0)
        return (1 - (recv / sent)) * 100;

    return 100;
}

/**
 * Returns the packet loss percentage between the radio and the client
 */
int DriverStation::radioPacketLoss() const
{
    qreal sent = (qreal) DS_SentRadioPackets();
    qreal recv = (qreal) DS_ReceivedRadioPackets();

    if (sent > 0)
        return (1 - (recv / sent)) * 100;

    return 100;
}

/**
 * Returns the packet loss percentage between the robot and the client
 */
int DriverStation::robotPacketLoss() const
{
    qreal sent = (qreal) DS_SentRobotPackets();
    qreal recv = (qreal) DS_ReceivedRobotPackets();

    if (sent > 0)
        return (1 - (recv / sent)) * 100;

    return 100;
}

/**
 * Returns the date when the LibDS binary was build
 */
QString DriverStation::buildDate()
{
    return QString (DS_GetBuildDate());
}

/**
 * Returns the time when the LibDS binary was build
 */
QString DriverStation::buildTime()
{
    return QString (DS_GetBuildTime());
}

/**
 * Returns the version of LibDS as a string
 */
QString DriverStation::libDSVersion()
{
    return QString (DS_GetVersion());
}

/**
 * Returns the team number used by the LibDS,
 * use the \c setTeamNumber() function to change it
 */
int DriverStation::teamNumber() const
{
    return DS_GetTeamNumber();
}

/**
 * Returns the number of joysticks registered with the Driver Station
 */
int DriverStation::joystickCount() const
{
    return DS_GetJoystickCount();
}

/**
 * Returns the current enabled state of the robot,
 * you can change it with the \c setEnabled() function
 */
bool DriverStation::isEnabled() const
{
    return DS_GetRobotEnabled();
}

/**
 * Returns \c true if the robot is currently in test mode.
 * You can change the control mode with the \c setControlMode() function
 * or with the \c switchToAutonomous(), \c switchToTestMode() or
 * \c switchToTeleoperated() functions
 */
bool DriverStation::isTestMode() const
{
    return (controlMode() == ControlTest);
}

/**
 * Returns \c true if it is possible to enable the robot.
 * This function checks if:
 *   - We have communications with the robot
 *   - The robot code is loaded
 *   - The robot is not emergency stopped
 */
bool DriverStation::canBeEnabled() const
{
    return DS_GetCanBeEnabled();
}

/**
 * Returns \c true if the robot code is present
 */
bool DriverStation::hasRobotCode() const
{
    return DS_GetRobotCode();
}

/**
 * Returns \c true if the robot is currently in autonomous mode.
 * You can change the control mode with the \c setControlMode() function
 * or with the \c switchToAutonomous(), \c switchToTestMode() or
 * \c switchToTeleoperated() functions
 */
bool DriverStation::isAutonomous() const
{
    return (controlMode() == ControlAutonomous);
}

/**
 * Returns \c true if the robot is currently in teleop mode.
 * You can change the control mode with the \c setControlMode() function
 * or with the \c switchToAutonomous(), \c switchToTestMode() or
 * \c switchToTeleoperated() functions
 */
bool DriverStation::isTeleoperated() const
{
    return (controlMode() == ControlTeleoperated);
}

/**
 * Returns \c true if the Driver Station has communications with the FMS
 */
bool DriverStation::connectedToFMS() const
{
    return DS_GetFMSCommunications();
}

/**
 * Returns \c true if the Driver Station has communications with the robot radio
 */
bool DriverStation::connectedToRadio() const
{
    return DS_GetRadioCommunications();
}

/**
 * Returns \c true if the Driver Station has communications with the robot
 */
bool DriverStation::connectedToRobot() const
{
    return DS_GetRobotCommunications();
}

/**
 * Returns \c true if the Driver Station detects that the robot is emergency
 * stopped.
 *
 * You can trigger an emergency stop with the \c setEmergencyStopped() function
 */
bool DriverStation::emergencyStopped() const
{
    return DS_GetEmergencyStopped();
}

/**
 * Returns the current voltage reported by the robot and the loaded protocol
 */
qreal DriverStation::voltage() const
{
    return (qreal) DS_GetRobotVoltage();
}

/**
 * Returns the current voltage as a string, this function ensures that
 * the voltage will always display two decimals, even if the number is
 * an integer (e.g. this function will return "12.00 V")
 */
QString DriverStation::voltageString() const
{
    /* Round voltage to two decimal places */
    qreal volt = roundf (voltage() * 100) / 100;

    /* Avoid this: http://i.imgur.com/iAAi1bX.png */
    if (volt > maximumBatteryVoltage())
        volt = maximumBatteryVoltage();

    /* Separate voltage into natural and decimal numbers */
    int integer = static_cast<int> (volt);
    int decimal = static_cast<qreal> (volt - integer) * 100;

    /* Convert the obtained numbers into strings */
    QString integer_str = QString::number (integer);
    QString decimal_str = QString::number (decimal);

    /* Prepend a 0 to the decimal numbers if required */
    if (decimal < 10)
        decimal_str.prepend ("0");

    /* Construct final string */
    return integer_str + "." + decimal_str + " V";
}

/**
 * Returns the maximum battery voltage specified by the current protocol.
 * This value can be used to genereate graphs or enchance the user interface
 */
qreal DriverStation::maximumBatteryVoltage() const
{
    return (qreal) DS_GetMaximumBatteryVoltage();
}

/**
 * Returns the current control mode set by the Driver Station.
 * Possible return values are:
 *    - \c kControlTest
 *    - \c kControlAutonomous
 *    - \c kControlTeleoperated
 */
DriverStation::Control DriverStation::controlMode() const
{
    switch (DS_GetControlMode()) {
    case DS_CONTROL_TEST:
        return ControlTest;
        break;
    case DS_CONTROL_AUTONOMOUS:
        return ControlAutonomous;
        break;
    case DS_CONTROL_TELEOPERATED:
        return ControlTeleoperated;
        break;
    default:
        return ControlTeleoperated;
        break;
    }
}

/**
 * Returns the current team station of the DS/robot.
 *
 * Possible return values are:
 *   - \c kStationRed1
 *   - \c kStationRed2
 *   - \c kStationRed3
 *   - \c kStationBlue1
 *   - \c kStationBlue2
 *   - \c kStationBlue3
 *
 * You can also use the \c teamAlliance() and \c teamPosition() functions to
 * obtain the individual alliance and position of the DS/robot.
 */
DriverStation::Station DriverStation::teamStation() const
{
    if (teamAlliance() == AllianceRed) {
        switch (teamPosition()) {
        case Position1:
            return StationRed1;
            break;
        case Position2:
            return StationRed2;
            break;
        case Position3:
            return StationRed3;
            break;
        default:
            return StationRed1;
            break;
        }
    }

    else {
        switch (teamPosition()) {
        case Position1:
            return StationBlue1;
            break;
        case Position2:
            return StationBlue2;
            break;
        case Position3:
            return StationBlue3;
            break;
        default:
            return StationBlue1;
            break;
        }
    }
}

/**
 * Returns the current team alliance of the DS/robot.
 *
 * Possible return values are:
 *   - \c kAllianceRed
 *   - \c kAllianceBlue
 *
 * Related functions: \c teamPosition() and \c teamStation()
 */
DriverStation::Alliance DriverStation::teamAlliance() const
{
    switch (DS_GetAlliance()) {
    case DS_ALLIANCE_RED:
        return AllianceRed;
        break;
    case DS_ALLIANCE_BLUE:
        return AllianceBlue;
        break;
    default:
        return AllianceRed;
        break;
    }
}

/**
 * Returns the current position of the DS/robot.
 *
 * Possible return values are:
 *   - \c kPosition1
 *   - \c kPosition2
 *   - \c kPosition3
 *
 * Related functions: \c teamAlliance() and \c teamStation()
 */
DriverStation::Position DriverStation::teamPosition() const
{
    switch (DS_GetPosition()) {
    case DS_POSITION_1:
        return Position1;
        break;
    case DS_POSITION_2:
        return Position2;
        break;
    case DS_POSITION_3:
        return Position3;
        break;
    default:
        return Position1;
        break;
    }
}

/**
 * Returns the internet address that the Driver Station is using to communicate
 * with the Field Management System.
 */
QString DriverStation::appliedFMSAddress() const
{
    return QString::fromUtf8 (DS_GetAppliedFMSAddress());
}

/**
 * Returns the internet address that the Driver Station is using to communicate
 * with the robot radio.
 */
QString DriverStation::appliedRadioAddress() const
{
    return QString::fromUtf8 (DS_GetAppliedRadioAddress());
}

/**
 * Returns the internet address that the Driver Station is using to communicate
 * with the robot controller.
 */
QString DriverStation::appliedRobotAddress() const
{
    return QString::fromUtf8 (DS_GetAppliedRobotAddress());
}

/**
 * Returns the FMS address recommended by the loaded protocol
 */
QString DriverStation::defaultFMSAddress() const
{
    return QString::fromUtf8 (DS_GetDefaultFMSAddress());
}

/**
 * Returns the radio address recommended by the loaded protocol
 */
QString DriverStation::defaultRadioAddress() const
{
    return QString::fromUtf8 (DS_GetDefaultRadioAddress());
}

/**
 * Returns the robot address recommended by the loaded protocol
 */
QString DriverStation::defaultRobotAddress() const
{
    return QString::fromUtf8 (DS_GetDefaultRobotAddress());
}

/**
 * Returns the elapsed time since the robot has been enabled
 */
QString DriverStation::elapsedTime()
{
    if (m_elapsedTime.isEmpty())
        m_elapsedTime = "00:00.0";

    return m_elapsedTime;
}

/**
 * Returns the current status of the robot/DS.
 * This string is meant to be used directly by the clien application,
 * the DS has no real use for it.
 *
 * Possible return values are:
 *    - No Robot Communications
 *    - No Robot Code
 *    - Emergency Stopped
 *    - Teleoperated Enabled/Disabled
 *    - Autonomous Enabled/Disabled
 *    - Test Enabled/Disabled
 */
QString DriverStation::generalStatus() const
{
    return QString::fromUtf8 (DS_GetStatusString());
}

/**
 * Returns the user-set FMS address, use the \c setCustomFMSAddress() function
 * to change this value.
 */
QString DriverStation::customFMSAddress() const
{
    return QString::fromUtf8 (DS_GetCustomFMSAddress());
}

/**
 * Returns the user-set radio address, use the \c setCustomRadioAddress()
 * function to change this value.
 */
QString DriverStation::customRadioAddress() const
{
    return QString::fromUtf8 (DS_GetCustomRadioAddress());
}

/**
 * Returns the user-set robot address, use the \c setCustomRobotAddress()
 * function to change this value.
 */
QString DriverStation::customRobotAddress() const
{
    return QString::fromUtf8 (DS_GetCustomRobotAddress());
}

/**
 * Returns an ordered list with the available team stations.
 * You can use the value indexes of this list directly with the
 * \c setTeamStation() function
 */
QStringList DriverStation::stations() const
{
    QStringList list;

    list.append (tr ("Red 1"));
    list.append (tr ("Red 2"));
    list.append (tr ("Red 3"));
    list.append (tr ("Blue 1"));
    list.append (tr ("Blue 2"));
    list.append (tr ("Blue 3"));

    return list;
}

/**
 * Returns an ordered list with the available communication protocols.
 * You can use the value indexes of this list directly with the \c setProtocol()
 * function.
 */
QStringList DriverStation::protocols() const
{
    QStringList list;

    list.append (tr ("FRC 2016"));
    list.append (tr ("FRC 2015"));
    list.append (tr ("FRC 2014"));

    return list;
}

/**
 * Returns the number of sent FMS bytes since the current
 * protocol was loaded
 */
unsigned long DriverStation::sentFMSBytes() const
{
    return DS_SentFMSBytes();
}

/**
 * Returns the number of sent radio bytes since the current
 * protocol was loaded
 */
unsigned long DriverStation::sentRadioBytes() const
{
    return DS_SentRadioBytes();
}

/**
 * Returns the number of sent robot bytes since the current
 * protocol was loaded
 */
unsigned long DriverStation::sentRobotBytes() const
{
    return DS_SentRobotBytes();
}

/**
 * Returns the number of received FMS bytes since the current
 * protocol was loaded
 */
unsigned long DriverStation::receivedFMSBytes() const
{
    return DS_ReceivedFMSBytes();
}

/**
 * Returns the number of received radio bytes since the current
 * protocol was loaded
 */
unsigned long DriverStation::receivedRadioBytes() const
{
    return DS_ReceivedRadioBytes();
}

/**
 * Returns the number of received robot bytes since the current
 * protocol was loaded
 */
unsigned long DriverStation::receivedRobotBytes() const
{
    return DS_ReceivedRobotBytes();
}

/**
 * Returns the number of axes that the given \a joystick has.
 * If the joystick does not exist, this function will return \c 0
 */
int DriverStation::getNumAxes (const int joystick) const
{
    return DS_GetJoystickNumAxes (joystick);
}

/**
 * Returns the number of hats that the given \a joystick has.
 * If the joystick does not exist, this function will return \c 0
 */
int DriverStation::getNumHats (const int joystick) const
{
    return DS_GetJoystickNumHats (joystick);
}

/**
 * Returns the number of buttons that the given \a joystick has.
 * If the joystick does not exist, this function will return \c 0
 */
int DriverStation::getNumButtons (const int joystick) const
{
    return DS_GetJoystickNumButtons (joystick);
}

/**
 * Initializes the LibDS system and instructs the class to close the LibDS
 * before the Qt application is closed.
 */
void DriverStation::start()
{
    if (!DS_Initialized()) {
        DS_Init();
        processEvents();
        updateElapsedTime();
        emit statusChanged (generalStatus());
        connect (qApp, SIGNAL (aboutToQuit()), this, SLOT (quitDS()));
    }
}

/**
 * Reboots the robot controller
 */
void DriverStation::rebootRobot()
{
    LOG << "Requesting robot reboot...";
    DS_RebootRobot();
}

/**
 * Removes all the registered joysticks from the Driver Station
 */
void DriverStation::resetJoysticks()
{
    LOG << "Resetting joysticks...";
    DS_JoysticksReset();
    emit joystickCountChanged();
}

/**
 * Restarts the robot code process in the robot controller
 */
void DriverStation::restartRobotCode()
{
    LOG << "Requesting robot code restart...";
    DS_RestartRobotCode();
}

/**
 * Disables or enables the robot
 */
void DriverStation::setEnabled (const bool enabled)
{
    LOG << "Setting enabled state to" << enabled;

    if (enabled)
        resetElapsedTime();

    DS_SetRobotEnabled (enabled);
}

/**
 * Changes the team \a number
 * \note Changing the team number will trigger a refresh in the internet
 *       addresses used to communicate with the FMS, radio and robot.
 */
void DriverStation::setTeamNumber (const int number)
{
    LOG << "Changing team number to" << number;
    DS_SetTeamNumber (number);

    emit fmsAddressChanged();
    emit radioAddressChanged();
    emit robotAddressChanged();
    emit teamNumberChanged (number);
}

/**
 * De-allocates the current protocol and loads the given \a protocol
 *
 * \param protocol the new communication protocol to use
 */
void DriverStation::loadProtocol (const DS_Protocol& protocol)
{
    DS_ConfigureProtocol (&protocol);

    setCustomFMSAddress (customFMSAddress());
    setCustomRadioAddress (customRadioAddress());
    setCustomRobotAddress (customRobotAddress());

    emit protocolChanged();
    emit statusChanged (QString::fromUtf8 (DS_GetStatusString()));
}

/**
 * Changes the control \a mode of the robot
 */
void DriverStation::setControlMode (const Control mode)
{
    LOG << "Setting control mode to" << mode;

    if (isEnabled() && mode != controlMode())
        setEnabled (false);
    if (mode != controlMode())
        resetElapsedTime();

    switch (mode) {
    case ControlTest:
        DS_SetControlMode (DS_CONTROL_TEST);
        break;
    case ControlAutonomous:
        DS_SetControlMode (DS_CONTROL_AUTONOMOUS);
        break;
    case ControlTeleoperated:
        DS_SetControlMode (DS_CONTROL_TELEOPERATED);
        break;
    default:
        DS_SetControlMode (DS_CONTROL_TELEOPERATED);
        break;
    }
}

/**
 * Loads the given \a protocol type.
 *
 * \note You can use this function directly with the index values given by the
 *       \c protocols() function
 */
void DriverStation::setProtocol (const Protocol protocol)
{
    switch ((Protocol) protocol) {
    case Protocol2014:
        loadProtocol (DS_GetProtocolFRC_2014());
        LOG << "Switched to FRC 2014 Protocol";
        break;
    case Protocol2015:
        loadProtocol (DS_GetProtocolFRC_2015());
        LOG << "Switched to FRC 2015 Protocol";
        break;
    case Protocol2016:
        loadProtocol (DS_GetProtocolFRC_2016());
        LOG << "Switched to FRC 2016 Protocol";
        break;
    default:
        break;
    }
}

/**
 * Changes the team alliance and position based on the given \a station
 *
 * Possible input values are:
 *   - \c kStationRed1 (0)
 *   - \c kStationRed2 (1)
 *   - \c kStationRed3 (2)
 *   - \c kStationBlue1 (3)
 *   - \c kStationBlue2 (4)
 *   - \c kStationBlue3 (5)
 *
 * \note You can use this function directly with the output given by the \c
 *       \c stations() function
 */
void DriverStation::setTeamStation (const Station station)
{
    switch (station) {
    case StationRed1:
        setTeamPosition (Position1);
        setTeamAlliance (AllianceRed);
        break;
    case StationRed2:
        setTeamPosition (Position2);
        setTeamAlliance (AllianceRed);
        break;
    case StationRed3:
        setTeamPosition (Position3);
        setTeamAlliance (AllianceRed);
        break;
    case StationBlue1:
        setTeamPosition (Position1);
        setTeamAlliance (AllianceBlue);
        break;
    case StationBlue2:
        setTeamPosition (Position2);
        setTeamAlliance (AllianceBlue);
        break;
    case StationBlue3:
        setTeamPosition (Position3);
        setTeamAlliance (AllianceBlue);
        break;
    default:
        break;
    }
}

/**
 * Updates the team alliance, possible input values are:
 *    - \c kAllianceRed (0)
 *    - \c kAllianceBlue (1)
 */
void DriverStation::setTeamAlliance (const Alliance alliance)
{
    LOG << "Setting alliance to" << alliance;

    switch ((Alliance) alliance) {
    case AllianceRed:
        DS_SetAlliance (DS_ALLIANCE_RED);
        break;
    case AllianceBlue:
        DS_SetAlliance (DS_ALLIANCE_BLUE);
        break;
    }
}

/**
 * Updates the team position, possible input values are:
 *    - \c kPosition1 (0)
 *    - \c kPosition2 (1)
 *    - \c kPosition3 (2)
 */
void DriverStation::setTeamPosition (const Position position)
{
    LOG << "Setting position to" << position;

    switch ((Position) position) {
    case Position1:
        DS_SetPosition (DS_POSITION_1);
        break;
    case Position2:
        DS_SetPosition (DS_POSITION_2);
        break;
    case Position3:
        DS_SetPosition (DS_POSITION_3);
        break;
    }
}

/**
 * Changes the emergency stopped state of the robot
 */
void DriverStation::setEmergencyStopped (const bool stopped)
{
    LOG << "Setting ESTOP to" << stopped;
    DS_SetEmergencyStopped (stopped);
}

/**
 * Forces the LibDS to use the given \a address to communicate with the FMS
 */
void DriverStation::setCustomFMSAddress (const QString& address)
{
    LOG << "Using new FMS address" << getAddress (address);
    DS_SetCustomFMSAddress (getAddress (address).toStdString().c_str());

    emit fmsAddressChanged();
}

/**
 * Forces the LibDS to use the given \a address to communicate with the radio
 */
void DriverStation::setCustomRadioAddress (const QString& address)
{
    LOG << "Using new radio address" << getAddress (address);
    DS_SetCustomRadioAddress (getAddress (address).toStdString().c_str());

    emit radioAddressChanged();
}

/**
 * Forces the LibDS to use the given \a address to communicate with the robot
 */
void DriverStation::setCustomRobotAddress (const QString& address)
{
    LOG << "Using new robot address" << getAddress (address);
    DS_SetCustomRobotAddress (getAddress (address).toStdString().c_str());
    emit robotAddressChanged();
}

/**
 * Broadcasts/sends the given \a message to the NetConsole network
 */
void DriverStation::sendNetConsoleMessage (const QString& message)
{
    if (!message.isEmpty())
        DS_SendNetConsoleMessage (message.toStdString().c_str());
}

/**
 * Registers a new joystick with the Driver Station
 *
 * \param axes the number of axes of the new joystick
 * \param hats the number of hats/povs of the new joystick
 * \param buttons the number of buttons of the new joystick
 */
void DriverStation::addJoystick (int axes, int hats, int buttons)
{
    DS_JoysticksAdd (axes, hats, buttons);

    LOG << "Registered new joystick with"
        << axes << "axes,"
        << hats << "hats and"
        << buttons << "buttons";

    emit joystickCountChanged();
}

/**
 * Updates the \a angle of the given \a hat of the given \a joystick
 *
 * \param joystick the ID of the joystick to update (e.g. 0 for Joystick 0)
 * \param hat the ID of the hat to update (e.g. 0 for primary hat)
 * \param angle the new angle to apply to the hat
 */
void DriverStation::setJoystickHat (int joystick, int hat, int angle)
{
    DS_SetJoystickHat (joystick, hat, angle);
}

/**
 * Updates the \a value of the given \a axis of the given \a joystick
 *
 * \param joystick the ID of the joystick to update (e.g. 0 for Joystick 0)
 * \param axis the ID of the axis to update (e.g. 0 for primary X axis)
 * \param value the new value to apply to the given axis (e.g. 0.5)
 *
 * \note the \a value must have a range from \c -1 to \c 1
 */
void DriverStation::setJoystickAxis (int joystick, int axis, float value)
{
    DS_SetJoystickAxis (joystick, axis, value);
}

/**
 * Updates the \a pressed state of the given \a button of the given \a joystick
 *
 * \param joystick the ID of the joystick to update (e.g. 0 for Joystick 0)
 * \param button the ID of the button to update (e.g. 0 for A in an Xbox stick)
 * \param pressed the new pressed state of the button
 */
void DriverStation::setJoystickButton (int joystick, int button, bool pressed)
{
    DS_SetJoystickButton (joystick, button, pressed);
}

/**
 * Breaks the internal loops of the LibDS, de-allocates its assets and
 * closes all the network sockets used by the library
 */
void DriverStation::quitDS()
{
    if (DS_Initialized()) {
        LOG << "Stopping DS Engine...";
        DS_Close();
        LOG << "DS Engine Stopped";
    }
}

/**
 * Polls for new LibDS events and emits Qt signals as appropiate.
 * This function is called every 5 milliseconds.
 */
void DriverStation::processEvents()
{
    DS_Event event;
    while (DS_PollEvent (&event)) {
        switch (event.type) {
        case DS_FMS_COMMS_CHANGED:
            emit fmsAddressChanged();
            emit fmsCommunicationsChanged (event.fms.connected);
            break;
        case DS_RADIO_COMMS_CHANGED:
            emit radioAddressChanged();
            emit radioCommunicationsChanged (event.radio.connected);
            break;
        case DS_NETCONSOLE_NEW_MESSAGE:
            emit newMessage (QString::fromUtf8 (event.netconsole.message));
            break;
        case DS_ROBOT_ENABLED_CHANGED:
            emit enabledChanged (event.robot.enabled);
            break;
        case DS_ROBOT_MODE_CHANGED:
            emit controlModeChanged (controlMode());
            break;
        case DS_ROBOT_COMMS_CHANGED:
            emit robotAddressChanged();
            emit robotCommunicationsChanged (event.robot.connected);
            break;
        case DS_ROBOT_CODE_CHANGED:
            emit robotCodeChanged (event.robot.code);
            break;
        case DS_ROBOT_VOLTAGE_CHANGED:
            emit voltageChanged (event.robot.voltage);
            break;
        case DS_ROBOT_CAN_UTIL_CHANGED:
            emit canUsageChanged (event.robot.can_util);
            break;
        case DS_ROBOT_CPU_INFO_CHANGED:
            emit cpuUsageChanged (event.robot.cpu_usage);
            break;
        case DS_ROBOT_RAM_INFO_CHANGED:
            emit ramUsageChanged (event.robot.ram_usage);
            break;
        case DS_ROBOT_DISK_INFO_CHANGED:
            emit diskUsageChanged (event.robot.disk_usage);
            break;
        case DS_ROBOT_STATION_CHANGED:
            emit stationChanged();
            emit allianceChanged (teamAlliance());
            emit positionChanged (teamPosition());
            break;
        case DS_ROBOT_ESTOP_CHANGED:
            emit emergencyStoppedChanged (event.robot.estopped);
            break;
        case DS_STATUS_STRING_CHANGED:
            emit statusChanged (QString::fromUtf8 (DS_GetStatusString()));
            break;
        default:
            break;
        }
    }

    QTimer::singleShot (5, Qt::CoarseTimer, this, SLOT (processEvents()));
}

/**
 * Restarts the elapsed time counter
 */
void DriverStation::resetElapsedTime()
{
    m_time.restart();
    m_elapsedTime.clear();
    emit elapsedTimeChanged (elapsedTime());
}

/**
 * Updates the elapsed time and formats it as a string only if the robot
 * is currently enabled
 */
void DriverStation::updateElapsedTime()
{
    if (isEnabled()) {
        int milliseconds = m_time.elapsed();
        int seconds = (milliseconds / 1000);
        int minutes = (seconds / 60) % 60;

        seconds = seconds % 60;
        milliseconds = milliseconds % 1000;

        m_elapsedTime = QString ("%1:%2.%3")
                        .arg (minutes, 2, 10, QLatin1Char ('0'))
                        .arg (seconds, 2, 10, QLatin1Char ('0'))
                        .arg (QString::number (milliseconds).at (0));

        emit elapsedTimeChanged (elapsedTime());
    }

    QTimer::singleShot (100, Qt::PreciseTimer,
                        this, SLOT (updateElapsedTime()));
}

/**
 * Returns a valid network \a address
 */
QString DriverStation::getAddress (const QString& address)
{
    if (!address.isEmpty()) {
        if (address.endsWith (".local", Qt::CaseInsensitive) ||
            address.endsWith (".lan",   Qt::CaseInsensitive) ||
            address.endsWith (".com",   Qt::CaseInsensitive) ||
            address.endsWith (".net",   Qt::CaseInsensitive))
            return address;

        QHostAddress ip (address);
        if (!ip.isNull())
            return ip.toString();
    }

    return "";
}
