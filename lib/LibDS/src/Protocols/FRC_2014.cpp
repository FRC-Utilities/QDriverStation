/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#include "FRC_2014.h"

/**
 * Constants used to encode the control byte
 */
enum Control {
    cEnabled          = 0x20, /**< DS enables the robot */
    cTestMode         = 0x02, /**< Set operation mode to test mode */
    cAutonomous       = 0x10, /**< Set operation mode to autonomous mode */
    cTeleoperated     = 0x00, /**< Set operation mode to teleoperated mode */
    cFMS_Attached     = 0x08, /**< Sent when DS has comms. with FMS */
    cResyncComms      = 0x04, /**< Resyncs the communications with robot */
    cRebootRobot      = 0x80, /**< Reboots the robot controller */
    cEmergencyStopOn  = 0x00, /**< Enables the emergency stop */
    cEmergencyStopOff = 0x40, /**< Disables the emergency stop */
};

/**
 * Constants used to encode the alliance and position of the robot
 */
enum Stations {
    cPosition1    = 0x31, /**< Position 1 */
    cPosition2    = 0x32, /**< Position 2 */
    cPosition3    = 0x33, /**< Position 3 */
    cAllianceRed  = 0x52, /**< Red alliance */
    cAllianceBlue = 0x42, /**< Blue alliance */
};

/**
 * Constants used to encode the FMS states
 */
enum FMS {
    cFMSAutonomous   = 0x53, /**< Sent by FMS to switch robot mode to Auto */
    cFMSTeleoperated = 0x43, /**< Sent by FMS to switch robot mode to Teleop */
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
 * Send 10 FMS packets per second
 */
int FRC_2014::fmsFrequency() {
    return 10;
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
 * Returns the nominal/maximum voltage given by the robot battery.
 */
qreal FRC_2014::nominalBatteryVoltage() {
    return 12.8;
}

/**
 * Returns the nominal amperage given by the robot battery
 */
qreal FRC_2014::nominalBatteryAmperage() {
    return 17;
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
 * Radio is located at 10.TE.AM.1
 */
QString FRC_2014::radioAddress() {
    return DS::getStaticIP (10, config()->team(), 1);
}

/**
 * Robot is located at 10.TE.AM.2
 */
QString FRC_2014::robotAddress() {
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
    data[72] = (DS_UByte) 0x30;
    data[73] = (DS_UByte) 0x34;
    data[74] = (DS_UByte) 0x30;
    data[75] = (DS_UByte) 0x31;
    data[76] = (DS_UByte) 0x31;
    data[77] = (DS_UByte) 0x36;
    data[78] = (DS_UByte) 0x30;
    data[79] = (DS_UByte) 0x30;

    /* Add CRC checksum */
    m_crc32.update (data);
    DS_UByte checksum = m_crc32.value();
    data[1020] = (checksum & 0xff000000) >> 24;
    data[1021] = (checksum & 0xff0000) >> 16;
    data[1022] = (checksum & 0xff00) >> 8;
    data[1023] = (checksum & 0xff);

    return data;
}

/**
 * Gets the team station and robot mode from the FMS
 */
bool FRC_2014::interpretFMSPacket (const QByteArray& data) {
    /* The packet is smaller than what it should be */
    if (data.length() < 74) {
        qWarning() << name() << "received an invalid FMS packet";
        return false;
    }

    /* Read the parts of the packet that interest us */
    DS_UByte robotmod = data.at (2);
    DS_UByte alliance = data.at (3);
    DS_UByte position = data.at (4);

    /* Get the operation mode & enable status */
    DS::ControlMode mode;
    DS::EnableStatus enabled;
    switch (robotmod) {
    case (cFMSAutonomous):
        enabled = DS::kDisabled;
        mode = DS::kControlAutonomous;
        break;
    case (cFMSAutonomous | cEnabled):
        enabled = DS::kEnabled;
        mode = DS::kControlAutonomous;
        break;
    case (cFMSTeleoperated):
        enabled = DS::kDisabled;
        mode = DS::kControlTeleoperated;
        break;
    case (cFMSTeleoperated | cEnabled):
        enabled = DS::kEnabled;
        mode = DS::kControlTeleoperated;
        break;
    default:
        enabled = DS::kDisabled;
        mode = DS::kControlTeleoperated;
        break;
    }

    /* Set robot mode and team station */
    config()->updateEnabled (enabled);
    config()->updateControlMode (mode);
    config()->updateAlliance (getAlliance (alliance));
    config()->updatePosition (getPosition (position));

    return true;
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
    /* The packet is smaller than what it should be */
    if (data.length() < 1024) {
        qWarning() << name() << "received an invalid robot packet";
        return false;
    }

    /* Read status echo code and voltage */
    DS_UByte opcode  = data.at (0);
    DS_UByte integer = data.at (1);
    DS_UByte decimal = data.at (2);

    /* Parse the voltage (which is stored in a strange format) */
    QString voltage;
    QByteArray hex = data.toHex();
    voltage.append (hex.at (2));
    voltage.append (hex.at (3));
    voltage.append (".");
    voltage.append (hex.at (4));
    voltage.append (hex.at (5));

    /* The robot seems to be emergency stopped */
    if (opcode == cEmergencyStopOn && !config()->isEmergencyStopped())
        config()->updateOperationStatus (DS::kEmergencyStop);

    /* Update code status & voltage */
    bool hasCode = (integer != 0x37) && (decimal != 0x37);
    config()->setRobotCode (hasCode);
    config()->updateVoltage (hasCode ? voltage.toDouble() : 0);

    /* Update simulation status */
    if (receivedRobotPackets() > 10)
        config()->updateSimulated (voltage == "12.43");

    /* Packet read successfully */
    return true;
}

/**
 * Returns the code that represents the current alliance color
 */
DS_UByte FRC_2014::getAlliance() {
    if (config()->alliance() == DS::kAllianceBlue)
        return cAllianceBlue;

    return cAllianceRed;
}

/**
 * Returns the code that represents the current team position
 */
DS_UByte FRC_2014::getPosition() {
    if (config()->position() == DS::kPosition1)
        return cPosition1;

    if (config()->position() == DS::kPosition2)
        return cPosition2;

    if (config()->position() == DS::kPosition3)
        return cPosition3;

    return cPosition1;
}

/**
 * \todo Allow the LibDS to support digital inputs
 */
DS_UByte FRC_2014::getDigitalInput() {
    return 0x00;
}

/**
 * Returns the code used to identify the enable status, control mode,
 * operation mode and operation flags.
 */
DS_UByte FRC_2014::getOperationCode() {
    DS_UByte code = cEmergencyStopOff;
    DS_UByte enabled = config()->isEnabled() ? cEnabled : 0x00;

    /* Get the control mode (Test, Auto or TeleOp) */
    switch (config()->controlMode()) {
    case DS::kControlTest:
        code |= enabled + cTestMode;
        break;
    case DS::kControlAutonomous:
        code |= enabled + cAutonomous;
        break;
    case DS::kControlTeleoperated:
        code |= enabled + cTeleoperated;
        break;
    default:
        code = cEmergencyStopOff;
        break;
    }

    /* Resync robot communications */
    if (m_resync)
        code |= cResyncComms;

    /* Let robot know if we are connected to FMS */
    if (config()->isFMSAttached())
        code |= cFMS_Attached;

    /* Set the emergency stop state */
    if (config()->isEmergencyStopped())
        code = cEmergencyStopOn;

    /* Send the reboot code if required */
    if (m_rebootRobot)
        code = cRebootRobot;

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

        /* Get number of axes & buttons */
        int numAxes = joystickExists ? joysticks()->at (i)->numAxes : 0;
        int numButtons = joystickExists ? joysticks()->at (i)->numButtons : 0;

        /* Add axis values */
        for (int axis = 0; axis < maxAxisCount(); ++axis) {
            /* Joystick connected, add real data */
            if (joystickExists && axis < numAxes)
                data.append ((DS_SByte) (joysticks()->at (i)->axes [axis] * 127));

            /* Joystick disconnected, add neutral data */
            else
                data.append (static_cast<char> (0x00));
        }

        /* Calculate value of buttons */
        int button_data = 0;
        for (int button = 0; button < numButtons; ++button) {
            if (joystickExists && joysticks()->at (i)->buttons [button])
                button_data |= static_cast<int> (qPow (2, button));
        }

        /* Add button data */
        data.append ((button_data & 0xff00) >> 8);
        data.append ((button_data & 0xff));
    }

    return data;
}

/**
 * Gets the alliance from the received \a byte
 */
DS::Alliance FRC_2014::getAlliance (DS_UByte byte) {
    if (byte == cAllianceBlue)
        return DS::kAllianceBlue;

    return DS::kAllianceRed;
}

/**
 * Gets the position from the received \a byte
 */
DS::Position FRC_2014::getPosition (DS_UByte byte) {
    if (byte == cPosition1)
        return DS::kPosition1;

    if (byte == cPosition2)
        return DS::kPosition2;

    if (byte == cPosition3)
        return DS::kPosition3;

    return DS::kPosition1;
}
