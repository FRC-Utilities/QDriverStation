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

#include "../headers/DS_Protocol.h"

DS_Protocol::DS_Protocol()
{
    p_team = 0;
    p_robotCode = false;
    p_alliance = DS_AllianceRed1;
    p_controlMode = DS_ControlDisabled;
    p_joysticks = new QList<DS_Joystick*>;
}
bool DS_Protocol::robotCode()
{
    return p_robotCode;
}

DS_Alliance DS_Protocol::alliance()
{
    return p_alliance;
}

DS_ControlMode DS_Protocol::controlMode()
{
    return p_controlMode;
}

void DS_Protocol::setTeamNumber (int team)
{
    p_team = team;
}

void DS_Protocol::setRobotAddress (QString address)
{
    p_robotAddress = address;
}

void DS_Protocol::setAlliance (DS_Alliance alliance)
{
    p_alliance = alliance;
}

void DS_Protocol::setControlMode (DS_ControlMode mode)
{
    p_controlMode = mode;
    emit controlModeChanged (mode);
}

void DS_Protocol::setJoysticks (QList<DS_Joystick*>* joysticks)
{
    p_joysticks = joysticks;
}

QByteArray DS_Protocol::bitsToBytes (QBitArray bits)
{
    QByteArray bytes (bits.count() / 8, 0);

    for (int i = 0; i < bits.count(); ++i)
        bytes [i / 8] = (bytes.at (i / 8) | ((bits [i] ? 1 : 0) << (i % 8)));

    return bytes;
}
