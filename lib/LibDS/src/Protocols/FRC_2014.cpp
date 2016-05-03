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
#define TELEOP_BIT         0x00 // DS changes robot mode to teleoperated
#define AUTONOMOUS_BIT     0x10 // DS changes robot mode to autonomous
#define TEST_BIT           0x02 // DS changes robot mode to test
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
    m_resync = true;
    m_reboot = false;
    m_restartCode = false;
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
    m_resync = true;
    m_reboot = false;
    m_restartCode = false;
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
    /* The packet is smaller than it should be */
    if (data.length() < 1024)
        return false;

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
    if (opcode == ESTOP_ON_BIT && !isEmergencyStopped())
        setEmergencyStop (true);

    /* If both battery voltage values are 0x37, it means that there is no code loaded */
    updateRobotCode ((integer != 0x37) && (decimal != 0x37));

    /* Update the voltage values */
    updateVoltage (hasCode() ? voltage.toFloat() : 0);

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

//==================================================================================================
// FRC_Protocol2014::getJoystickData
//==================================================================================================

QByteArray FRC_Protocol2014::getJoystickData() {
    QByteArray data;

    /* The protocol must define data for 4 joysticks (even if they are not connected) */
    for (int i = 0; i < 4; ++i) {

        /* If set to false, the code will generate neutral button and axis values */
        bool joystick_exists = joysticks()->count() > i;

        /* Get number of axes and buttons of the current joystick */
        int num_axes = joystick_exists ? joysticks()->at (i).numAxes : 0;
        int num_buttons = joystick_exists ? joysticks()->at (i).numButtons : 0;

        /* Generate axis data (only 6 axes, no more, no less) */
        for (int axis = 0; axis < 6; ++axis) {
            /* The joystick exists and the axis also exists */
            if (joystick_exists && axis < num_axes)
                data.append (joysticks()->at (i).axes[axis] * 127);

            /* Append neutral data, since the axis is not real */
            else
                data.append ((char) 0x00);
        }

        /* Generate button data */
        int button_data = 0;
        for (int button = 0; button < num_buttons; ++button) {
            /* Joystick exists and button is pressed */
            if (joystick_exists && joysticks()->at (i).buttons[button])
                button_data |= (int) qPow (2, button);
        }

        /* Append the button data in two bytes */
        data.append ((button_data & 0xff00) >> 8);
        data.append ((button_data & 0xff));
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
    quint8 enabled = isEnabled() ? ENABLED_BIT : 0x00;

    switch (controlMode()) {
    case DS::kControlAutonomous:
        code |= enabled + AUTONOMOUS_BIT;
        break;
    case DS::kControlTest:
        code |= enabled + TEST_BIT;
        break;
    case DS::kControlTeleoperated:
        code |= enabled + TELEOP_BIT;
        break;
    default:
        code = ESTOP_OFF_BIT;
        break;
    }

    if (m_resync)
        code |= RESYNC_BIT;

    if (isFmsAttached())
        code |= FMS_ATTACHED_BIT;

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
