/*
 * Copyright (c) 2014 WinT 3794 <http://wint3794.org>
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

#include "LibDS/Protocols/FRC_2014.h"

using namespace DS_Protocols;

//==================================================================================================
// Protocol bytes
//==================================================================================================

#define ESTOP_ON_BIT       0x00 // DS triggers the emergency stop on the robot
#define ESTOP_OFF_BIT      0x40 // DS triggers the emergency stop on the robot
#define REBOOT_BIT         0x80 // DS triggers a reboot of the cRIO
#define ENABLED_BIT        0x20 // DS enables the robot
#define RESYNC_BIT         0x04 // DS re-syncs comms with robot?
#define TELEOP_BIT         0x20 // DS changes robot mode to teleoperated
#define AUTONOMOUS_BIT     0x30 // DS changes robot mode to autonomous
#define TEST_BIT           0x22 // DS changes robot mode to test
#define FMS_ATTACHED_BIT   0x08 // DS sends this when it is connected to the FMS
#define ALLIANCE_RED_BIT   0x52 // DS changes robot alliance to red
#define ALLIANCE_BLUE_BIT  0x42 // DS changes robot alliance to blue
#define POSITION_1_BIT     0x31 // DS reports team station 1
#define POSITION_2_BIT     0x32 // DS reports team station 2
#define POSITION_3_BIT     0x33 // DS reports team station 3

//==================================================================================================
// FRC_Protocol2014::FRC_Protocol2014
//==================================================================================================

FRC_Protocol2014::FRC_Protocol2014() {
    m_reboot = false;
    m_restartCode = false;
    QTimer::singleShot (1000, Qt::CoarseTimer, this, SLOT (showProtocolWarning()));
}

//==================================================================================================
// FRC_Protocol2014::name
//==================================================================================================

QString FRC_Protocol2014::name() {
    return "FRC 2014 Protocol (Alpha)";
}

//==================================================================================================
// FRC_Protocol2014::fmsFrequency
//==================================================================================================

int FRC_Protocol2014::fmsFrequency() {
    return 10;
}

//==================================================================================================
// FRC_Protocol2014::robotFrequency
//==================================================================================================

int FRC_Protocol2014::robotFrequency() {
    return 50;
}

//==================================================================================================
// FRC_Protocol2014::fmsInputPort
//==================================================================================================

int FRC_Protocol2014::fmsInputPort() {
    return 1120;
}

//==================================================================================================
// FRC_Protocol2014::fmsOutputPort
//==================================================================================================

int FRC_Protocol2014::fmsOutputPort() {
    return 1160;
}

//==================================================================================================
// FRC_Protocol2014::clientPort
//==================================================================================================

int FRC_Protocol2014::robotInputPort() {
    return 1150;
}

//==================================================================================================
// FRC_Protocol2014::robotPort
//==================================================================================================

int FRC_Protocol2014::robotOutputPort() {
    return 1110;
}

//==================================================================================================
// FRC_Protocol2014::tcpProbePort
//==================================================================================================

int FRC_Protocol2014::tcpProbesPort() {
    return 80;
}

//==================================================================================================
// FRC_Protocol2014::netConsoleInputPort
//==================================================================================================

int FRC_Protocol2014::netConsoleInputPort() {
    return 6666;
}

//==================================================================================================
// FRC_Protocol2014::netConsoleOutputPort
//==================================================================================================

int FRC_Protocol2014::netConsoleOutputPort() {
    return 6668;
}

//==================================================================================================
// FRC_Protocol2014::acceptsConsoleCommnds
//==================================================================================================

bool FRC_Protocol2014::acceptsConsoleCommands() {
    return true;
}

//==================================================================================================
// FRC_Protocol2014::reboot
//==================================================================================================

void FRC_Protocol2014::reboot() {
    m_reboot = true;
}

//==================================================================================================
// FRC_Protocol2014::restartCode
//==================================================================================================

void FRC_Protocol2014::restartCode() {
    m_restartCode = true;
}

//==================================================================================================
// FRC_Protocol2014::resetProtocol
//==================================================================================================

void FRC_Protocol2014::resetProtocol() {
    m_reboot = false;
    m_restartCode = false;
}

//==================================================================================================
// FRC_Protocol2014::showProtocolWarning
//==================================================================================================

void FRC_Protocol2014::showProtocolWarning() {
    DS::sendMessage ("<p><b><font color=#FF7722>WARNING: </font></b>"
                     "<font color=#FFFFFF>"
                     "This protocol is under heavy development and you WILL "
                     "encounter bugs. If using a real robot, limit its area of "
                     "movement by placing it over a box or something. "
                     "<b>Safety is your number one priority!</b></font></p>");
}

//==================================================================================================
// FRC_Protocol2014::interpretFMSPacket
//==================================================================================================

bool FRC_Protocol2014::interpretFmsPacket (QByteArray data) {
    Q_UNUSED (data);
    return true;
}

//==================================================================================================
// FRC_Protocol2014::interpretPacket
//==================================================================================================

bool FRC_Protocol2014::interpretRobotPacket (QByteArray data) {
    Q_UNUSED (data);
    return true;
}

//==================================================================================================
// FRC_Protocol2014::generateFmsPacket
//==================================================================================================

QByteArray FRC_Protocol2014::generateFmsPacket() {
    QByteArray data;
    return data;
}

//==================================================================================================
// FRC_Protocol2014::generateRobotPacket
//==================================================================================================

QByteArray FRC_Protocol2014::generateRobotPacket() {
    QByteArray data;

    /* Setup the base packet */
    data.resize (1024);
    data.fill   (0x00);

    /* Add packet index */
    data[0] = (sentRobotPackets() & 0xff00) >> 8;
    data[1] = (sentRobotPackets() & 0xff);

    /* Add team number */
    data[4] = (team() & 0xff00) >> 8;
    data[5] = (team() & 0xff);

    /* Add operation code, empty digital input and alliance & position */
    data[2] = getOperationCode();
    data[3] = getDigitalInput();
    data[6] = getAlliance();
    data[7] = getPosition();

    /* Add joystick data */
    QByteArray joysticks = getJoystickData();
    data.replace (8, joysticks.length(), joysticks);

    /* Add FRC Driver Station version */
    data[72] = (quint8) 0x30;
    data[73] = (quint8) 0x31;
    data[74] = (quint8) 0x30;
    data[75] = (quint8) 0x34;
    data[76] = (quint8) 0x31;
    data[77] = (quint8) 0x34;
    data[78] = (quint8) 0x30;
    data[79] = (quint8) 0x30;

    /* Add CRC checksum */
    quint16 checksum = qChecksum (data, 32);
    data[1020] = (checksum & 0xff000000) >> 24;
    data[1021] = (checksum & 0xff0000) >> 16;
    data[1022] = (checksum & 0xff00) >> 8;
    data[1023] = (checksum & 0xff);

    return data;
}

//==================================================================================================
// FRC_Protocol2014::getJoystickData
//==================================================================================================

QByteArray FRC_Protocol2014::getJoystickData() {
    QByteArray data;

    /* Do not send joystick data on DS init */
    if (sentRobotPackets() <= 5)
        return data;

    /* Generate data for each joystick */
    for (int i = 0; i < joysticks()->count(); ++i) {
        int _num_axes    = joysticks()->at (i).numAxes;
        int _num_buttons = joysticks()->at (i).numButtons;

        /* Add axis data */
        for (int axis = 0; axis < _num_axes; ++axis)
            data.append (joysticks()->at (i).axes [axis]);

        /* Generate button data */
        int _button_data = 0;
        for (int button = 0; button < _num_buttons; ++button)
            _button_data += joysticks()->at (i).buttons [button] ? qPow (2, button) : 0;

        /* Add button data */
        data.append (DS::intToBytes (_button_data));
    }

    return data;
}

//==================================================================================================
// FRC_Protocol2014::getAlliance
//==================================================================================================

quint8 FRC_Protocol2014::getAlliance() {
    if (alliance() == DS::kAllianceBlue1 ||
            alliance() == DS::kAllianceBlue2 ||
            alliance() == DS::kAllianceBlue3)
        return ALLIANCE_BLUE_BIT;

    return ALLIANCE_RED_BIT;
}

//==================================================================================================
// FRC_Protocol2014::getPosition
//==================================================================================================

quint8 FRC_Protocol2014::getPosition() {
    /* Position 1 */
    if (alliance() == DS::kAllianceRed1 || alliance() == DS::kAllianceBlue1)
        return POSITION_1_BIT;

    /* Position 2 */
    if (alliance() == DS::kAllianceRed2 || alliance() == DS::kAllianceBlue2)
        return POSITION_2_BIT;

    /* Position 3 */
    if (alliance() == DS::kAllianceRed3 || alliance() == DS::kAllianceBlue3)
        return POSITION_3_BIT;

    /* Default to position 1 */
    return POSITION_1_BIT;
}

//==================================================================================================
// FRC_Protocol2014::getOperationCode
//==================================================================================================

quint8 FRC_Protocol2014::getOperationCode() {
    quint8 code = ESTOP_OFF_BIT;

    if (isEnabled()) {
        switch (controlMode()) {
        case DS::kControlAutonomous:
            code |= AUTONOMOUS_BIT;
            break;
        case DS::kControlTest:
            code |= TEST_BIT;
            break;
        case DS::kControlTeleoperated:
            code |= TELEOP_BIT;
            break;
        default:
            code = ESTOP_OFF_BIT;
            break;
        }
    }

    if (isEmergencyStopped())
        code = ESTOP_ON_BIT;

    if (m_reboot)
        code = REBOOT_BIT;

    return code;
}

//==================================================================================================
// FRC_Protocol2014::getDigitalInput
//==================================================================================================

quint8 FRC_Protocol2014::getDigitalInput() {
    return 0x00;
}
