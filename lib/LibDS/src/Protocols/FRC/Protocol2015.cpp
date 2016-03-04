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

#include "LibDS/Protocols/FRC/Protocol2015.h"

using namespace DS_Protocols;

//=============================================================================
// FTP locations
//=============================================================================

const QString PCM_PATH = "/tmp/frc_versions/PCM-0-versions.ini";
const QString PDP_PATH = "/tmp/frc_versions/PDP-0-versions.ini";
const QString LIB_PATH = "/tmp/frc_versions/FRC_Lib_Version.ini";

//=============================================================================
// Protocol codes/standards
//=============================================================================

enum ProtocolStandards {
    /* Client -> RoboRIO operation states */
    pEmStopOn                 = 0x80,
    pEmStopOff                = 0x00,
    pEnabled                  = 0x04,
    pDisabled                 = 0x00,
    pFMSAttached              = 0x01,

    /* Client -> RoboRIO operation modes */
    pControlTest              = 0x01,
    pControlAutonomous        = 0x02,
    pControlTeleoperated      = 0x00,

    /* Client -> RoboRIO section codes */
    pHeaderTime               = 0x0f,
    pHeaderGeneral            = 0x01,
    pHeaderJoystick           = 0x0c,
    pHeaderTimezone           = 0x10,

    /* Client -> RoboRIO special instructions */
    pInstructionNormal        = 0x10,
    pInstructionInvalid       = 0x00,
    pInstructionRebootRIO     = 0x14,
    pInstructionRestartCode   = 0x16,

    /* Client -> RoboRIO alliance & postion */
    pRed1                     = 0x00,
    pRed2                     = 0x01,
    pRed3                     = 0x02,
    pBlue1                    = 0x03,
    pBlue2                    = 0x04,
    pBlue3                    = 0x05,

    /* RoboRIO -> Client extra data */
    pRIOHeaderJoystickOutput  = 0x01,
    pRIOHeaderDiskInformation = 0x04,
    pRIOHeaderCPUInformation  = 0x05,
    pRIOHeaderRAMInformation  = 0x06,
    pRIOHeaderCANMetrics      = 0x0e,

    /* RoboRIO -> Client echo codes */
    pProgramTest              = 0x01,
    pProgramAutonomous        = 0x02,
    pProgramCodePresent       = 0x20,
    pProgramTeleoperated      = 0x00,
    pProgramRequestTime       = 0x01,
};

//=============================================================================
// FRC_Protocol2015::fmsFrequency
//=============================================================================

int FRC_Protocol2015::fmsFrequency() {
    return 2;
}

//=============================================================================
// FRC_Protocol2015::robotFrequency
//=============================================================================

int FRC_Protocol2015::robotFrequency() {
    return 50;
}

//=============================================================================
// FRC_Protocol2015::fmsInputPort
//=============================================================================

int FRC_Protocol2015::fmsInputPort() {
    return 1120;
}

//=============================================================================
// FRC_Protocol2015::fmsOutputPort
//=============================================================================

int FRC_Protocol2015::fmsOutputPort() {
    return 1160;
}

//=============================================================================
// FRC_Protocol2015::clientPort
//=============================================================================

int FRC_Protocol2015::robotInputPort() {
    return 1150;
}

//=============================================================================
// FRC_Protocol2015::robotPort
//=============================================================================

int FRC_Protocol2015::robotOutputPort() {
    return 1110;
}

//=============================================================================
// FRC_Protocol2015::tcpProbePort
//=============================================================================

int FRC_Protocol2015::tcpProbesPort() {
    return 80;
}

//=============================================================================
// FRC_Protocol2015::netConsoleInputPort
//=============================================================================

int FRC_Protocol2015::netConsoleInputPort() {
    return 6666;
}

//=============================================================================
// FRC_Protocol2015::acceptsConsoleCommnds
//=============================================================================

bool FRC_Protocol2015::acceptsConsoleCommands() {
    return false;
}

//=============================================================================
// FRC_Protocol2015::defaultRadioAddress
//=============================================================================

QStringList FRC_Protocol2015::defaultRadioAddress() {
    return QStringList (DS::getStaticIP (10, team(), 1));
}

//=============================================================================
// FRC_Protocol2015::defaultRobotAddress
//=============================================================================

QStringList FRC_Protocol2015::defaultRobotAddress() {
    QStringList list;

    list.append (QString ("roboRIO-%1.local").arg (team()));
    list.append (QString ("172.22.11.2"));
    list.append (QString ("127.0.0.1"));

    /* Try all the DHCP ranges */
    for (int i = 20; i < 100; ++i)
        list.append (QString (DS::getStaticIP (10, team(), i)));

    return list;
}

//=============================================================================
// FRC_Protocol2015::reboot
//=============================================================================

void FRC_Protocol2015::reboot() {
    m_instructionCode = pInstructionRebootRIO;
}

//=============================================================================
// FRC_Protocol2015::restartCode
//=============================================================================

void FRC_Protocol2015::restartCode() {
    m_instructionCode  = pInstructionRestartCode;
}

//=============================================================================
// FRC_Protocol2015::resetProtocol
//=============================================================================

void FRC_Protocol2015::_resetProtocol() {
    m_instructionCode = pInstructionInvalid;
}

//=============================================================================
// FRC_Protocol2015::readFMSPacket
//=============================================================================

bool FRC_Protocol2015::_readFMSPacket (QByteArray data) {
    Q_UNUSED (data);
    return true;
}

//=============================================================================
// FRC_Protocol2015::readPacket
//=============================================================================

bool FRC_Protocol2015::_readRobotPacket (QByteArray data) {
    int offset = 8;

    /* Packet length is invalid, watchdog will not be reset */
    if (data.length() < offset)
        return false;

    /* Be sure to reset status bit */
    if (m_instructionCode  == pInstructionInvalid)
        m_instructionCode = pInstructionNormal;

    /* Read robot packet */
    quint8 opcode       = data.at (3);
    quint8 status       = data.at (4);
    quint8 request      = data.at (7);
    quint8 majorVoltage = data.at (5);
    quint8 minorVoltage = data.at (6);

    /* Update e-stop information */
    setEmergencyStop (opcode == pEmStopOn);

    /* Update client information */
    updateVoltageBrownout (false);
    updateVoltage         (majorVoltage, minorVoltage);
    updateSendDateTime    (request == pProgramRequestTime);
    updateRobotCode       ((status & pProgramCodePresent) != 0);

    /* Packet contains additional data structures */
    if (data.length() > offset) {
        QByteArray extended;
        for (int i = offset; i < data.length() - offset - 1; i++)
            extended.append (data.at (i));

        int id = extended.at (1);

        /* Robot wants us to rumble a joystick */
        if (id == pRIOHeaderJoystickOutput) {
            // TODO
        }

        /* Packet contains roboRIO CPU info */
        else if (id == pRIOHeaderCPUInformation) {
            // TODO
        }

        /* Packet contains roboRIO RAM info */
        else if (id == pRIOHeaderRAMInformation) {
            int a = (quint8) extended.at (6);
            int b = (quint8) extended.at (7);
            emit ramUsageChanged (a + b);
        }

        /* Packet contains roboRIO disk info */
        else if (id == pRIOHeaderDiskInformation) {
            int a = (quint8) extended.at (6);
            int b = (quint8) extended.at (7);
            emit diskUsageChanged (a + b);
        }

        /* Packet contains CAN metrics data */
        else if (id == pRIOHeaderCANMetrics) {
            // TODO
        }
    }

    /* Packet was successfully read, reset watchdog */
    return true;
}

//=============================================================================
// FRC_Protocol2015::getFMSPacket
//============================================================================

QByteArray FRC_Protocol2015::_getFmsPacket() {
    QByteArray data;
    return data;
}

//=============================================================================
// FRC_Protocol2015::getClientPacket
//=============================================================================

QByteArray FRC_Protocol2015::_getClientPacket() {
    QByteArray data;

    /* Used for rebooting the robot and restarting its code */
    quint8 instruction = isConnectedToRobot() ? m_instructionCode :
                         pInstructionInvalid;

    /* Defines operation mode, e-stop state & enabled state */
    quint8 opcode = (isEmergencyStopped() ? pEmStopOn : pEmStopOff) |
                    (isEnabled() ? pEnabled : pDisabled) |
                    (getControlCode());

    /* Gets timezone data or joystick input */
    QByteArray extensions = sendDateTime() ? _getTimezoneData() :
                            _getJoystickData();

    /* Construct the packet */
    data.append (DS::intToBytes (sentRobotPackets()));
    data.append (pHeaderGeneral);
    data.append (opcode);
    data.append (instruction);
    data.append (getAllianceCode());
    data.append (extensions);

    return data;
}

//=============================================================================
// FRC_Protocol2015::getJoystickData
//=============================================================================

QByteArray FRC_Protocol2015::_getJoystickData() {
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
        data.append (pHeaderJoystick);

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

//=============================================================================
// FRC_Protocol2015::getTimezoneData
//=============================================================================

QByteArray FRC_Protocol2015::_getTimezoneData() {
    QByteArray data;

    /* Add size (always 11) */
    data.append (0x0B);

    /* Get current date/time */
    QDateTime dt = QDateTime::currentDateTime();
    QDate date = dt.date();
    QTime time = dt.time();

    /* Add current date/time */
    data.append (pHeaderTime);
    data.append (DS::intToBytes (time.msec()));
    data.append (time.second());
    data.append (time.minute());
    data.append (time.hour());
    data.append (date.day());
    data.append (date.month());
    data.append (date.year() - 1900);

    /* Add timezone data */
    data.append (DS::Timezone().length() + 1);
    data.append (pHeaderTimezone);
    data.append (DS::Timezone());

    return data;
}

//=============================================================================
// FRC_Protocol2015::getControlCode
//=============================================================================

int FRC_Protocol2015::getControlCode() {
    if (controlMode() == DS::kControlTest)
        return pControlTest;

    else if (controlMode() == DS::kControlAutonomous)
        return pControlAutonomous;

    else if (controlMode() == DS::kControlTeleoperated)
        return pControlTeleoperated;

    return pControlTeleoperated;
}

//=============================================================================
// FRC_Protocol2015::getAllianceCode
//=============================================================================

int FRC_Protocol2015::getAllianceCode() {
    if (alliance() == DS::kAllianceRed1)
        return pRed1;

    else if (alliance() == DS::kAllianceRed2)
        return pRed2;

    else if (alliance() == DS::kAllianceRed3)
        return pRed3;

    else if (alliance() == DS::kAllianceBlue1)
        return pBlue1;

    else if (alliance() == DS::kAllianceBlue2)
        return pBlue2;

    else if (alliance() == DS::kAllianceBlue3)
        return pBlue3;

    return pRed1;
}

//=============================================================================
// FRC_Protocol2015::getJoystickSize
//=============================================================================

int FRC_Protocol2015::getJoystickSize (DS::Joystick joystick) {
    return  5
            + (joystick.numAxes > 0 ? joystick.numAxes : 0)
            + (joystick.numButtons / 8)
            + (joystick.numButtons % 8 == 0 ? 0 : 1)
            + (joystick.numPOVs > 0 ? joystick.numPOVs * 2 : 0);
}
