/*
 * Copyright (c) 2015 WinT 3794 <http://wint3794.org>
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

#include "LibDS/Protocols/Protocol2015.h"

//=============================================================================
// FTP locations
//=============================================================================

const QString PCM_PATH = "/tmp/frc_versions/PCM-0-versions.ini";
const QString PDP_PATH = "/tmp/frc_versions/PDP-0-versions.ini";
const QString LIB_PATH = "/tmp/frc_versions/FRC_Lib_Version.ini";

//=============================================================================
// Protocol codes/standards
//=============================================================================

enum ProtocolStandards
{
    /* Client -> RoboRIO operation states */
    pEmStopOn                 = 0x80, /**< Emergency stop enabled */
    pEmStopOff                = 0x00, /**< Emergency stop disabled */
    pEnabled                  = 0x04, /**< Robot Enabled */
    pDisabled                 = 0x00, /**< Robot Disabled */

    /* Client -> RoboRIO operation modes */
    pControlTest              = 0x01, /**< Individual actuators can be moved */
    pControlAutonomous        = 0x02, /**< Robot takes over the world */
    pControlTeleoperated      = 0x00, /**< User moves the robot */

    /* Client -> RoboRIO section codes */
    pHeaderTime               = 0x0f, /**< Indicates start of current time data */
    pHeaderGeneral            = 0x01, /**< Indicates a client packet */
    pHeaderJoystick           = 0x0c, /**< Indicates start of JS section */
    pHeaderTimezone           = 0x10, /**< Indicates start of Timezone data */

    /* Client -> RoboRIO special instructions */
    pInstructionNormal        = 0x10, /**< Normal operation */
    pInstructionInvalid       = 0x00, /**< No communication */
    pInstructionRebootRIO     = 0x14, /**< Reboot the roboRIO */
    pInstructionRestartCode   = 0x16, /**< Kill the user code */

    /* Client -> RoboRIO alliance & postion */
    pRed1                     = 0x00, /**< Red alliance, position 1  */
    pRed2                     = 0x01, /**< Red alliance, position 2  */
    pRed3                     = 0x02, /**< Red alliance, position 3  */
    pBlue1                    = 0x03, /**< Blue alliance, position 1 */
    pBlue2                    = 0x04, /**< Blue alliance, position 2 */
    pBlue3                    = 0x05, /**< Blue alliance, position 3 */

    /* RoboRIO -> Client extra data */
    pRIOHeaderJoystickOutput  = 0x01, /**< Asks us to rumble joysticks */
    pRIOHeaderDiskInformation = 0x04, /**< Gives us roboRIO disk metrics */
    pRIOHeaderCPUInformation  = 0x05, /**< Gives us roboRIO CPU metrics */
    pRIOHeaderRAMInformation  = 0x06, /**< Gives us roboRIO RAM metrics */
    pRIOHeaderCANMetrics      = 0x0e, /**< Gives us robot CAN net metrics */

    /* RoboRIO -> Client echo codes */
    pProgramTest              = 0x01, /**< Control mode confirmation code */
    pProgramAutonomous        = 0x02, /**< Control mode confirmation code */
    pProgramCodePresent       = 0x20, /**< Not very elegant, but it works */
    pProgramTeleoperated      = 0x00, /**< Control mode confirmation code */
    pProgramRequestTime       = 0x01, /**< Robot wants to get the date/time */
};

//=============================================================================
// DS_Protocol2015::fmsFrequency
//=============================================================================

int DS_Protocol2015::fmsFrequency()
{
    return 2;
}

//=============================================================================
// DS_Protocol2015::robotFrequency
//=============================================================================

int DS_Protocol2015::robotFrequency()
{
    return 50;
}

//=============================================================================
// DS_Protocol2015::fmsInputPort
//=============================================================================

int DS_Protocol2015::fmsInputPort()
{
    return 1120;
}

//=============================================================================
// DS_Protocol2015::fmsOutputPort
//=============================================================================

int DS_Protocol2015::fmsOutputPort()
{
    return 1160;
}

//=============================================================================
// DS_Protocol2015::clientPort
//=============================================================================

int DS_Protocol2015::robotInputPort()
{
    return 1150;
}

//=============================================================================
// DS_Protocol2015::robotPort
//=============================================================================

int DS_Protocol2015::robotOutputPort()
{
    return 1110;
}

//=============================================================================
// DS_Protocol2015::tcpProbePort
//=============================================================================

int DS_Protocol2015::tcpProbesPort()
{
    return 80;
}

//=============================================================================
// DS_Protocol2015::netConsoleInputPort
//=============================================================================

int DS_Protocol2015::netConsoleInputPort()
{
    return 6666;
}

//=============================================================================
// DS_Protocol2015::netConsoleOutputPort
//=============================================================================

int DS_Protocol2015::netConsoleOutputPort()
{
    return 6668;
}

//=============================================================================
// DS_Protocol2015::acceptsConsoleCommnds
//=============================================================================

bool DS_Protocol2015::acceptsConsoleCommands()
{
    return true;
}

//=============================================================================
// DS_Protocol2015::defaultRadioAddress
//=============================================================================

QStringList DS_Protocol2015::defaultRadioAddress()
{
    return QStringList (DS_GetStaticIp (team(), 1));
}

//=============================================================================
// DS_Protocol2015::defaultRobotAddress
//=============================================================================

QStringList DS_Protocol2015::defaultRobotAddress()
{
    QStringList list;

    list.append (QString ("roboRIO-%1.local").arg (team()));
    list.append (QString ("roboRIO-%1").arg (team()));
    list.append (QString ("172.22.11.2"));
    list.append (QString ("127.0.0.1"));

    return list;
}

//=============================================================================
// DS_Protocol2015::reboot
//=============================================================================

void DS_Protocol2015::reboot()
{
    m_instructionCode = pInstructionRebootRIO;
}

//=============================================================================
// DS_Protocol2015::restartCode
//=============================================================================

void DS_Protocol2015::restartCode()
{
    m_instructionCode  = pInstructionRestartCode;
}

//=============================================================================
// DS_Protocol2015::resetProtocol
//=============================================================================

void DS_Protocol2015::_resetProtocol()
{
    m_instructionCode = pInstructionInvalid;
}

//=============================================================================
// DS_Protocol2015::readFMSPacket
//=============================================================================

bool DS_Protocol2015::_readFMSPacket (QByteArray data)
{
    Q_UNUSED (data);
    return true;
}

//=============================================================================
// DS_Protocol2015::readPacket
//=============================================================================

bool DS_Protocol2015::_readRobotPacket (QByteArray data)
{
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
    if (data.length() > offset)
        {
            QByteArray extended;
            for (int i = offset; i < data.length() - offset - 1; i++)
                extended.append (data.at (i));

            int id = extended.at (1);

            /* Robot wants us to rumble a joystick */
            if (id == pRIOHeaderJoystickOutput)
                qDebug() << Q_FUNC_INFO << "TODO: Joystick Output";

            /* Packet contains roboRIO CPU info */
            else if (id == pRIOHeaderCPUInformation)
                qDebug() << Q_FUNC_INFO << "TODO: CPU information";

            /* Packet contains roboRIO RAM info */
            else if (id == pRIOHeaderRAMInformation)
                {
                    int a = (quint8) extended.at (6);
                    int b = (quint8) extended.at (7);
                    emit ramUsageChanged (a + b);
                }

            /* Packet contains roboRIO disk info */
            else if (id == pRIOHeaderDiskInformation)
                {
                    int a = (quint8) extended.at (6);
                    int b = (quint8) extended.at (7);
                    emit diskUsageChanged (a + b);
                }

            /* Packet contains CAN metrics data */
            else if (id == pRIOHeaderCANMetrics)
                qDebug() << Q_FUNC_INFO << "TODO: CPU information" << extended.toHex();
        }

    /* Packet was successfully read, reset watchdog */
    return true;
}

//=============================================================================
// DS_Protocol2015::getFMSPacket
//============================================================================

QByteArray DS_Protocol2015::_getFmsPacket()
{
    QByteArray data;
    return data;
}

//=============================================================================
// DS_Protocol2015::getClientPacket
//=============================================================================

QByteArray DS_Protocol2015::_getClientPacket()
{
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
    data.append (DS_ToBytes (sentRobotPackets())); // Ping data
    data.append (pHeaderGeneral);                  // Protocol version code
    data.append (opcode);                          // Operation code
    data.append (instruction);                     // Special instructions
    data.append (getAllianceCode());               // Alliance & position
    data.append (extensions);                      // Joystick data or UTC info

    return data;
}

//=============================================================================
// DS_Protocol2015::getJoystickData
//=============================================================================

QByteArray DS_Protocol2015::_getJoystickData()
{
    QByteArray data;

    /* Do not send joystick data on DS init */
    if (sentRobotPackets() <= 5)
        return data;

    /* Generate data for each joystick */
    for (int i = 0; i < joysticks()->count(); ++i)
        {
            quint8 numAxes    = joysticks()->at (i)->numAxes;
            quint8 numButtons = joysticks()->at (i)->numButtons;
            quint8 numPovHats = joysticks()->at (i)->numPOVs;

            /* Add joystick information and put the section header */
            data.append (getJoystickSize (joysticks()->at (i)) - 1);
            data.append (pHeaderJoystick);

            /* Add axis data */
            data.append (numAxes);
            for (int axis = 0; axis < numAxes; ++axis)
                data.append (joysticks()->at (i)->axes [axis] * 127);

            /* Generate button data */
            int buttons = 0;
            for (int button = 0; button < numButtons; ++button)
                {
                    bool pressed = joysticks()->at (i)->buttons [button];
                    buttons += pressed ? qPow (2, button) : 0;
                }

            /* Add button data */
            data.append (numButtons);
            data.append (DS_ToBytes (buttons));

            /* Add hat/pov data */
            data.append (numPovHats);
            for (int hat = 0; hat < numPovHats; ++hat)
                data.append (DS_ToBytes (joysticks()->at (i)->POVs [hat]));
        }

    return data;
}

//=============================================================================
// DS_Protocol2015::getTimezoneData
//=============================================================================

QByteArray DS_Protocol2015::_getTimezoneData()
{
    QByteArray data;

    /* Add size (always 11) */
    data.append (0x0B);

    /* Get current date/time */
    QDateTime dt = QDateTime::currentDateTime();
    QDate date = dt.date();
    QTime time = dt.time();

    /* Add current date/time */
    data.append (pHeaderTime);
    data.append (DS_ToBytes (time.msec()));
    data.append (time.second());
    data.append (time.minute());
    data.append (time.hour());
    data.append (date.day());
    data.append (date.month());
    data.append (date.year() - 1900);

    /* Add timezone data */
    data.append (DS_GetTimezoneCode().length() + 1);
    data.append (pHeaderTimezone);
    data.append (DS_GetTimezoneCode());

    return data;
}

//=============================================================================
// DS_Protocol2015::getControlCode
//=============================================================================

int DS_Protocol2015::getControlCode()
{
    if (controlMode() == kControlTest)
        return pControlTest;

    else if (controlMode() == kControlAutonomous)
        return pControlAutonomous;

    else if (controlMode() == kControlTeleoperated)
        return pControlTeleoperated;

    return pControlTeleoperated;
}

//=============================================================================
// DS_Protocol2015::getAllianceCode
//=============================================================================

int DS_Protocol2015::getAllianceCode()
{
    if (alliance() == kAllianceRed1)
        return pRed1;

    else if (alliance() == kAllianceRed2)
        return pRed2;

    else if (alliance() == kAllianceRed3)
        return pRed3;

    else if (alliance() == kAllianceBlue1)
        return pBlue1;

    else if (alliance() == kAllianceBlue2)
        return pBlue2;

    else if (alliance() == kAllianceBlue3)
        return pBlue3;

    return pRed1;
}

//=============================================================================
// DS_Protocol2015::getJoystickSize
//=============================================================================

int DS_Protocol2015::getJoystickSize (DS_Joystick* joystick)
{
    return  5
            + (joystick->numAxes > 0 ? joystick->numAxes : 0)
            + (joystick->numButtons / 8)
            + (joystick->numButtons % 8 == 0 ? 0 : 1)
            + (joystick->numPOVs > 0 ? joystick->numPOVs * 2 : 0);
}
