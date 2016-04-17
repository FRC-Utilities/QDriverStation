/*
 * Copyright (c) 2015-2016 WinT 3794 <http://wint3794.org>
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

#include "LibDS/Protocols/FRC_2015.h"

using namespace DS_Protocols;

//==================================================================================================
// Protocol bits
//==================================================================================================

#define ESTOP_ON             0x80 /* Emergency stop enabled */
#define ESTOP_OFF            0x00 /* Emergency stop disabled */
#define ENABLED              0x04 /* Robot enabled */
#define DISABLED             0x00 /* Robot disabled */
#define FMS_ATTACHED         0x01 /* FMS Attached bit */
#define CONTROL_TEST         0x01 /* Test mode */
#define CONTROL_AUTONOMOUS   0x02 /* Autonomous mode */
#define CONTROL_TELEOPERATED 0x00 /* Teleop mode */
#define HEADER_TIME          0x0f /* Indicates start of time section */
#define HEADER_GENERAL       0x01 /* Indicates start of packet data */
#define HEADER_JOYSTICK      0x0c /* Indicates start of joystick section */
#define HEADER_TIMEZONE      0x10 /* Indicates start of timezone code */
#define NORMAL               0x10 /* Robot operates normally */
#define INVALID              0x00 /* Sent when there is no communication */
#define REBOOT               0x14 /* Reboots the robot controller */
#define RESTART_CODE         0x16 /* Re-launches the robot code */
#define ALLIANCE_RED_1       0x00 /* Red alliance, position 1 */
#define ALLIANCE_RED_2       0x01 /* Red alliance, position 2 */
#define ALLIANCE_RED_3       0x02 /* Red alliance, position 3 */
#define ALLIANCE_BLUE_1      0x03 /* Blue alliance, position 1 */
#define ALLIANCE_BLUE_2      0x04 /* Blue alliance, position 2 */
#define ALLIANCE_BLUE_3      0x05 /* Blue alliance, position 3 */
#define HEADER_JOYSTICK_OUT  0x01 /* Joystick rumble request (from robot) */
#define HEADER_DISK_INFO     0x04 /* Robot disk storage information */
#define HEADER_CPU_INFO      0x05 /* Robot CPU information */
#define HEADER_RAM_INFO      0x06 /* Robot RAM information */
#define HEADER_CAN_METRICS   0x0e /* Robot CANBus metrics */
#define PROGRAM_TEST         0x01 /* Robot test mode echo */
#define PROGRAM_AUTONOMOUS   0x02 /* Robot autonomous echo */
#define PROGRAM_CODE_PRESENT 0x20 /* Robot code status */
#define PROGRAM_TELEOPERATED 0x00 /* Robot teleoperated echo */
#define PROGRAM_REQUEST_TIME 0x01 /* Robot requests a new time packet */

//==================================================================================================
// FRC_Protocol2015::name
//==================================================================================================

QString FRC_Protocol2015::name() {
    return "FRC 2015 Protocol";
}

//==================================================================================================
// FRC_Protocol2015::fmsFrequency
//==================================================================================================

int FRC_Protocol2015::fmsFrequency() {
    return 2;
}

//==================================================================================================
// FRC_Protocol2015::robotFrequency
//==================================================================================================

int FRC_Protocol2015::robotFrequency() {
    return 50;
}

//==================================================================================================
// FRC_Protocol2015::fmsInputPort
//==================================================================================================

int FRC_Protocol2015::fmsInputPort() {
    return 1120;
}

//==================================================================================================
// FRC_Protocol2015::fmsOutputPort
//==================================================================================================

int FRC_Protocol2015::fmsOutputPort() {
    return 1160;
}

//==================================================================================================
// FRC_Protocol2015::clientPort
//==================================================================================================

int FRC_Protocol2015::robotInputPort() {
    return 1150;
}

//==================================================================================================
// FRC_Protocol2015::robotPort
//==================================================================================================

int FRC_Protocol2015::robotOutputPort() {
    return 1110;
}

//==================================================================================================
// FRC_Protocol2015::tcpProbePort
//==================================================================================================

int FRC_Protocol2015::tcpProbesPort() {
    return 80;
}

//==================================================================================================
// FRC_Protocol2015::netConsoleInputPort
//==================================================================================================

int FRC_Protocol2015::netConsoleInputPort() {
    return 6666;
}

//==================================================================================================
// FRC_Protocol2015::acceptsConsoleCommnds
//==================================================================================================

bool FRC_Protocol2015::acceptsConsoleCommands() {
    return false;
}

//==================================================================================================
// FRC_Protocol2015::additionalRobotIPs
//==================================================================================================

QStringList FRC_Protocol2015::additionalRobotIPs() {
    QStringList list;
    list.append (QString ("roboRIO-%1.local").arg (team()));
    list.append (QString ("172.22.11.2"));
    return list;
}

//==================================================================================================
// FRC_Protocol2015::reboot
//==================================================================================================

void FRC_Protocol2015::reboot() {
    m_instructionCode = REBOOT;
}

//==================================================================================================
// FRC_Protocol2015::restartCode
//==================================================================================================

void FRC_Protocol2015::restartCode() {
    m_instructionCode = RESTART_CODE;
}

//==================================================================================================
// FRC_Protocol2015::resetProtocol
//==================================================================================================

void FRC_Protocol2015::resetProtocol() {
    m_instructionCode = INVALID;
}

//==================================================================================================
// FRC_Protocol2015::readFMSPacket
//==================================================================================================

bool FRC_Protocol2015::interpretFmsPacket (QByteArray data) {
    Q_UNUSED (data);
    return true;
}

//==================================================================================================
// FRC_Protocol2015::readPacket
//==================================================================================================

bool FRC_Protocol2015::interpretRobotPacket (QByteArray data) {
    int offset = 8;

    /* Packet length is invalid, watchdog will not be reset */
    if (data.length() < offset)
        return false;

    /* Be sure to reset status bit */
    if (m_instructionCode == INVALID)
        m_instructionCode = NORMAL;

    /* Read robot packet */
    quint8 opcode  = data.at (3);
    quint8 status  = data.at (4);
    quint8 request = data.at (7);
    quint8 integer = data.at (5);
    quint8 decimal = data.at (6);

    /* Update e-stop information */
    setEmergencyStop (opcode == ESTOP_ON);

    /* Update client information */
    updateVoltageBrownout (false);
    updateSendDateTime (request == PROGRAM_REQUEST_TIME);
    updateRobotCode ((status & PROGRAM_CODE_PRESENT) != 0);
    updateVoltage (QString::number (integer), QString::number (decimal));

    /* Packet contains additional data structures */
    if (data.length() > offset) {
        QByteArray extended;
        for (int i = offset; i < data.length() - offset - 1; i++)
            extended.append (data.at (i));

        int id = extended.at (1);

        /* Robot wants us to rumble a joystick */
        if (id == HEADER_JOYSTICK_OUT) {
            // TODO
        }

        /* Packet contains roboRIO CPU info */
        else if (id == HEADER_CPU_INFO) {
            // TODO
        }

        /* Packet contains roboRIO RAM info */
        else if (id == HEADER_RAM_INFO) {
            int a = (quint8) extended.at (6);
            int b = (quint8) extended.at (7);
            emit ramUsageChanged (a + b);
        }

        /* Packet contains roboRIO disk info */
        else if (id == HEADER_DISK_INFO) {
            int a = (quint8) extended.at (6);
            int b = (quint8) extended.at (7);
            emit diskUsageChanged (a + b);
        }

        /* Packet contains CAN metrics data */
        else if (id == HEADER_CAN_METRICS) {
            // TODO
        }
    }

    /* Packet was successfully read, reset watchdog */
    return true;
}

//==================================================================================================
// FRC_Protocol2015::getJoystickData
//==================================================================================================

QByteArray FRC_Protocol2015::getJoystickData() {
    QByteArray data;

    /* Do not send joystick data on DS init */
    if (sentRobotPackets() <= 5)
        return data;

    /* Generate data for each joystick */
    for (int i = 0; i < joysticks()->count(); ++i) {
        quint8 _num_axes     = joysticks()->at (i).numAxes;
        quint8 _num_buttons  = joysticks()->at (i).numButtons;
        quint8 _num_pov_hats = joysticks()->at (i).numPOVs;

        /* Add joystick information and put the section header */
        data.append (getJoystickSize (joysticks()->at (i)) - 1);
        data.append (HEADER_JOYSTICK);

        /* Add axis data */
        data.append (_num_axes);
        for (int axis = 0; axis < _num_axes; ++axis)
            data.append (joysticks()->at (i).axes [axis] * 127);

        /* Generate button data */
        int _button_data = 0;
        for (int button = 0; button < _num_buttons; ++button) {
            bool pressed = joysticks()->at (i).buttons [button];
            _button_data += pressed ? qPow (2, button) : 0;
        }

        /* Add button data */
        data.append (_num_buttons);
        data.append (DS::intToBytes (_button_data));

        /* Add hat/pov data */
        data.append (_num_pov_hats);
        for (int hat = 0; hat < _num_pov_hats; ++hat)
            data.append (DS::intToBytes (joysticks()->at (i).POVs [hat]));
    }

    return data;
}

//==================================================================================================
// FRC_Protocol2015::getTimezoneData
//==================================================================================================

QByteArray FRC_Protocol2015::getTimezoneData() {
    QByteArray data;

    /* Add size (always 11) */
    data.append (0x0B);

    /* Get current date/time */
    QDateTime dt = QDateTime::currentDateTime();
    QDate date = dt.date();
    QTime time = dt.time();

    /* Add current date/time */
    data.append (HEADER_TIME);
    data.append (DS::intToBytes (time.msec()));
    data.append (time.second());
    data.append (time.minute());
    data.append (time.hour());
    data.append (date.day());
    data.append (date.month());
    data.append (date.year() - 1900);

    /* Add timezone data */
    data.append (DS::timezone().length() + 1);
    data.append (HEADER_TIMEZONE);
    data.append (DS::timezone());

    return data;
}

//==================================================================================================
// FRC_Protocol2015::getFMSPacket
//==================================================================================================

QByteArray FRC_Protocol2015::generateFmsPacket() {
    QByteArray data;
    return data;
}

//==================================================================================================
// FRC_Protocol2015::getClientPacket
//==================================================================================================

QByteArray FRC_Protocol2015::generateRobotPacket() {
    QByteArray data;

    /* Used for rebooting the robot and restarting its code */
    quint8 instruction = isConnectedToRobot() ? m_instructionCode : INVALID;

    /* Defines operation mode, e-stop state & enabled state */
    quint8 opcode = (isEmergencyStopped() ? ESTOP_ON : ESTOP_OFF) |
                    (isEnabled() ? ENABLED : DISABLED) |
                    (getControlCode());

    /* Gets timezone data or joystick input */
    QByteArray extensions = sendDateTime() ? getTimezoneData() : getJoystickData();

    /* Construct the packet */
    data.append (DS::intToBytes (sentRobotPackets()));
    data.append (HEADER_GENERAL);
    data.append (opcode);
    data.append (instruction);
    data.append (getAllianceCode());
    data.append (extensions);

    return data;
}

//==================================================================================================
// FRC_Protocol2015::getControlCode
//==================================================================================================

int FRC_Protocol2015::getControlCode() {
    if (controlMode() == DS::kControlTest)
        return CONTROL_TEST;

    else if (controlMode() == DS::kControlAutonomous)
        return CONTROL_AUTONOMOUS;

    else if (controlMode() == DS::kControlTeleoperated)
        return CONTROL_TELEOPERATED;

    return CONTROL_TELEOPERATED;
}

//==================================================================================================
// FRC_Protocol2015::getAllianceCode
//==================================================================================================

int FRC_Protocol2015::getAllianceCode() {
    if (alliance() == DS::kAllianceRed1)
        return ALLIANCE_RED_1;

    else if (alliance() == DS::kAllianceRed2)
        return ALLIANCE_RED_2;

    else if (alliance() == DS::kAllianceRed3)
        return ALLIANCE_RED_3;

    else if (alliance() == DS::kAllianceBlue1)
        return ALLIANCE_BLUE_1;

    else if (alliance() == DS::kAllianceBlue2)
        return ALLIANCE_BLUE_2;

    else if (alliance() == DS::kAllianceBlue3)
        return ALLIANCE_BLUE_3;

    return ALLIANCE_RED_1;
}

//==================================================================================================
// FRC_Protocol2015::getJoystickSize
//==================================================================================================

int FRC_Protocol2015::getJoystickSize (DS::Joystick joystick) {
    return  5
            + (joystick.numAxes > 0 ? joystick.numAxes : 0)
            + (joystick.numButtons / 8)
            + (joystick.numButtons % 8 == 0 ? 0 : 1)
            + (joystick.numPOVs > 0 ? joystick.numPOVs * 2 : 0);
}
