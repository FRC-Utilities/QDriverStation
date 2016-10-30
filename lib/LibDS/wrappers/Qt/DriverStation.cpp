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

#include "DriverStation.h"

#include <math.h>
#include <LibDS.h>

#include <QTimer>
#include <QDebug>
#include <QHostAddress>
#include <QApplication>

static bool initialized = 0;

#define LOG qDebug() << "DS Client:"

/**
 * Converts the given \c QString into a \c SDS string
 *
 * \param string the \c QString to convert
 *
 * \returns the obtained \c SDS string
 */
static sds qstring_to_sds (const QString& string)
{
    return sdsnew (string.toStdString().c_str());
}

/**
 * Converts the given \c SDS string into a \c QString
 *
 * \param string the \c SDS string to convert
 *
 * \returns the obtained \c QString
 */
static QString sds_to_qstring (const sds string)
{
    if (string)
        return QString (string);

    return QString ("");
}

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
 * Returns the packet loss percentage between the FMS and the client
 */
int DriverStation::fmsPacketLoss() const
{
    return (qreal) (DS_ReceivedFMSPackets() / DS_SentFMSPackets()) * 100;
}

/**
 * Returns the packet loss percentage between the radio and the client
 */
int DriverStation::radioPacketLoss() const
{
    return (qreal) (DS_ReceivedRadioPackets() / DS_SentRadioPackets()) * 100;
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
    return (controlMode() == kControlTest);
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
    return (controlMode() == kControlAutonomous);
}

/**
 * Returns \c true if the robot is currently in teleop mode.
 * You can change the control mode with the \c setControlMode() function
 * or with the \c switchToAutonomous(), \c switchToTestMode() or
 * \c switchToTeleoperated() functions
 */
bool DriverStation::isTeleoperated() const
{
    return (controlMode() == kControlTeleoperated);
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
        return kControlTest;
        break;
    case DS_CONTROL_AUTONOMOUS:
        return kControlAutonomous;
        break;
    case DS_CONTROL_TELEOPERATED:
        return kControlTeleoperated;
        break;
    default:
        return kControlTeleoperated;
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
    if (teamAlliance() == kAllianceRed) {
        switch (teamPosition()) {
        case kPosition1:
            return kStationRed1;
            break;
        case kPosition2:
            return kStationRed2;
            break;
        case kPosition3:
            return kStationRed3;
            break;
        default:
            return kStationRed1;
            break;
        }
    }

    else {
        switch (teamPosition()) {
        case kPosition1:
            return kStationBlue1;
            break;
        case kPosition2:
            return kStationBlue2;
            break;
        case kPosition3:
            return kStationBlue3;
            break;
        default:
            return kStationBlue1;
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
        return kAllianceRed;
        break;
    case DS_ALLIANCE_BLUE:
        return kAllianceBlue;
        break;
    default:
        return kAllianceRed;
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
        return kPosition1;
        break;
    case DS_POSITION_2:
        return kPosition2;
        break;
    case DS_POSITION_3:
        return kPosition3;
        break;
    default:
        return kPosition1;
        break;
    }
}

/**
 * Returns the internet address that the Driver Station is using to communicate
 * with the Field Management System.
 */
QString DriverStation::appliedFMSAddress() const
{
    return sds_to_qstring (DS_GetAppliedFMSAddress());
}

/**
 * Returns the internet address that the Driver Station is using to communicate
 * with the robot radio.
 */
QString DriverStation::appliedRadioAddress() const
{
    return sds_to_qstring (DS_GetAppliedRadioAddress());
}

/**
 * Returns the internet address that the Driver Station is using to communicate
 * with the robot controller.
 */
QString DriverStation::appliedRobotAddress() const
{
    return sds_to_qstring (DS_GetAppliedRobotAddress());
}

/**
 * Returns the FMS address recommended by the loaded protocol
 */
QString DriverStation::defaultFMSAddress() const
{
    return sds_to_qstring (DS_GetDefaultFMSAddress());
}

/**
 * Returns the radio address recommended by the loaded protocol
 */
QString DriverStation::defaultRadioAddress() const
{
    return sds_to_qstring (DS_GetDefaultRadioAddress());
}

/**
 * Returns the robot address recommended by the loaded protocol
 */
QString DriverStation::defaultRobotAddress() const
{
    return sds_to_qstring (DS_GetDefaultRobotAddress());
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
    return sds_to_qstring (DS_GetStatusString());
}

/**
 * Returns the user-set FMS address, use the \c setCustomFMSAddress() function
 * to change this value.
 */
QString DriverStation::customFMSAddress() const
{
    return sds_to_qstring (DS_GetCustomFMSAddress());
}

/**
 * Returns the user-set radio address, use the \c setCustomRadioAddress()
 * function to change this value.
 */
QString DriverStation::customRadioAddress() const
{
    return sds_to_qstring (DS_GetCustomRadioAddress());
}

/**
 * Returns the user-set robot address, use the \c setCustomRobotAddress()
 * function to change this value.
 */
QString DriverStation::customRobotAddress() const
{
    return sds_to_qstring (DS_GetCustomRobotAddress());
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
 * Initializes the LibDS system and instructs the class to close the LibDS
 * before the Qt application is closed.
 */
void DriverStation::start()
{
    if (!initialized) {
        DS_Init();
        processEvents();
        emit statusChanged (sds_to_qstring (DS_GetStatusString()));
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
 * Disables the robot and switches its control mode to \c kControlTest
 */
void DriverStation::switchToTestMode()
{
    setEnabled (false);
    setControlMode (kControlTest);
}

/**
 * Disables the robot and switches its control mode to \c kControlAutonomous
 */
void DriverStation::switchToAutonomous()
{
    setEnabled (false);
    setControlMode (kControlAutonomous);
}

/**
 * Disables the robot and switches its control mode to \c kControlTeleoperated
 */
void DriverStation::switchToTeleoperated()
{
    setEnabled (false);
    setControlMode (kControlTeleoperated);
}

/**
 * Disables or enables the robot
 */
void DriverStation::setEnabled (const bool enabled)
{
    LOG << "Setting enabled state to" << enabled;
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
    emit teamNumberChanged (number);
}

/**
 * De-allocates the current protocol and loads the given \a protocol
 *
 * \param protocol the new communication protocol to use
 */
void DriverStation::setProtocol (DS_Protocol* protocol)
{
    if (protocol) {
        LOG << "Loading protocol" << protocol;

        start();
        DS_ConfigureProtocol (protocol);

        emit protocolChanged();
        emit statusChanged (sds_to_qstring (DS_GetStatusString()));

        LOG << "Protocol" << protocol << "loaded";
    }
}

/**
 * Changes the control \a mode of the robot
 */
void DriverStation::setControlMode (const Control mode)
{
    LOG << "Setting control mode to" << mode;

    switch (mode) {
    case kControlTest:
        DS_SetControlMode (DS_CONTROL_TEST);
        break;
    case kControlAutonomous:
        DS_SetControlMode (DS_CONTROL_AUTONOMOUS);
        break;
    case kControlTeleoperated:
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
void DriverStation::setProtocol (const int protocol)
{
    switch ((Protocol) protocol) {
    case kProtocol2014:
        setProtocol (DS_GetProtocolFRC_2014());
        LOG << "Switched to FRC 2014 Protocol";
        break;
    case kProtocol2015:
        setProtocol (DS_GetProtocolFRC_2015());
        LOG << "Switched to FRC 2015 Protocol";
        break;
    case kProtocol2016:
        setProtocol (DS_GetProtocolFRC_2016());
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
void DriverStation::setTeamStation (const int station)
{
    switch ((Station) station) {
    case kStationRed1:
        setTeamPosition (kPosition1);
        setTeamAlliance (kAllianceRed);
        break;
    case kStationRed2:
        setTeamPosition (kPosition2);
        setTeamAlliance (kAllianceRed);
        break;
    case kStationRed3:
        setTeamPosition (kPosition3);
        setTeamAlliance (kAllianceRed);
        break;
    case kStationBlue1:
        setTeamPosition (kPosition1);
        setTeamAlliance (kAllianceBlue);
        break;
    case kStationBlue2:
        setTeamPosition (kPosition2);
        setTeamAlliance (kAllianceBlue);
        break;
    case kStationBlue3:
        setTeamPosition (kPosition3);
        setTeamAlliance (kAllianceBlue);
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
void DriverStation::setTeamAlliance (const int alliance)
{
    LOG << "Setting alliance to" << alliance;

    switch ((Alliance) alliance) {
    case kAllianceRed:
        DS_SetAlliance (DS_ALLIANCE_RED);
        break;
    case kAllianceBlue:
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
void DriverStation::setTeamPosition (const int position)
{
    LOG << "Setting position to" << position;

    switch ((Position) position) {
    case kPosition1:
        DS_SetPosition (DS_POSITION_1);
        break;
    case kPosition2:
        DS_SetPosition (DS_POSITION_2);
        break;
    case kPosition3:
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
    if (addressIsValid (address) || address.isEmpty()) {
        LOG << "Using new FMS address" << address;
        DS_SetCustomFMSAddress (qstring_to_sds (address));
    }
}

/**
 * Forces the LibDS to use the given \a address to communicate with the radio
 */
void DriverStation::setCustomRadioAddress (const QString& address)
{
    if (addressIsValid (address) || address.isEmpty()) {
        LOG << "Using new radio address" << address;
        DS_SetCustomRadioAddress (qstring_to_sds (address));
    }
}

/**
 * Forces the LibDS to use the given \a address to communicate with the robot
 */
void DriverStation::setCustomRobotAddress (const QString& address)
{
    if (addressIsValid (address) || address.isEmpty()) {
        LOG << "Using new robot address" << address;
        DS_SetCustomRobotAddress (qstring_to_sds (address));
    }
}

/**
 * Broadcasts/sends the given \a message to the NetConsole network
 */
void DriverStation::sendNetConsoleMessage (const QString& message)
{
    if (!message.isEmpty())
        DS_SendNetConsoleMessage (qstring_to_sds (message));
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
    LOG << "Stopping DS Engine...";
    DS_Close();
    LOG << "DS Engine Stopped";
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
            emit fmsCommunicationsChanged (event.fms.connected);
            break;
        case DS_RADIO_COMMS_CHANGED:
            emit radioCommunicationsChanged (event.radio.connected);
            break;
        case DS_NETCONSOLE_NEW_MESSAGE:
            emit newMessage (sds_to_qstring (event.netconsole.message));
            break;
        case DS_ROBOT_ENABLED_CHANGED:
            emit enabledChanged (event.robot.enabled);
            break;
        case DS_ROBOT_MODE_CHANGED:
            emit controlModeChanged (controlMode());
            break;
        case DS_ROBOT_COMMS_CHANGED:
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
            emit allianceChanged (teamAlliance());
            emit positionChanged (teamPosition());
            break;
        case DS_ROBOT_ESTOP_CHANGED:
            emit emergencyStoppedChanged (event.robot.estopped);
            break;
        case DS_STATUS_STRING_CHANGED:
            emit statusChanged (sds_to_qstring (DS_GetStatusString()));
            break;
        default:
            break;
        }
    }

    QTimer::singleShot (5, Qt::CoarseTimer, this, SLOT (processEvents()));
}

/**
 * Returns \c true if the given \a address is a valid IP or DNS address
 *
 * \param address the network address to check for validity
 */
bool DriverStation::addressIsValid (const QString& address)
{
    /* This is a valid mDNS address */
    if (address.endsWith (".local"))
        return true;

    /* Check if address is a valid IPv4 address*/
    QHostAddress host (address);
    if (!host.isNull()) {
        if (address.split (".").count() == 4)
            return true;
    }

    /* This is an invalid IPv4 address */
    return false;
}
