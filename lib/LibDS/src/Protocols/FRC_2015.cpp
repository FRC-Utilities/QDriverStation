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
// Protocol bytes
//==================================================================================================

/* Robot mode codes */
const uint OP_MODE_TEST         = 0x01; // 0000 0001
const uint OP_MODE_AUTONOMOUS   = 0x02; // 0000 0010
const uint OP_MODE_TELEOPERATED = 0x00; // 0000 0000
const uint OP_MODE_ESTOPPED     = 0x80; // 1000 0000
const uint OP_MODE_ENABLED      = 0x04; // 0000 0100

/* Status & request flags */
const uint DS_FMS_ATTACHED      = 0x08; // 0000 1000
const uint DS_REQUEST_NORMAL    = 0x80; // 1000 0000
const uint DS_REQUEST_UNKNOWN   = 0x00; // 0000 0000
const uint DS_REQUEST_REBOOT    = 0x08; // 0000 1000
const uint DS_REQUEST_KILL_CODE = 0x04; // 0000 0100

/* RIO-to-DS control flags */
const uint R_CTRL_HAS_CODE      = 0x20; // 0010 0000
const uint R_CTRL_BROWNOUT      = 0x10; // 0001 0000

/* DS-to-FMS flags */
const uint DS_FMS_COMM_VERSION  = 0x00; // 0000 0000
const uint DS_FMS_ROBOT_COMMS   = 0x20; // 0010 0000
const uint DS_FMS_RADIO_PING    = 0x10; // 0001 0000
const uint DS_FMS_ROBOT_PING    = 0x08; // 0000 1000

/* DS-to-RIO data tags */
const uint DS_TAG_DATE          = 0x0f; // 0000 1111
const uint DS_TAG_GENERAL       = 0x01; // 0000 0001
const uint DS_TAG_JOYSTICK      = 0x0c; // 0000 1100
const uint DS_TAG_TIMEZONE      = 0x10; // 0001 0000

/* Alliances & positions */
const uint STATION_RED_1        = 0x00; // 0000 0000
const uint STATION_RED_2        = 0x01; // 0000 0001
const uint STATION_RED_3        = 0x02; // 0000 0010
const uint STATION_BLUE_1       = 0x03; // 0000 0011
const uint STATION_BLUE_2       = 0x04; // 0000 0100
const uint STATION_BLUE_3       = 0x05; // 0000 0101

/* RIO-to-DS data tags */
const uint R_TAG_JOYSTICK_OUT   = 0x01; // 0000 0001
const uint R_TAG_DISK_INFO      = 0x04; // 0000 0100
const uint R_TAG_CPU_INFO       = 0x05; // 0000 0101
const uint R_TAG_RAM_INFO       = 0x06; // 0000 0110
const uint R_TAG_CAN_METRICS    = 0x0e; // 0000 1110
const uint R_REQUEST_TIME       = 0x01; // 0000 0001

//==================================================================================================
// FRC_Protocol2015::reboot
//==================================================================================================

void FRC_Protocol2015::reboot() {
    m_rebootRio = true;
    QTimer::singleShot (200, Qt::PreciseTimer, this, SLOT (resetProtocol()));
}

//==================================================================================================
// FRC_Protocol2015::restartCode
//==================================================================================================

void FRC_Protocol2015::restartCode() {
    m_restartCode = true;
    QTimer::singleShot (200, Qt::PreciseTimer, this, SLOT (resetProtocol()));
}

//==================================================================================================
// FRC_Protocol2015::resetProtocol
//==================================================================================================

void FRC_Protocol2015::resetProtocol() {
    m_rebootRio = false;
    m_restartCode = false;
}

//==================================================================================================
// FRC_Protocol2015::readFMSPacket
//==================================================================================================

bool FRC_Protocol2015::interpretFmsPacket (QByteArray data) {
    if (data.length() >= 22) {
        quint8 control  = data.at (3);
        quint8 alliance = data.at (5);

        /* Change robot enabled state based on what FMS tells us to do*/
        setEnabled (control & OP_MODE_ENABLED);

        /* Get FMS robot mode */
        DS::ControlMode mode = DS::kControlInvalid;
        if (control & OP_MODE_TELEOPERATED)
            mode = DS::kControlTeleoperated;
        else if (control & OP_MODE_AUTONOMOUS)
            mode = DS::kControlAutonomous;
        else if (control & OP_MODE_TEST)
            mode = DS::kControlTest;

        /* Update robot mode */
        if (mode != controlMode())
            setControlMode (mode);

        /* Update to correct alliance and position */
        if (alliance != getAllianceCode())
            setAlliance (getAllianceStation (alliance));

        return true;
    }

    return false;
}

//==================================================================================================
// FRC_Protocol2015::readPacket
//==================================================================================================

bool FRC_Protocol2015::interpretRobotPacket (QByteArray data) {
    /* Packet is invalid */
    if (data.length() < 8)
        return false;

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
    bool send_date_time = (request == R_REQUEST_TIME);

    /* Update client information */
    updateRobotCode    (has_code);
    updateVBrownout    (voltage_brwn);
    updateSendDateTime (send_date_time);

    /* Update emergency stop state */
    if (e_stopped && !isEmergencyStopped())
        setEmergencyStop (true);
    else if (!e_stopped && isEmergencyStopped())
        setEmergencyStop (false);

    /* Calculate the voltage */
    float voltage = integer + (99 * decimal / 255);
    updateVoltage (voltage);

    /* Read extra data tags */
    if (data.length() > 8) {
        switch (data.at (8)) {
        case R_TAG_JOYSTICK_OUT:
            break;
        case R_TAG_DISK_INFO:
            break;
        case R_TAG_CPU_INFO:
            break;
        case R_TAG_RAM_INFO:
            break;
        case R_TAG_CAN_METRICS:
            break;
        default:
            break;
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
        int numAxes    = joysticks()->at (i).numAxes;
        int numPOVs    = joysticks()->at (i).numPOVs;
        int numButtons = joysticks()->at (i).numButtons;

        /* Add joystick information and put the section header */
        data.append (getJoystickSize (joysticks()->at (i)) - 1);
        data.append (DS_TAG_JOYSTICK);

        /* Add axis data */
        data.append (numAxes);
        for (int axis = 0; axis < numAxes; ++axis)
            data.append (joysticks()->at (i).axes [axis] * 127);

        /* Generate button data */
        int buttonData = 0;
        for (int button = 0; button < numButtons; ++button)
            buttonData += joysticks()->at (i).buttons [button] ? qPow (2, button) : 0;

        /* Add button data */
        data.append (numButtons);
        data.append (DS::intToBytes (buttonData));

        /* Add hat/pov data */
        data.append (numPOVs);
        for (int hat = 0; hat < numPOVs; ++hat)
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
    data.append (DS_TAG_DATE);
    data.append (DS::intToBytes (time.msec()));
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

//==================================================================================================
// FRC_Protocol2015::generateFmsPacket
//==================================================================================================

QByteArray FRC_Protocol2015::generateFmsPacket() {
    QByteArray data;
    data.append (DS::intToBytes (sentFmsPackets()));
    data.append (DS_FMS_COMM_VERSION);
    data.append (getFmsControlCode());
    data.append (DS::intToBytes (team()));
    data.append (DS::floatToBytes (robotVoltage()));
    return data;
}

//==================================================================================================
// FRC_Protocol2015::generateRobotPacket
//==================================================================================================

QByteArray FRC_Protocol2015::generateRobotPacket() {
    QByteArray data;
    data.append (DS::intToBytes (sentRobotPackets()));
    data.append (DS_TAG_GENERAL);
    data.append (getControlCode());
    data.append (getRequestCode());
    data.append (getAllianceCode());
    data.append (sendDateTime() ? getTimezoneData() : getJoystickData());
    return data;
}

//==================================================================================================
// FRC_Protocol2015::getControlCode
//==================================================================================================

uint FRC_Protocol2015::getControlCode() {
    uint code = 0;

    switch (controlMode()) {
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

    if (isFmsAttached())
        code |= DS_FMS_ATTACHED;

    if (isEmergencyStopped())
        code |= OP_MODE_ESTOPPED;

    if (isEnabled())
        code |= OP_MODE_ENABLED;

    return code;
}

//==================================================================================================
// FRC_Protocol2015::getRequestCode
//==================================================================================================

uint FRC_Protocol2015::getRequestCode() {
    uint code = DS_REQUEST_UNKNOWN;

    if (isConnectedToRobot())
        code = DS_REQUEST_NORMAL;

    if (isConnectedToRobot() && m_rebootRio)
        code |= DS_REQUEST_REBOOT;

    if (isConnectedToRobot() && m_restartCode)
        code |= DS_REQUEST_KILL_CODE;

    return code;
}

//==================================================================================================
// FRC_Protocol2015::getAllianceCode
//==================================================================================================

uint FRC_Protocol2015::getAllianceCode() {
    switch (alliance()) {
    case DS::kAllianceRed1:
        return STATION_RED_1;
        break;
    case DS::kAllianceRed2:
        return STATION_RED_2;
        break;
    case DS::kAllianceRed3:
        return STATION_RED_3;
        break;
    case DS::kAllianceBlue1:
        return STATION_BLUE_1;
        break;
    case DS::kAllianceBlue2:
        return STATION_BLUE_2;
        break;
    case DS::kAllianceBlue3:
        return STATION_BLUE_3;
        break;
    default:
        return STATION_RED_1;
        break;
    }
}

//==================================================================================================
// FRC_Protocol2015::getFmsControlCode
//==================================================================================================

uint FRC_Protocol2015::getFmsControlCode() {
    uint code = 0;

    switch (controlMode()) {
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

    if (isEmergencyStopped())
        code |= OP_MODE_ESTOPPED;

    if (isEnabled())
        code |= OP_MODE_ENABLED;

    if (isConnectedToRadio())
        code |= DS_FMS_RADIO_PING;

    if (isConnectedToRobot()) {
        code |= DS_FMS_ROBOT_COMMS;
        code |= DS_FMS_ROBOT_PING;
    }

    return code;
}

//==================================================================================================
// FRC_Protocol2015::getJoystickSize
//==================================================================================================

uint FRC_Protocol2015::getJoystickSize (DS::Joystick joystick) {
    return  5
            + (joystick.numAxes > 0 ? joystick.numAxes : 0)
            + (joystick.numButtons / 8)
            + (joystick.numButtons % 8 == 0 ? 0 : 1)
            + (joystick.numPOVs > 0 ? joystick.numPOVs * 2 : 0);
}


//==================================================================================================
// FRC_Protocol2015::getAllianceStation
//==================================================================================================

DS::Alliance FRC_Protocol2015::getAllianceStation (quint8 code) {
    switch (code) {
    case STATION_RED_1:
        return DS::kAllianceRed1;
        break;
    case STATION_RED_2:
        return DS::kAllianceRed2;
        break;
    case STATION_RED_3:
        return DS::kAllianceRed3;
        break;
    case STATION_BLUE_1:
        return DS::kAllianceBlue1;
        break;
    case STATION_BLUE_2:
        return DS::kAllianceBlue2;
        break;
    case STATION_BLUE_3:
        return DS::kAllianceBlue3;
        break;
    default:
        return DS::kAllianceRed1;
        break;
    }
}
