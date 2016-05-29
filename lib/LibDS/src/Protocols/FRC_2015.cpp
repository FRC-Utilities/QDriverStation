/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#include "FRC_2015.h"

const uint OP_MODE_TEST         = 0x01;
const uint OP_MODE_AUTONOMOUS   = 0x02;
const uint OP_MODE_TELEOPERATED = 0x00;
const uint OP_MODE_ESTOPPED     = 0x80;
const uint OP_MODE_ENABLED      = 0x04;
const uint DS_FMS_ATTACHED      = 0x08;
const uint DS_REQUEST_NORMAL    = 0x80;
const uint DS_REQUEST_UNKNOWN   = 0x00;
const uint DS_REQUEST_REBOOT    = 0x08;
const uint DS_REQUEST_KILL_CODE = 0x04;
const uint R_CTRL_HAS_CODE      = 0x20;
const uint R_CTRL_BROWNOUT      = 0x10;
const uint DS_FMS_COMM_VERSION  = 0x00;
const uint DS_FMS_ROBOT_COMMS   = 0x20;
const uint DS_FMS_RADIO_PING    = 0x10;
const uint DS_FMS_ROBOT_PING    = 0x08;
const uint DS_TAG_DATE          = 0x0f;
const uint DS_TAG_GENERAL       = 0x01;
const uint DS_TAG_JOYSTICK      = 0x0c;
const uint DS_TAG_TIMEZONE      = 0x10;
const uint STATION_RED_1        = 0x00;
const uint STATION_RED_2        = 0x01;
const uint STATION_RED_3        = 0x02;
const uint STATION_BLUE_1       = 0x03;
const uint STATION_BLUE_2       = 0x04;
const uint STATION_BLUE_3       = 0x05;
const uint R_REQUEST_TIME       = 0x01;

/**
 * Implements the 2015 FRC Communication protocol
 */
FRC_2015::FRC_2015()
{
    m_restartCode = false;
    m_rebootRobot = false;
    m_sendDateTime = false;
}

/**
 * Returns the display name of the protocol
 */
QString FRC_2015::name()
{
    return QObject::tr ("FRC 2015 Protocol");
}

/**
 * Send 2 FMS packets every second
 */
int FRC_2015::fmsFrequency()
{
    return 2;
}

/**
 * Send 50 robot packets every second
 */
int FRC_2015::robotFrequency()
{
    return 50;
}

/**
 * We receive data from FMS at local port 1120
 */
int FRC_2015::fmsInputPort()
{
    return 1120;
}

/**
 * We send data to the FMS to remote port 1160
 */
int FRC_2015::fmsOutputPort()
{
    return 1160;
}

/**
 * We receive data from the robot at local port 1150
 */
int FRC_2015::robotInputPort()
{
    return 1150;
}

/**
 * We send data to the robot at remote port 1110
 */
int FRC_2015::robotOutputPort()
{
    return 1110;
}

/**
 * We received broadcasted messages from the robot in port 6666
 */
int FRC_2015::netconsoleInputPort()
{
    return 6666;
}

/**
 * Reboots the robot in the next packet cycle
 */
void FRC_2015::rebootRobot()
{
    m_rebootRobot = true;
}

/**
 * Restarts the robot code in the next packet cycle
 */
void FRC_2015::restartRobotCode()
{
    m_restartCode = true;
}

/**
 * Do not reboot robot or restart code when we re-establish communications
 * with the robot.
 */
void FRC_2015::onRobotWatchdogExpired()
{
    m_restartCode = false;
    m_rebootRobot = false;
    m_sendDateTime = false;
}

/**
 * FMS communications work with UDP datagrams
 */
DS::SocketType FRC_2015::fmsSocketType()
{
    return DS::kSocketTypeUDP;
}

/**
 * Robot communications work with UDP datagrams
 */
DS::SocketType FRC_2015::robotSocketType()
{
    return DS::kSocketTypeUDP;
}

/**
 * Default radio address is 10.TE.AM.1
 */
QString FRC_2015::defaultRadioAddress()
{
    return DS::getStaticIP (10, config()->team(), 1);
}

/**
 * Return a list with the following robot addresses:
 *     - roboRIO-TEAM.local
 *     - 172.22.11.2
 *     - 10.TE.AM.2
 */
QStringList FRC_2015::defaultRobotAddresses()
{
    QStringList list;
    list.append (QString ("roboRIO-%1.local").arg (config()->team()));
    list.append (QString ("172.22.11.2"));
    list.append (DS::getStaticIP (10, config()->team(), 2));
    return list;
}

/**
 * Generates a packet that the DS will send to the FMS
 */
QByteArray FRC_2015::getFMSPacket()
{
    QByteArray data;
    data.append ((sentFMSPackets() & 0xff00) >> 8);
    data.append ((sentFMSPackets()) & 0xff);
    data.append (DS_FMS_COMM_VERSION);
    data.append (getFMSControlCode());
    data.append ((config()->team() & 0xff00) >> 8);
    data.append ((config()->team()) & 0xff);
    data.append ((int) config()->voltage());
    data.append (config()->voltage() - (int) config()->voltage());
    return data;
}

/**
 * Generates a packet that the DS will send to the robot
 */
QByteArray FRC_2015::getRobotPacket()
{
    QByteArray data;
    data.append ((sentRobotPackets() & 0xff00) >> 8);
    data.append ((sentRobotPackets()) & 0xff);
    data.append (DS_TAG_GENERAL);
    data.append (getControlCode());
    data.append (getRequestCode());
    data.append (getTeamStationCode());
    data.append (m_sendDateTime ? getTimezoneData() : getJoystickData());
    return data;
}

/**
 * Interprets the packet and follows the instructions sent by the FMS.
 * Possible instructions are:
 *   - Change robot control mode
 *   - Change robot enabled status
 *   - Change team alliance
 *   - Change team position
 */
bool FRC_2015::interpretFMSPacket (const QByteArray& data)
{
    if (data.length() >= 22) {
        quint8 control = data.at (3);
        quint8 station = data.at (5);

        /* Change robot enabled state based on what FMS tells us to do*/
        config()->setEnabled (control & OP_MODE_ENABLED);

        /* Get FMS robot mode */
        DS::ControlMode mode;
        if (control & OP_MODE_TELEOPERATED)
            mode = DS::kControlTeleoperated;
        else if (control & OP_MODE_AUTONOMOUS)
            mode = DS::kControlAutonomous;
        else if (control & OP_MODE_TEST)
            mode = DS::kControlTest;

        /* Update robot mode */
        config()->updateControlMode (mode);

        /* Update to correct alliance and position */
        config()->updateAlliance (getAlliance (station));
        config()->updatePosition (getPosition (station));

        return true;
    }

    qWarning() << name() << "Received invalid FMS packet!";

    return false;
}

/**
 * Interprets the received data and updates DS values as needed.
 * From the received information, we can extract:
 *   - The robot voltage
 *   - Code state of the robot
 *   - Control mode of the robot
 *   - If the robot is emergency stopped
 *   - If the robot is experiencing a voltage brownout
 */
bool FRC_2015::interpretRobotPacket (const QByteArray& data)
{
    /* Packet is invalid */
    if (data.length() < 8) {
        qWarning() << name() << "received an invalid robot packet";
        return false;
    }

    /* Read robot packet */
    uint control = data.at (3);
    uint status  = data.at (4);
    uint request = data.at (7);
    uint integer = data.at (5);
    uint decimal = data.at (6);

    /* Generate control information */
    bool has_code       = (status & R_CTRL_HAS_CODE);
    bool e_stopped      = (control & OP_MODE_ESTOPPED);
    bool voltage_brwn   = (control & R_CTRL_BROWNOUT);

    /* Update client information */
    config()->setRobotCode (has_code);
    config()->setBrownout  (voltage_brwn);
    m_sendDateTime = (request == R_REQUEST_TIME);

    /* Update emergency stop state */
    if (e_stopped && !config()->isEmergencyStopped())
        config()->setEmergencyStop (true);
    else if (!e_stopped && config()->isEmergencyStopped())
        config()->setEmergencyStop (false);

    /* Calculate the voltage */
    float voltage = (integer + ((float) (decimal) * 99 / 255 / 100));
    config()->updateVoltage (voltage);

    return true;
}

/**
 * Returns information regarding the current date and time and the timezone
 * of the client computer.
 *
 * The robot may ask for this information in some cases (e.g. when initializing
 * the robot code).
 */
QByteArray FRC_2015::getTimezoneData()
{
    QByteArray data;

    /* Add size (always 11) */
    data.append (0x0B);

    /* Get current date/time */
    QDateTime dt = QDateTime::currentDateTime();
    QDate date = dt.date();
    QTime time = dt.time();

    /* Add current date/time */
    data.append (DS_TAG_DATE);
    data.append ((time.msec() & 0xff00) >> 8);
    data.append ((time.msec()) & 0xff);
    data.append (time.second());
    data.append (time.minute());
    data.append (time.hour());
    data.append (date.day());
    data.append (date.month());
    data.append (date.year() - 1900);

    /* Add timezone data */
    data.append (DS::timezone().length() + 1);
    data.append (DS_TAG_TIMEZONE);
    data.append (DS::timezone());

    return data;
}

/**
 * Constructs a joystick information structure for every attached joystick.
 * Unlike the 2014 protocol, the 2015 protocol only generates joystick data
 * for the attached joysticks.
 *
 * This information is separated with a joystick header code (specified at the
 * top of this file).
 */
QByteArray FRC_2015::getJoystickData()
{
    QByteArray data;

    /* Do not send joystick data on DS init */
    if (sentRobotPackets() <= 5)
        return data;

    /* Generate data for each joystick */
    for (int i = 0; i < joysticks()->count(); ++i) {
        int numAxes    = joysticks()->at (i)->numAxes;
        int numPOVs    = joysticks()->at (i)->numPOVs;
        int numButtons = joysticks()->at (i)->numButtons;

        /* Add joystick information and put the section header */
        data.append (getJoystickSize (*joysticks()->at (i)) - 1);
        data.append (DS_TAG_JOYSTICK);

        /* Add axis data */
        data.append (numAxes);
        for (int axis = 0; axis < numAxes; ++axis)
            data.append (joysticks()->at (i)->axes [axis] * 127);

        /* Generate button data */
        int buttonData = 0;
        for (int button = 0; button < numButtons; ++button)
            buttonData += joysticks()->at (i)->buttons [button] ?
                          qPow (2, button) : 0;

        /* Add button data */
        data.append (numButtons);
        data.append ((buttonData & 0xff00) >> 8);
        data.append ((buttonData & 0xff));

        /* Add hat/pov data */
        data.append (numPOVs);
        for (int hat = 0; hat < numPOVs; ++hat) {
            data.append ((joysticks()->at (i)->povs [hat] & 0xff00) >> 8);
            data.append ((joysticks()->at (i)->povs [hat] & 0xff));
        }
    }

    return data;
}

/**
 * This function returns the alliance color referenced by the given \a station
 * code. This function is used to follow the instructions outlined by the
 * FMS packets.
 */
DS::Alliance FRC_2015::getAlliance (quint8 station)
{
    if (station == STATION_BLUE_1
        || station == STATION_BLUE_2
        || station == STATION_BLUE_3)
        return DS::kAllianceBlue;

    return DS::kAllianceRed;
}

/**
 * This function returns the position referenced by the given \a station code.
 * This function is used to follow the instructions outlined by the FMS packets.
 */
DS::Position FRC_2015::getPosition (quint8 station)
{
    if (station == STATION_RED_1 || station == STATION_BLUE_1)
        return DS::kPosition1;

    if (station == STATION_RED_2 || station == STATION_BLUE_2)
        return DS::kPosition2;

    if (station == STATION_RED_3 || station == STATION_BLUE_3)
        return DS::kPosition3;

    return DS::kPosition1;
}

/**
 * Returns the control code sent to the robot.
 * This code contains the following information:
 *    - The control mode of the robot (teleop, autonomous, test)
 *    - The enabled state of the robot
 *    - The FMS attached keyword
 *    - The operation state (e-stop, normal)
 */
uint FRC_2015::getControlCode()
{
    uint code = 0;

    switch (config()->controlMode()) {
    case DS::kControlTest:
        code |= OP_MODE_TEST;
        break;
    case DS::kControlAutonomous:
        code |= OP_MODE_AUTONOMOUS;
        break;
    case DS::kControlTeleoperated:
        code |= OP_MODE_TELEOPERATED;
        break;
    default:
        break;
    }

    if (config()->isFMSAttached())
        code |= DS_FMS_ATTACHED;

    if (config()->isEmergencyStopped())
        code |= OP_MODE_ESTOPPED;

    if (config()->isEnabled())
        code |= OP_MODE_ENABLED;

    return code;
}

/**
 * Generates the request code sent to the robot, which may instruct it to:
 *    - Operate normally
 *    - Reboot the RIO
 *    - Restart the robot code
 */
uint FRC_2015::getRequestCode()
{
    uint code = DS_REQUEST_UNKNOWN;

    if (config()->isConnectedToRobot())
        code = DS_REQUEST_NORMAL;

    if (config()->isConnectedToRobot() && m_rebootRobot)
        code |= DS_REQUEST_REBOOT;

    if (config()->isConnectedToRobot() && m_restartCode)
        code |= DS_REQUEST_KILL_CODE;

    return code;
}

/**
 * Returns the control code sent to the FMS. This code is very similar to
 * the control code sent to the robot, however, it contains addional information
 * regarding the robot radio.
 *
 * This code contains the following information:
 *    - The control mode of the robot (teleop, autonomous, test)
 *    - The enabled state of the robot
 *    - The FMS attached keyword
 *    - Robot radio connected?
 *    - The operation state (e-stop, normal)
 */
uint FRC_2015::getFMSControlCode()
{
    uint code = 0;

    switch (config()->controlMode()) {
    case DS::kControlTest:
        code |= OP_MODE_TEST;
        break;
    case DS::kControlAutonomous:
        code |= OP_MODE_AUTONOMOUS;
        break;
    case DS::kControlTeleoperated:
        code |= OP_MODE_TELEOPERATED;
        break;
    default:
        break;
    }

    if (config()->isEmergencyStopped())
        code |= OP_MODE_ESTOPPED;

    if (config()->isEnabled())
        code |= OP_MODE_ENABLED;

    if (config()->isConnectedToRadio())
        code |= DS_FMS_RADIO_PING;

    if (config()->isConnectedToRobot()) {
        code |= DS_FMS_ROBOT_COMMS;
        code |= DS_FMS_ROBOT_PING;
    }

    return code;
}

/**
 * Returns the team station code sent to the robot.
 * This value may be used by the robot program to use specialized autonomous
 * modes or adjust sensor input.
 */
uint FRC_2015::getTeamStationCode()
{
    if (config()->position() == DS::kPosition1) {
        if (config()->alliance() == DS::kAllianceRed)
            return STATION_RED_1;
        else
            return STATION_BLUE_1;
    }

    if (config()->position() == DS::kPosition2) {
        if (config()->alliance() == DS::kAllianceRed)
            return STATION_RED_2;
        else
            return STATION_BLUE_2;
    }

    if (config()->position() == DS::kPosition3) {
        if (config()->alliance() == DS::kAllianceRed)
            return STATION_RED_3;
        else
            return STATION_BLUE_3;
    }

    return STATION_RED_1;
}

/**
 * Returns the size of the given \a joystick. This function is used to generate
 * joystick data (which is sent to the robot).
 *
 * This information will help the robot decide where a information starts and
 * ends for each attached joystick.
 */
uint FRC_2015::getJoystickSize(const DS::Joystick& joystick)
{
    return  5
            + (joystick.numAxes > 0 ? joystick.numAxes : 0)
            + (joystick.numButtons / 8)
            + (joystick.numButtons % 8 == 0 ? 0 : 1)
            + (joystick.numPOVs > 0 ? joystick.numPOVs * 2 : 0);
}
