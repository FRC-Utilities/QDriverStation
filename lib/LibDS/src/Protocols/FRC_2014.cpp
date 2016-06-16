/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#include "FRC_2014.h"

/**
 * Represents the basic operation codes of the robot
 */
enum Control {
    kEnabled          = 0x20, /**< Enabled flag */
    kTestMode         = 0x02, /**< Test operation mode */
    kAutonomous       = 0x10, /**< Autonomous operation mode */
    kTeleoperated     = 0x00, /**< Teleoperated operation mode */
    kFMS_Attached     = 0x08, /**< Sent to robot when we are connected to FMS */
    kEmergencyStopOn  = 0x00, /**< Robot program is stopped */
    kEmergencyStopOff = 0x40, /**< Normal operation of the robot program */
};

/**
 * Represents the special instructions that we can send to the robot
 */
enum Instructions {
    kResyncComms = 0x04, /**< Resyncs the DS and the robot loops */
    kRebootRobot = 0x80, /**< Restarts the robot controller */
};

/**
 * Represents the available team station
 */
enum Stations {
    kPosition1    = 0x31, /**< Position 1 on any alliance */
    kPosition2    = 0x32, /**< Position 2 on any alliance */
    kPosition3    = 0x33, /**< Position 3 on any alliance */
    kAllianceRed  = 0x52, /**< Set alliance to red */
    kAllianceBlue = 0x42, /**< Set alliance to blue */
};

/**
 * Implements the 2009-2014 FRC communication protocol
 */
FRC_2014::FRC_2014() {
    m_resync = true;
    m_restartCode = false;
    m_rebootRobot = false;
}

/**
 * Returns the display name of the protocol
 */
QString FRC_2014::name() {
    return QObject::tr ("FRC 2014 Protocol");
}

/**
 * Send FMS packets 2 times per second
 */
int FRC_2014::fmsFrequency() {
    return 2;
}

/**
 * Send 50 robot packets per second
 */
int FRC_2014::robotFrequency() {
    return 50;
}

/**
 * We receive data from FMS at local port 1120
 */
int FRC_2014::fmsInputPort() {
    return 1120;
}

/**
 * We send data to the FMS to remote port 1160
 */
int FRC_2014::fmsOutputPort() {
    return 1160;
}

/**
 * We receive data from the robot at local port 1150
 */
int FRC_2014::robotInputPort() {
    return 1150;
}

/**
 * We send data to the robot at remote port 1110
 */
int FRC_2014::robotOutputPort() {
    return 1110;
}

/**
 * FRC 2014 protocol does not use POVs
 */
int FRC_2014::maxPOVCount() {
    return 0;
}

/**
 * FRC 2014 protocol supports a maximum of 6 axes.
 * Remaining axes will be ignored.
 */
int FRC_2014::maxAxisCount() {
    return 6;
}

/**
 * FRC 2014 protocol supports a maximum of 12 buttons.
 * Remaining buttons will be ignored.
 */
int FRC_2014::maxButtonCount() {
    return 12;
}

/**
 * FRC 2014 Protocol supports a maximum of 4 joysticks.
 * Remaining joysticks will be ignored.
 */
int FRC_2014::maxJoystickCount() {
    return 4;
}

/**
 * Configures the protocol to reboot the robot with the next
 * sent packet.
 */
void FRC_2014::rebootRobot() {
    m_rebootRobot = true;
}

/**
 * Configures the protocol to restart the robot code with the next
 * sent packet (not really, it does not work for the moment).
 */
void FRC_2014::restartRobotCode() {
    m_restartCode = true;
}

/**
 * Be sure that we do not reboot the robot or restart its code when
 * we re-establish communications with it.
 */
void FRC_2014::onRobotWatchdogExpired() {
    m_resync = true;
    m_restartCode = false;
    m_rebootRobot = false;
}

/**
 * FMS communications work with UDP datagrams
 */
DS::SocketType FRC_2014::fmsSocketType() {
    return DS::kSocketTypeUDP;
}

/**
 * Robot communications work with UDP datagrams
 */
DS::SocketType FRC_2014::robotSocketType() {
    return DS::kSocketTypeUDP;
}

/**
 * Radio is located at 10.XX.YY.1
 */
QString FRC_2014::defaultRadioAddress() {
    return DS::getStaticIP (10, config()->team(), 1);
}

/**
 * Robot is located at 10.XX.YY.2
 */
QString FRC_2014::defaultRobotAddress() {
    return DS::getStaticIP (10, config()->team(), 2);
}

/**
 * \todo Implement this function
 */
QByteArray FRC_2014::getFMSPacket() {
    QByteArray data;
    return data;
}

/**
 * Generates a packet that the DS will send to the robot
 */
QByteArray FRC_2014::getRobotPacket() {
    QByteArray data;

    /* Setup the base packet */
    data.resize (1024);
    data.fill   (0x00);

    /* Add packet index */
    data[0] = (sentRobotPackets() & 0xff00) >> 8;
    data[1] = (sentRobotPackets() & 0xff);

    /* Add team number */
    data[4] = (config()->team() & 0xff00) >> 8;
    data[5] = (config()->team() & 0xff);

    /* Add operation code, empty digital input and alliance & position */
    data[2] = getOperationCode();
    data[3] = getDigitalInput();
    data[6] = getAlliance();
    data[7] = getPosition();

    /* Add joystick data */
    QByteArray joysticks = getJoystickData();
    data.replace (8, joysticks.length(), joysticks);

    /* Add FRC Driver Station version (same as the one sent by 16.0.1) */
    data[72] = (quint8) 0x30;
    data[73] = (quint8) 0x34;
    data[74] = (quint8) 0x30;
    data[75] = (quint8) 0x31;
    data[76] = (quint8) 0x31;
    data[77] = (quint8) 0x36;
    data[78] = (quint8) 0x30;
    data[79] = (quint8) 0x30;

    /* Add CRC checksum */
    m_crc32.update (data);
    uint checksum = m_crc32.value();
    data[1020] = (checksum & 0xff000000) >> 24;
    data[1021] = (checksum & 0xff0000) >> 16;
    data[1022] = (checksum & 0xff00) >> 8;
    data[1023] = (checksum & 0xff);

    return data;
}

/**
 * \todo Implement this function
 */
bool FRC_2014::interpretFMSPacket (const QByteArray& data) {
    Q_UNUSED (data);
    return false;
}

/**
 * Reads the input \a data and extracts the robot voltage, the
 * robot code status and the emergency stop status.
 *
 * If the packet is read successfully, the function returns \c true.
 *
 * \note If both voltage values are equal to \c 0x37, it means that
 *       the robot code is not running
 */
bool FRC_2014::interpretRobotPacket (const QByteArray& data) {
    /* The packet is smaller than it should be */
    if (data.length() < 1024) {
        qWarning() << name() << "received an invalid robot packet";
        return false;
    }

    /* Read status echo code and voltage */
    uint opcode  = data.at (0);
    uint integer = data.at (1);
    uint decimal = data.at (2);

    /* Parse the voltage (which is stored in a strange format) */
    QString voltage;
    QByteArray hex = data.toHex();
    voltage.append (hex.at (2));
    voltage.append (hex.at (3));
    voltage.append (".");
    voltage.append (hex.at (4));
    voltage.append (hex.at (5));

    /* The robot seems to be emergency stopped */
    if (opcode == kEmergencyStopOn && !config()->isEmergencyStopped())
        config()->updateOperationStatus (DS::kEmergencyStop);

    /* Update code status & voltage */
    bool hasCode = (integer != 0x37) && (decimal != 0x37);
    config()->setRobotCode (hasCode);
    config()->updateVoltage (hasCode ? voltage.toFloat() : 0);

    /* Update simulation status */
    if (receivedRobotPackets() > 10)
        config()->updateSimulated (voltage == "12.43");

    /* Packet read successfully */
    return true;
}

/**
 * Returns the code that represents the current alliance color
 */
quint8 FRC_2014::getAlliance() {
    if (config()->alliance() == DS::kAllianceBlue)
        return kAllianceBlue;

    return kAllianceRed;
}

/**
 * Returns the code that represents the current team position
 */
quint8 FRC_2014::getPosition() {
    if (config()->position() == DS::kPosition1)
        return kPosition1;

    if (config()->position() == DS::kPosition2)
        return kPosition2;

    if (config()->position() == DS::kPosition3)
        return kPosition3;

    return kPosition1;
}

/**
 * \todo Allow the LibDS to support digital inputs
 */
quint8 FRC_2014::getDigitalInput() {
    return 0x00;
}

/**
 * Returns the code used to identify the enable status, control mode,
 * operation mode and operation flags.
 */
quint8 FRC_2014::getOperationCode() {
    quint8 code = kEmergencyStopOff;
    quint8 enabled = config()->isEnabled() ? kEnabled : 0x00;

    switch (config()->controlMode()) {
    case DS::kControlTest:
        code |= enabled + kTestMode;
        break;
    case DS::kControlAutonomous:
        code |= enabled + kAutonomous;
        break;
    case DS::kControlTeleoperated:
        code |= enabled + kTeleoperated;
        break;
    default:
        code = kEmergencyStopOff;
        break;
    }

    if (m_resync)
        code |= kResyncComms;

    if (config()->isFMSAttached())
        code |= kFMS_Attached;

    if (config()->isEmergencyStopped())
        code = kEmergencyStopOn;

    if (m_rebootRobot)
        code = kRebootRobot;

    return code;
}

/**
 * Generates the joystick data. The FRC protocol defines joystick values
 * for all joysticks, regardless if all joysticks are connected or not.
 *
 * In the case that a joystick is not connected, the protocol will send
 * a netrual value (e.g. \c 0 for each axis and \c false for each button).
 */
QByteArray FRC_2014::getJoystickData() {
    QByteArray data;

    for (int i = 0; i < maxJoystickCount(); ++i) {
        bool joystickExists = joysticks()->count() > i;

        int numAxes = joystickExists ? joysticks()->at (i)->numAxes : 0;
        int numButtons = joystickExists ? joysticks()->at (i)->numButtons : 0;

        for (int axis = 0; axis < maxAxisCount(); ++axis) {
            if (joystickExists && axis < numAxes)
                data.append (joysticks()->at (i)->axes [axis] * 127);
            else
                data.append (static_cast<char>(0x00));
        }

        int button_data = 0;
        for (int button = 0; button < numButtons; ++button) {
            if (joystickExists && joysticks()->at (i)->buttons [button])
                button_data |= static_cast<int>(qPow (2, button));
        }

        data.append ((button_data & 0xff00) >> 8);
        data.append ((button_data & 0xff));
    }

    return data;
}
