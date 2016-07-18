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
    cTest          = 0x01, /**< Test operation mode */
    cEnabled       = 0x04, /**< Sent when DS enables the robot */
    cAutonomous    = 0x02, /**< Autonomous operation mode */
    cTeleoperated  = 0x00, /**< Teleoperated operation mode */
    cFMS_Attached  = 0x08, /**< Sent to robot when we are connected with FMS */
    cEmergencyStop = 0x80, /**< Robot code is stopped */
};

/**
 * Holds the different operation states sent to the robot
 */
enum DS_Flags {
    cRequestReboot      = 0x08, /**< Sent when DS wants a robot reboot */
    cRequestNormal      = 0x80, /**< Sent when DS has comms. with robot */
    cRequestUnconnected = 0x00, /**< Sent when DS has no comms. with robot */
    cRequestRestartCode = 0x04, /**< Sent when DS wants to restart robot code */
};

/**
 * Holds different flags that are sent to the FMS
 */
enum FMS_Control {
    cFMS_RadioPing  = 0x10, /**< Sent if TCP ping with radio is successfull */
    cFMS_RobotPing  = 0x08, /**< Sent if TCP ping with robot is successfull */
    cFMS_RobotComms = 0x20, /**< Sent if DS has comms. with robot */
    cFMS_DS_Version = 0x00, /**< Sends DS version to the FMS */
};

/**
 * Holds the different tags/headers of each section of the sent packets
 */
enum DS_Tags {
    cTagDate     = 0x0f, /**< Staart of date & time section */
    cTagGeneral  = 0x01, /**< Start of control/basic section */
    cTagJoystick = 0x0c, /**< Start of joystick input section */
    cTagTimezone = 0x10, /**< Start of timezone section */
};

/**
 * Represents the different team stations
 */
enum Stations {
    cRed1  = 0x00, /**< Red Alliance, Position 1 */
    cRed2  = 0x01, /**< Red Alliance, Position 2 */
    cRed3  = 0x02, /**< Red Alliance, Position 3 */
    cBlue1 = 0x03, /**< Blue Alliance, Position 1 */
    cBlue2 = 0x04, /**< Blue Alliance, Position 2 */
    cBlue3 = 0x05, /**< Blue Alliance, Position 3 */
};

/**
 * Represents the tags that can be sent by the robot with the extended packets
 */
enum Robot_Tags {
    cRTagCpuInfo     = 0x05, /**< Robot program sents CPU usage */
    cRTagMemInfo     = 0x06, /**< Robot program sends RAM usage */
    cRTagDiskInfo    = 0x04, /**< Robot program sends disk usage */
    cRTagJoystickOut = 0x01, /**< Robot program wants to rumble joysticks */
};

/**
 * Different robot requests & operation status flags
 */
enum Robot_Data {
    cRequestTime     = 0x01, /**< Robot wants current date & time */
    cRobotHasCode    = 0x20, /**< Robot has user code loaded */
    cVoltageBrownout = 0x10, /**< Robot experiences a voltage brownout */
};

/**
 * \brief Holds a battery voltage bytes and resultant \c float
 */
struct BatteryVoltage {
    qreal voltage;
    quint16 integer;
    quint16 decimal;

    void updateValues (qreal volt) {
        voltage = roundf (volt * 100) / 100;
        integer = static_cast<int> (volt);
        decimal = (volt - integer) * 100;
    }

    void updateValues (DS_Byte a, DS_Byte b) {
        integer = a;
        decimal = (roundf ((static_cast<qreal> (b) / 255) * 100) / 100) * 100;
        voltage = integer + (static_cast<qreal> (decimal) / 100);
    }
};

/**
 * Implements the 2015 FRC Communication protocol
 */
FRC_2015::FRC_2015() {
    m_restartCode = false;
    m_rebootRobot = false;
    m_sendDateTime = false;
}

/**
 * Returns the display name of the protocol
 */
QString FRC_2015::name() {
    return QObject::tr ("FRC 2015 Protocol");
}

/**
 * Send 2 FMS packets every second
 */
int FRC_2015::fmsFrequency() {
    return 2;
}

/**
 * Send 50 robot packets every second
 */
int FRC_2015::robotFrequency() {
    return 50;
}

/**
 * We receive data from FMS at local port 1120
 */
int FRC_2015::fmsInputPort() {
    return 1120;
}

/**
 * We send data to the FMS to remote port 1160
 */
int FRC_2015::fmsOutputPort() {
    return 1160;
}

/**
 * We receive data from the robot at local port 1150
 */
int FRC_2015::robotInputPort() {
    return 1150;
}

/**
 * We send data to the robot at remote port 1110
 */
int FRC_2015::robotOutputPort() {
    return 1110;
}

/**
 * We received broadcasted messages from the robot in port 6666
 */
int FRC_2015::netconsoleInputPort() {
    return 6666;
}

/**
 * FRC 2015 protocol supports only 1 POV.
 * Remaining POVs will be ignored.
 */
int FRC_2015::maxPOVCount() {
    return 1;
}

/**
 * FRC 2015 protocol supports a maximum of 6 axes.
 * Remaining axes will be ignored.
 */
int FRC_2015::maxAxisCount() {
    return 6;
}

/**
 * FRC 2015 protocol supports a maximum of 12 buttons.
 * Remaining buttons will be ignored.
 */
int FRC_2015::maxButtonCount() {
    return 12;
}

/**
 * FRC 2015 Protocol supports a maximum of 6 joysticks.
 * Remaining joysticks will be ignored.
 */
int FRC_2015::maxJoystickCount() {
    return 6;
}

/**
 * Reboots the robot in the next packet cycle
 */
void FRC_2015::rebootRobot() {
    m_rebootRobot = true;
}

/**
 * Restarts the robot code in the next packet cycle
 */
void FRC_2015::restartRobotCode() {
    m_restartCode = true;
}

/**
 * Do not reboot robot or restart code when we re-establish communications
 * with the robot.
 */
void FRC_2015::onRobotWatchdogExpired() {
    m_restartCode = false;
    m_rebootRobot = false;
    m_sendDateTime = false;
}

/**
 * Returns the nominal/maximum voltage given by the robot battery.
 */
qreal FRC_2015::nominalBatteryVoltage() {
    return 12.8;
}

/**
 * Returns the nominal amperage given by the robot battery
 */
qreal FRC_2015::nominalBatteryAmperage() {
    return 17;
}

/**
 * FMS communications work with UDP datagrams
 */
DS::SocketType FRC_2015::fmsSocketType() {
    return DS::kSocketTypeUDP;
}

/**
 * Robot communications work with UDP datagrams
 */
DS::SocketType FRC_2015::robotSocketType() {
    return DS::kSocketTypeUDP;
}

/**
 * Default radio address is 10.TE.AM.1
 */
QString FRC_2015::radioAddress() {
    return DS::getStaticIP (10, config()->team(), 1);
}

/**
 * Default robot address is roboRIO-TEAM.local
 */
QString FRC_2015::robotAddress() {
    return QString ("roboRIO-%1.local").arg (config()->team());
}

/**
 * Generates a packet that the DS will send to the FMS
 */
QByteArray FRC_2015::getFMSPacket() {
    QByteArray data;
    BatteryVoltage voltage;

    voltage.updateValues (config()->voltage());

    data.append ((sentFMSPackets() & 0xff00) >> 8);
    data.append ((sentFMSPackets()) & 0xff);
    data.append (cFMS_DS_Version);
    data.append (getFMSControlCode());
    data.append ((config()->team() & 0xff00) >> 8);
    data.append ((config()->team()) & 0xff);
    data.append (voltage.integer);
    data.append (voltage.decimal);

    return data;
}

/**
 * Generates a packet that the DS will send to the robot
 */
QByteArray FRC_2015::getRobotPacket() {
    QByteArray data;

    data.append ((sentRobotPackets() & 0xff00) >> 8);
    data.append ((sentRobotPackets()) & 0xff);
    data.append (cTagGeneral);
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
bool FRC_2015::interpretFMSPacket (const QByteArray& data) {
    if (data.length() >= 22) {
        DS_Byte control = data.at (3);
        DS_Byte station = data.at (5);

        /* Change robot enabled state based on what FMS tells us to do*/
        config()->setEnabled (control & cEnabled);

        /* Get FMS robot mode */
        if (control & cTeleoperated)
            config()->updateControlMode (DS::kControlTeleoperated);
        else if (control & cAutonomous)
            config()->updateControlMode (DS::kControlAutonomous);
        else if (control & cTest)
            config()->updateControlMode (DS::kControlTest);

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
bool FRC_2015::interpretRobotPacket (const QByteArray& data) {
    /* Packet is invalid */
    if (data.length() < 8) {
        qWarning() << name() << "received an invalid robot packet";
        return false;
    }

    /* Read robot packet */
    DS_Byte control = data.at (3);
    DS_Byte status  = data.at (4);
    DS_Byte request = data.at (7);

    /* Generate control information */
    bool has_code       = (status & cRobotHasCode);
    bool e_stopped      = (control & cEmergencyStop);
    bool voltage_brwn   = (control & cVoltageBrownout);

    /* Update client information */
    config()->setRobotCode (has_code);
    config()->setBrownout  (voltage_brwn);
    m_sendDateTime = (request == cRequestTime);

    /* Update emergency stop state */
    config()->setEmergencyStop (e_stopped);

    /* Calculate the voltage */
    BatteryVoltage voltage;
    voltage.updateValues (data.at (5), data.at (6));
    config()->updateVoltage (voltage.voltage);

    /* If voltage is 0, then robot is simulated */
    if (receivedRobotPackets() > 10)
        config()->updateSimulated (voltage.voltage == 0.00);

    /* This is an extended packet, read its extra data */
    if (data.size() > 8) {
        std::string extended = std::string (data.constData(), data.length());
        readExtended (QString::fromStdString (extended.substr (8)).toUtf8());
    }

    /* Packet read, feed the watchdog some meat */
    return true;
}

/**
 * Returns information regarding the current date and time and the timezone
 * of the client computer.
 *
 * The robot may ask for this information in some cases (e.g. when initializing
 * the robot code).
 */
QByteArray FRC_2015::getTimezoneData() {
    QByteArray data;

    /* Add size (always 11) */
    data.append (0x0B);

    /* Get current date/time */
    QDateTime dt = QDateTime::currentDateTime();
    QDate date = dt.date();
    QTime time = dt.time();

    /* Add current date/time */
    data.append (cTagDate);
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
    data.append (cTagTimezone);
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
QByteArray FRC_2015::getJoystickData() {
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
        data.append (cTagJoystick);

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
DS::Alliance FRC_2015::getAlliance (DS_Byte station) {
    /* Station code is any one of the blue  */
    if (station == cBlue1
            || station == cBlue2
            || station == cBlue3)
        return DS::kAllianceBlue;

    /* Station code is not blue, so it must be red */
    return DS::kAllianceRed;
}

/**
 * This function returns the position referenced by the given \a station code.
 * This function is used to follow the instructions outlined by the FMS packets.
 */
DS::Position FRC_2015::getPosition (DS_Byte station) {
    /* We are on position 1, regardless of the alliance */
    if (station == cRed1 || station == cBlue1)
        return DS::kPosition1;

    /* We are on position 2, regardless of the alliance */
    if (station == cRed2 || station == cBlue2)
        return DS::kPosition2;

    /* We are on position 3, regardless of the alliance */
    if (station == cRed3 || station == cBlue3)
        return DS::kPosition3;

    /* I'll will chop of one of my balls if this happens */
    return DS::kPosition1;
}

/**
 * Sometimes, the roboRIO will send us additional information, such as CPU
 * usage and CAN metrics. This function is in charge of extracting this
 * information from the packet and updating DS values accordingly.
 */
void FRC_2015::readExtended (const QByteArray& data) {
    if (data.isEmpty() || data.length() < 2)
        return;

    DS_Byte tag = data.at (1);

    /* Robot wants to "rumble" the joystick */
    if (tag == cRTagJoystickOut) {
        /* TODO */
    }

    /* Packet contains information about the CPU */
    else if (tag == cRTagCpuInfo) {
        int count = data.at (2);
        for (int i = 0; i < count; ++i)
            if (data.length() > i + 12)
                config()->updateCpuUsage (data.at (i + 12));
    }

    /* Packet contains information about the RAM */
    else if (tag == cRTagMemInfo) {
        if (data.length() > 5)
            config()->updateRamUsage (data.at (5));
    }

    /* Packat contains information about the CANBUS */
    else if (tag == cRTagDiskInfo) {
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
DS_Byte FRC_2015::getControlCode() {
    DS_Byte code = 0;

    /* Get current control mode (Test, Auto or Teleop) */
    switch (config()->controlMode()) {
    case DS::kControlTest:
        code |= cTest;
        break;
    case DS::kControlAutonomous:
        code |= cAutonomous;
        break;
    case DS::kControlTeleoperated:
        code |= cTeleoperated;
        break;
    default:
        break;
    }

    /* Let the robot know if we are connected to the FMS */
    if (config()->isFMSAttached())
        code |= cFMS_Attached;

    /* Let the robot know if it should e-stop right now */
    if (config()->isEmergencyStopped())
        code |= cEmergencyStop;

    /* Append the robot enabled state */
    if (config()->isEnabled())
        code |= cEnabled;

    return code;
}

/**
 * Generates the request code sent to the robot, which may instruct it to:
 *    - Operate normally
 *    - Reboot the RIO
 *    - Restart the robot code
 */
DS_Byte FRC_2015::getRequestCode() {
    /* Initialize a byte with the unconnected status (could trigger a resync in the robot) */
    DS_Byte code = cRequestUnconnected;

    /* Send a normal-operation status byte to the robot */
    if (config()->isConnectedToRobot())
        code = cRequestNormal;

    /* Send a reboot request byte to the robot */
    if (config()->isConnectedToRobot() && m_rebootRobot)
        code |= cRequestReboot;

    /* Send a restart code request byte to the robot */
    if (config()->isConnectedToRobot() && m_restartCode)
        code |= cRequestRestartCode;

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
DS_Byte FRC_2015::getFMSControlCode() {
    DS_Byte code = 0x00;

    /* Let the FMS know the operational status of the robot */
    switch (config()->controlMode()) {
    case DS::kControlTest:
        code |= cTest;
        break;
    case DS::kControlAutonomous:
        code |= cAutonomous;
        break;
    case DS::kControlTeleoperated:
        code |= cTeleoperated;
        break;
    default:
        break;
    }

    /* Let the FMS know if robot is e-stopped */
    if (config()->isEmergencyStopped())
        code |= cEmergencyStop;

    /* Let the FMS know if the robot is enabled */
    if (config()->isEnabled())
        code |= cEnabled;

    /* Let the FMS know if we are connected to radio */
    if (config()->isConnectedToRadio())
        code |= cFMS_RadioPing;

    /* Let the FMS know if we are connected to robot */
    if (config()->isConnectedToRobot()) {
        code |= cFMS_RobotComms;
        code |= cFMS_RobotPing;
    }

    return code;
}

/**
 * Returns the team station code sent to the robot.
 * This value may be used by the robot program to use specialized autonomous
 * modes or adjust sensor input.
 */
DS_Byte FRC_2015::getTeamStationCode() {
    /* Current config is set to position 1 */
    if (config()->position() == DS::kPosition1) {
        if (config()->alliance() == DS::kAllianceRed)
            return cRed1;
        else
            return cBlue1;
    }

    /* Current config is set to position 2 */
    if (config()->position() == DS::kPosition2) {
        if (config()->alliance() == DS::kAllianceRed)
            return cRed2;
        else
            return cBlue2;
    }

    /* Current config is set to position 3 */
    if (config()->position() == DS::kPosition3) {
        if (config()->alliance() == DS::kAllianceRed)
            return cRed3;
        else
            return cBlue3;
    }

    /* I'll will chop my other ball if this happens */
    return cRed1;
}

/**
 * Returns the size of the given \a joystick. This function is used to generate
 * joystick data (which is sent to the robot).
 *
 * This information will help the robot decide where a information starts and
 * ends for each attached joystick.
 */
DS_Byte FRC_2015::getJoystickSize (const DS::Joystick& joystick) {
    return  5
            + (joystick.numAxes > 0 ? joystick.numAxes : 0)
            + (joystick.numButtons / 8)
            + (joystick.numButtons % 8 == 0 ? 0 : 1)
            + (joystick.numPOVs > 0 ? joystick.numPOVs * 2 : 0);
}
