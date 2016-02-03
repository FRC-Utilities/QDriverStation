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

#include "LibDS/Protocols/Protocol2014.h"

//=============================================================================
// Protocol codes/standards
//=============================================================================

enum ProtocolStandards
{
};

//=============================================================================
// DS_Protocol2014::fmsFrequency
//=============================================================================

int DS_Protocol2014::fmsFrequency()
{
    return 2;
}

//=============================================================================
// DS_Protocol2014::robotFrequency
//=============================================================================

int DS_Protocol2014::robotFrequency()
{
    return 50;
}

//=============================================================================
// DS_Protocol2014::fmsInputPort
//=============================================================================

int DS_Protocol2014::fmsInputPort()
{
    return DS_PROTOCOL_NO_PORT;
}

//=============================================================================
// DS_Protocol2014::fmsOutputPort
//=============================================================================

int DS_Protocol2014::fmsOutputPort()
{
    return DS_PROTOCOL_NO_PORT;
}

//=============================================================================
// DS_Protocol2014::clientPort
//=============================================================================

int DS_Protocol2014::robotInputPort()
{
    return DS_PROTOCOL_NO_PORT;
}

//=============================================================================
// DS_Protocol2014::robotPort
//=============================================================================

int DS_Protocol2014::robotOutputPort()
{
    return DS_PROTOCOL_NO_PORT;
}

//=============================================================================
// DS_Protocol2014::tcpProbePort
//=============================================================================

int DS_Protocol2014::tcpProbesPort()
{
    return 80;
}

//=============================================================================
// DS_Protocol2014::netConsoleInputPort
//=============================================================================

int DS_Protocol2014::netConsoleInputPort()
{
    return 6666;
}

//=============================================================================
// DS_Protocol2014::netConsoleOutputPort
//=============================================================================

int DS_Protocol2014::netConsoleOutputPort()
{
    return 6668;
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
    return QStringList (DS_GetStaticIp (team(), 1));
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
}

//=============================================================================
// DS_Protocol2014::restartCode
//=============================================================================

void DS_Protocol2014::restartCode()
{
}

//=============================================================================
// DS_Protocol2014::resetProtocol
//=============================================================================

void DS_Protocol2014::_resetProtocol()
{
}

//=============================================================================
// DS_Protocol2014::readFMSPacket
//=============================================================================

bool DS_Protocol2014::_readFMSPacket (QByteArray data)
{
    Q_UNUSED (data);
    return true;
}

//=============================================================================
// DS_Protocol2014::readPacket
//=============================================================================

bool DS_Protocol2014::_readRobotPacket (QByteArray data)
{
    Q_UNUSED (data);
    return true;
}

//=============================================================================
// DS_Protocol2014::getFMSPacket
//=============================================================================

QByteArray DS_Protocol2014::_getFmsPacket()
{
    QByteArray data;
    return data;
}

//=============================================================================
// DS_Protocol2014::getClientPacket
//=============================================================================

QByteArray DS_Protocol2014::_getClientPacket()
{
    QByteArray data;
    return data;
}

//=============================================================================
// DS_Protocol2014::getJoystickData
//=============================================================================

QByteArray DS_Protocol2014::_getJoystickData()
{
    QByteArray data;
    return data;
}

//=============================================================================
// DS_Protocol2014::getTimezoneData
//=============================================================================

QByteArray DS_Protocol2014::_getTimezoneData()
{
    QByteArray data;
    return data;
}
