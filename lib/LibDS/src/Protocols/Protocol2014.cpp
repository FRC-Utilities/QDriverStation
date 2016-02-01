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

#include "LibDS/Protocols/Protocol2014.h"

//=============================================================================
// Protocol codes/standards
//=============================================================================

enum ProtocolStandards
{
    /* Client -> RoboRIO operation modes/states */
    pEmStopOn                 = 0x80, /**< Emergency stop enabled */
    pEmStopOff                = 0x00, /**< Emergency stop disabled */
    pEnabled                  = 0x04, /**< Robot Enabled */
    pDisabled                 = 0x00, /**< Robot Disabled */
    pControlTest              = 0x01, /**< Individual actuators can be moved */
    pControlAutonomous        = 0x02, /**< Robot takes over the world */
    pControlTeleoperated      = 0x00, /**< User moves the robot */

    /* Client -> RoboRIO section codes */
    pHeaderTime               = 0x0f, /**< Indicates start of current time data */
    pHeaderGeneral            = 0x01, /**< Indicates a client packet */
    pHeaderJoystick           = 0x0c, /**< Indicates start of JS section */
    pHeaderTimezone           = 0x10, /**< Indicates start of Timezone data */

    /* Client -> RoboRIO special instructions */
    pStatusNormal             = 0x10, /**< Normal operation */
    pStatusInvalid            = 0x00, /**< No communication */
    pStatusRebootRio          = 0x14, /**< Reboot the roboRIO */
    pStatusRestartCode        = 0x16, /**< Kill the user code */

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
// DS_Protocol2014::robotPort
//=============================================================================

int DS_Protocol2014::robotPort()
{
    return DS_PROTOCOL_NO_PORT;
}

//=============================================================================
// DS_Protocol2014::clientPort
//=============================================================================

int DS_Protocol2014::clientPort()
{
    return DS_PROTOCOL_NO_PORT;
}

//=============================================================================
// DS_Protocol2014::tcpProbePort
//=============================================================================

int DS_Protocol2014::tcpProbePort()
{
    return 80;
}

//=============================================================================
// DS_Protocol2014::netConsolePort
//=============================================================================

int DS_Protocol2014::netConsolePort()
{
    return 6666;
}

//=============================================================================
// DS_Protocol2014::acceptsConsoleCommnds
//=============================================================================

bool DS_Protocol2014::acceptsConsoleCommands()
{
    return true;
}

//=============================================================================
// DS_Protocol2014::defaultRadioAddress
//=============================================================================

QStringList DS_Protocol2014::defaultRadioAddress()
{
    QStringList list;
    list.append (DS_GetStaticIp (team(), 1));
    return list;
}

//=============================================================================
// DS_Protocol2014::defaultRobotAddress
//=============================================================================

QStringList DS_Protocol2014::defaultRobotAddress()
{
    QStringList list;

    list.append (QString (DS_GetStaticIp (team(), 2)));
    list.append (QString ("127.0.0.1"));

    return list;
}

//=============================================================================
// DS_Protocol2014::reboot
//=============================================================================

void DS_Protocol2014::reboot()
{
    updateStatus (pStatusRebootRio);
}

//=============================================================================
// DS_Protocol2014::restartCode
//=============================================================================

void DS_Protocol2014::restartCode()
{
    updateStatus (pStatusRestartCode);
}

//=============================================================================
// DS_Protocol2014::resetProtocol
//=============================================================================

void DS_Protocol2014::resetProtocol()
{
    updateStatus (pStatusInvalid);
}

//=============================================================================
// DS_Protocol2014::readPacket
//=============================================================================

bool DS_Protocol2014::readPacket (QByteArray data)
{
    Q_UNUSED (data);
    return true;
}

//=============================================================================
// DS_Protocol2014::getClientPacket
//=============================================================================

QByteArray DS_Protocol2014::getClientPacket()
{
    QByteArray data;
    return data;
}

//=============================================================================
// DS_Protocol2014::getJoystickData
//=============================================================================

QByteArray DS_Protocol2014::getJoystickData()
{
    QByteArray data;
    return data;
}

//=============================================================================
// DS_Protocol2014::getTimezoneData
//=============================================================================

QByteArray DS_Protocol2014::getTimezoneData()
{
    QByteArray data;
    return data;
}

//=============================================================================
// DS_Protocol2014::getControlCode
//=============================================================================

int DS_Protocol2014::getControlCode()
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
// DS_Protocol2014::getAllianceCode
//=============================================================================

int DS_Protocol2014::getAllianceCode()
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
// DS_Protocol2014::getJoystickSize
//=============================================================================

int DS_Protocol2014::getJoystickSize (DS_Joystick* joystick)
{
    Q_UNUSED (joystick);
    return 0;
}
