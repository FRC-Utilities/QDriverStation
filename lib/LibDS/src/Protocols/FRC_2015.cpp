/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#include "FRC_2015.h"

/**
 * Holds the control mode flags sent to the robot
 */
enum Robot_Control {
    kTest          = 0x01, /**< Test operation mode */
    kEnabled       = 0x04, /**< Sent when DS enables the robot */
    kAutonomous    = 0x02, /**< Autonomous operation mode */
    kTeleoperated  = 0x00, /**< Teleoperated operation mode */
    kFMS_Attached  = 0x08, /**< Sent to robot when we are connected with FMS */
    kEmergencyStop = 0x80, /**< Robot code is stopped */
};

/**
 * Holds the different operation states sent to the robot
 */
enum DS_Flags {
    kRequestReboot      = 0x08, /**< Sent when DS wants a robot reboot */
    kRequestNormal      = 0x80, /**< Sent when DS has comms. with robot */
    kRequestUnconnected = 0x00, /**< Sent when DS has no comms. with robot */
    kRequestRestartCode = 0x04, /**< Sent when DS wants to restart robot code */
};

/**
 * Holds different flags that are sent to the FMS
 */
enum FMS_Control {
    kFMS_RadioPing  = 0x10, /**< Sent if TCP ping with radio is successfull */
    kFMS_RobotPing  = 0x08, /**< Sent if TCP ping with robot is successfull */
    kFMS_RobotComms = 0x20, /**< Sent if DS has comms. with robot */
    kFMS_DS_Version = 0x00, /**< Sends DS version to the FMS */
};

/**
 * Holds the different tags/headers of each section of the sent packets
 */
enum DS_Tags {
    kTagDate     = 0x0f, /**< Staart of date & time section */
    kTagGeneral  = 0x01, /**< Start of control/basic section */
    kTagJoystick = 0x0c, /**< Start of joystick input section */
    kTagTimezone = 0x10, /**< Start of timezone section */
};

/**
 * Represents the different team stations
 */
enum Stations {
    kRed1  = 0x00, /**< Red Alliance, Position 1 */
    kRed2  = 0x01, /**< Red Alliance, Position 2 */
    kRed3  = 0x02, /**< Red Alliance, Position 3 */
    kBlue1 = 0x03, /**< Blue Alliance, Position 1 */
    kBlue2 = 0x04, /**< Blue Alliance, Position 2 */
    kBlue3 = 0x05, /**< Blue Alliance, Position 3 */
};

/**
 * Represents the tags that can be sent by the robot with the extended packets
 */
enum Robot_Tags {
    kRTagCpuInfo     = 0x05, /**< Robot program sents CPU usage */
    kRTagMemInfo     = 0x06, /**< Robot program sends RAM usage */
    kRTagDiskInfo    = 0x04, /**< Robot program sends disk usage */
    kRTagJoystickOut = 0x01, /**< Robot program wants to rumble joysticks */
};

/**
 * Different robot requests & operation status flags
 */
enum Robot_Data {
    kRequestTime     = 0x01, /**< Robot wants current date & time */
    kRobotHasCode    = 0x20, /**< Robot has user code loaded */
    kVoltageBrownout = 0x10, /**< Robot experiences a voltage brownout */
};

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
 * FRC 2015 protocol supports only 1 POV.
 * Remaining POVs will be ignored.
 */
int FRC_2015::maxPOVCount()
{
    return 1;
}

/**
 * FRC 2015 protocol supports a maximum of 6 axes.
 * Remaining axes will be ignored.
 */
int FRC_2015::maxAxisCount()
{
    return 6;
}

/**
 * FRC 2015 protocol supports a maximum of 12 buttons.
 * Remaining buttons will be ignored.
 */
int FRC_2015::maxButtonCount()
{
    return 12;
}

/**
 * FRC 2015 Protocol supports a maximum of 6 joysticks.
 * Remaining joysticks will be ignored.
 */
int FRC_2015::maxJoystickCount()
{
    return 6;
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
    data.append (kFMS_DS_Version);
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
    data.append (kTagGeneral);
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
        config()->setEnabled (control & kEnabled);

        /* Get FMS robot mode */
        DS::ControlMode mode;
        if (control & kTeleoperated)
            mode = DS::kControlTeleoperated;
        else if (control & kAutonomous)
            mode = DS::kControlAutonomous;
        else if (control & kTest)
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
    bool has_code       = (status & kRobotHasCode);
    bool e_stopped      = (control & kEmergencyStop);
    bool voltage_brwn   = (control & kVoltageBrownout);

    /* Update client information */
    config()->setRobotCode (has_code);
    config()->setBrownout  (voltage_brwn);
    m_sendDateTime = (request == kRequestTime);

    /* Update emergency stop state */
    if (e_stopped && !config()->isEmergencyStopped())
        config()->setEmergencyStop (true);
    else if (!e_stopped && config()->isEmergencyStopped())
        config()->setEmergencyStop (false);

    /* Calculate the voltage */
    float voltage = (integer + ((float) (decimal) * 99 / 255 / 100));
    config()->updateVoltage (voltage);

    /* If voltage is 0, then robot is simulated */
    config()->updateSimulated (config()->voltage() == 0);

    /* This is an extended packet, read its extra data */
    if (data.size() > 8) {
        std::string extended = std::string (data.constData(), data.length());
        readExtended (QString::fromStdString (extended.substr (8)).toUtf8());
    }

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
    data.append (kTagDate);
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
    data.append (kTagTimezone);
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
        data.append (kTagJoystick);

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
    if (station == kBlue1
        || station == kBlue2
        || station == kBlue3)
        return DS::kAllianceBlue;

    return DS::kAllianceRed;
}

/**
 * This function returns the position referenced by the given \a station code.
 * This function is used to follow the instructions outlined by the FMS packets.
 */
DS::Position FRC_2015::getPosition (quint8 station)
{
    if (station == kRed1 || station == kBlue1)
        return DS::kPosition1;

    if (station == kRed2 || station == kBlue2)
        return DS::kPosition2;

    if (station == kRed3 || station == kBlue3)
        return DS::kPosition3;

    return DS::kPosition1;
}

/**
 * Sometimes, the roboRIO will send us additional information, such as CPU
 * usage and CAN metrics. This function is in charge of extracting this
 * information from the packet and updating DS values accordingly.
 */
void FRC_2015::readExtended (const QByteArray& data)
{
    if (data.isEmpty() || data.length() < 2)
        return;

    uint tag = data.at (1);

    if (tag == kRTagJoystickOut) {
        /* TODO */
    }

    else if (tag == kRTagCpuInfo) {
        int count = data.at (2);
        for (int i = 0; i < count; ++i)
            if (data.length() > i + 12)
                config()->updateCpuUsage (data.at (i + 12));
    }

    else if (tag == kRTagMemInfo) {
        if (data.length() > 5)
            config()->updateRamUsage (data.at (5));
    }

    else if (tag == kRTagDiskInfo) {
        if (data.length() > 5)
            config()->updateDiskUsage (data.at (5));
    }
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
        code |= kTest;
        break;
    case DS::kControlAutonomous:
        code |= kAutonomous;
        break;
    case DS::kControlTeleoperated:
        code |= kTeleoperated;
        break;
    default:
        break;
    }

    if (config()->isFMSAttached())
        code |= kFMS_Attached;

    if (config()->isEmergencyStopped())
        code |= kEmergencyStop;

    if (config()->isEnabled())
        code |= kEnabled;

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
    uint code = kRequestUnconnected;

    if (config()->isConnectedToRobot())
        code = kRequestNormal;

    if (config()->isConnectedToRobot() && m_rebootRobot)
        code |= kRequestReboot;

    if (config()->isConnectedToRobot() && m_restartCode)
        code |= kRequestRestartCode;

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
        code |= kTest;
        break;
    case DS::kControlAutonomous:
        code |= kAutonomous;
        break;
    case DS::kControlTeleoperated:
        code |= kTeleoperated;
        break;
    default:
        break;
    }

    if (config()->isEmergencyStopped())
        code |= kEmergencyStop;

    if (config()->isEnabled())
        code |= kEnabled;

    if (config()->isConnectedToRadio())
        code |= kFMS_RadioPing;

    if (config()->isConnectedToRobot()) {
        code |= kFMS_RobotComms;
        code |= kFMS_RobotPing;
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
            return kRed1;
        else
            return kBlue1;
    }

    if (config()->position() == DS::kPosition2) {
        if (config()->alliance() == DS::kAllianceRed)
            return kRed2;
        else
            return kBlue2;
    }

    if (config()->position() == DS::kPosition3) {
        if (config()->alliance() == DS::kAllianceRed)
            return kRed3;
        else
            return kBlue3;
    }

    return kRed1;
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
