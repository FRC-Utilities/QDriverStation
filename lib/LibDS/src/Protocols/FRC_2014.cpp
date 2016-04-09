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

#define DS_TO_CRIO_CONTROL_TEST           0x62
#define DS_TO_CRIO_CONTROL_TELEOP         0x60
#define DS_TO_CRIO_CONTROL_REBOOT         0x80
#define DS_TO_CRIO_CONTROL_AUTONOMOUS     0x70
#define DS_TO_CRIO_CONTROL_DISABLED       0x40
#define DS_TO_CRIO_CONTROL_EMERGENCY_STOP 0x00
#define DS_TO_CRIO_ALLIANCE_RED           0x52
#define DS_TO_CRIO_ALLIANCE_BLUE          0x52
#define DS_TO_CRIO_POSITION_1             0x01
#define DS_TO_CRIO_POSITION_2             0x02
#define DS_TO_CRIO_POSITION_3             0x03

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
    return 1110;
}

//==================================================================================================
// FRC_Protocol2014::robotPort
//==================================================================================================

int FRC_Protocol2014::robotOutputPort() {
    return 1150;
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
// FRC_Protocol2014::readFMSPacket
//==================================================================================================

bool FRC_Protocol2014::interpretFmsPacket (QByteArray data) {
    Q_UNUSED (data);
    return true;
}

//==================================================================================================
// FRC_Protocol2014::readPacket
//==================================================================================================

bool FRC_Protocol2014::interpretRobotPacket (QByteArray data) {
    Q_UNUSED (data);
    return true;
}

//==================================================================================================
// FRC_Protocol2014::getFMSPacket
//==================================================================================================

QByteArray FRC_Protocol2014::generateFmsPacket() {
    QByteArray data;
    return data;
}

//==================================================================================================
// FRC_Protocol2014::getClientPacket
//==================================================================================================

QByteArray FRC_Protocol2014::generateRobotPacket() {
    QByteArray data;

    data.resize (1024);
    data.fill   (0x00);

    data.append (DS::intToBytes (sentRobotPackets()));    // Packet index
    data.append (getOperationCode());                     // Operation mode & instructions
    data.append (getDigitalInput());                      // DIO, currently blank
    data.append (team());                                 // Team number
    data.append (getAlliance());                          // Current team alliance
    data.append (getPosition());                          // Current team station
    data.append (getJoystickData());                     // Joystick data, not tested
    data.append (getAnalogData());                        // Analog IO, currently blank
    data.append (qChecksum (data.data(), data.length())); // CRC checksum

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

        /* Add axis data (axis offset) + (joystick offset) */
        for (int axis = 0; axis < _num_axes; ++axis)
            data.append (joysticks()->at (i).axes [axis] * 127);

        /* Generate button data */
        int _button_data = 0;
        for (int button = 0; button < _num_buttons; ++button) {
            bool pressed = joysticks()->at (i).buttons [button];
            _button_data += pressed ? qPow (2, button) : 0;
        }

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
        return DS_TO_CRIO_ALLIANCE_BLUE;

    return DS_TO_CRIO_ALLIANCE_RED;
}

//==================================================================================================
// FRC_Protocol2014::getPosition
//==================================================================================================

quint8 FRC_Protocol2014::getPosition() {
    /* Position 1 */
    if (alliance() == DS::kAllianceRed1 || alliance() == DS::kAllianceBlue1)
        return DS_TO_CRIO_POSITION_1;

    /* Position 2 */
    if (alliance() == DS::kAllianceRed2 || alliance() == DS::kAllianceBlue2)
        return DS_TO_CRIO_POSITION_2;

    /* Position 3 */
    if (alliance() == DS::kAllianceRed3 || alliance() == DS::kAllianceBlue3)
        return DS_TO_CRIO_POSITION_3;

    /* Default to position 1 */
    return DS_TO_CRIO_POSITION_1;
}

//==================================================================================================
// FRC_Protocol2014::getOperationCode
//==================================================================================================

quint8 FRC_Protocol2014::getOperationCode() {
    quint8 _op_mode = DS_TO_CRIO_CONTROL_DISABLED;

    /* Send e-stop code */
    if (isEmergencyStopped())
        _op_mode = DS_TO_CRIO_CONTROL_EMERGENCY_STOP;

    /* Send reboot request */
    else if (m_reboot)
        _op_mode = DS_TO_CRIO_CONTROL_REBOOT;

    /* 'Normal' operation modes */
    else if (isEnabled()) {
        switch (controlMode()) {
        case DS::kControlAutonomous:
            _op_mode = DS_TO_CRIO_CONTROL_AUTONOMOUS;
            break;
        case DS::kControlTeleoperated:
            _op_mode = DS_TO_CRIO_CONTROL_TELEOP;
            break;
        case DS::kControlTest:
            _op_mode = DS_TO_CRIO_CONTROL_TEST;
            break;
        default:
            _op_mode = DS_TO_CRIO_CONTROL_DISABLED;
            break;
        }
    }

    return _op_mode;
}

//==================================================================================================
// FRC_Protocol2014::getAnalogData
//==================================================================================================

QByteArray FRC_Protocol2014::getAnalogData() {
    QByteArray data;
    data.append (DS::intToBytes (0x00));
    data.append (DS::intToBytes (0x00));
    data.append (DS::intToBytes (0x00));
    data.append (DS::intToBytes (0x00));
    return data;
}

//==================================================================================================
// FRC_Protocol2014::getDigitalData
//==================================================================================================

QByteArray FRC_Protocol2014::getDigitalInput() {
    QByteArray data;
    data.append (0xFF);
    return data;
}
