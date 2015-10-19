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

#include "LibDS/DS_Protocol2014.h"

#define CONTROL_DISABLED 0x40
#define CONTROL_NO_COMM  0x04
#define CONTROL_TELEOP   0x60
#define CONTROL_TEST     0x62
#define CONTROL_AUTO     0x70
#define CONTROL_ESTOP    0x00
#define STATUS_REBOOT    0x80
#define ALLIANCE_RED1    0x36
#define PORT_ROBOT       1110
#define PORT_CLIENT      1150

DS_Protocol2014::DS_Protocol2014()
{

}

void DS_Protocol2014::reboot()
{

}

int DS_Protocol2014::robotPort()
{
    return PORT_ROBOT;
}

int DS_Protocol2014::clientPort()
{
    return PORT_CLIENT;
}

void DS_Protocol2014::restartCode()
{

}

QByteArray DS_Protocol2014::getClientPacket()
{
    QByteArray data;
    return data;
}

void DS_Protocol2014::resetProtocol()
{

}

QString DS_Protocol2014::defaultRadioAddress()
{
    return DS_GetStaticIp (p_team, 1);
}

QString DS_Protocol2014::defaultRobotAddress()
{
    return DS_GetStaticIp (p_team, 2);
}

void DS_Protocol2014::downloadRobotInformation()
{

}

QByteArray DS_Protocol2014::generateJoystickData()
{
    QByteArray data;
    return data;
}

QByteArray DS_Protocol2014::generateTimezoneData()
{
    QByteArray data;
    return data;
}

void DS_Protocol2014::readRobotData (QByteArray data)
{

}

char DS_Protocol2014::getControlCode (DS_ControlMode mode)
{
    return CONTROL_DISABLED;
}

DS_ControlMode DS_Protocol2014::getControlMode (char byte)
{
    return DS_ControlDisabled;
}

char DS_Protocol2014::getAllianceCode (DS_Alliance alliance)
{
    return ALLIANCE_RED1;
}
