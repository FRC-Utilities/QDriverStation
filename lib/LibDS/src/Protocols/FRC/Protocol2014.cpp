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

#include "LibDS/Protocols/FRC/Protocol2014.h"

using namespace DS_Protocols;

//=============================================================================
// Protocol bytes
//=============================================================================

#define DS_TO_CRIO_CONTROL_TEST           0x62
#define DS_TO_CRIO_CONTROL_TELEOP         0x60
#define DS_TO_CRIO_CONTROL_REBOOT         0x80
#define DS_TO_CRIO_CONTROL_AUTONOMOUS     0x70
#define DS_TO_CRIO_CONTROL_DISABLED       0x40
#define DS_TO_CRIO_CONTROL_EMERGENCY_STOP 0x00

//=============================================================================
// FRC_Protocol2014::FRC_Protocol2014
//=============================================================================

FRC_Protocol2014::FRC_Protocol2014() {
    m_reboot = false;
    m_restartCode = false;
    m_dsVersion = "090210a3";

    QTimer::singleShot (1000, Qt::CoarseTimer,
                        this, SLOT (_showProtocolWarning()));
}

//=============================================================================
// FRC_Protocol2014::fmsFrequency
//=============================================================================

int FRC_Protocol2014::fmsFrequency() {
    return 10;
}

//=============================================================================
// FRC_Protocol2014::robotFrequency
//=============================================================================

int FRC_Protocol2014::robotFrequency() {
    return 50;
}

//=============================================================================
// FRC_Protocol2014::fmsInputPort
//=============================================================================

int FRC_Protocol2014::fmsInputPort() {
    return 1120;
}

//=============================================================================
// FRC_Protocol2014::fmsOutputPort
//=============================================================================

int FRC_Protocol2014::fmsOutputPort() {
    return 1160;
}

//=============================================================================
// FRC_Protocol2014::clientPort
//=============================================================================

int FRC_Protocol2014::robotInputPort() {
    return 1110;
}

//=============================================================================
// FRC_Protocol2014::robotPort
//=============================================================================

int FRC_Protocol2014::robotOutputPort() {
    return 1150;
}

//=============================================================================
// FRC_Protocol2014::tcpProbePort
//=============================================================================

int FRC_Protocol2014::tcpProbesPort() {
    return 80;
}

//=============================================================================
// FRC_Protocol2014::netConsoleInputPort
//=============================================================================

int FRC_Protocol2014::netConsoleInputPort() {
    return 6666;
}

//=============================================================================
// FRC_Protocol2014::netConsoleOutputPort
//=============================================================================

int FRC_Protocol2014::netConsoleOutputPort() {
    return 6668;
}

//=============================================================================
// FRC_Protocol2014::acceptsConsoleCommnds
//=============================================================================

bool FRC_Protocol2014::acceptsConsoleCommands() {
    return true;
}

//=============================================================================
// FRC_Protocol2014::reboot
//=============================================================================

void FRC_Protocol2014::reboot() {
    m_reboot = true;
}

//=============================================================================
// FRC_Protocol2014::restartCode
//=============================================================================

void FRC_Protocol2014::restartCode() {
    m_restartCode = true;
}

//=============================================================================
// FRC_Protocol2014::resetProtocol
//=============================================================================

void FRC_Protocol2014::_resetProtocol() {
    m_reboot = false;
    m_restartCode = false;
}

//=============================================================================
// FRC_Protocol2014::_showProtocolWarning
//=============================================================================

void FRC_Protocol2014::_showProtocolWarning() {
    DS::sendMessage ("<p><b><font color=#FF7722>WARNING: </font></b>"
                     "<font color=#FFFFFF>"
                     "This protocol is under heavy development and you WILL "
                     "encounter bugs. If using a real robot, limit its area of "
                     "movement by placing it over a box or something. "
                     "<b>Safety is your number one priority!</b></font></p>");
}

//=============================================================================
// FRC_Protocol2014::readFMSPacket
//=============================================================================

bool FRC_Protocol2014::_readFMSPacket (QByteArray data) {
    Q_UNUSED (data);
    return true;
}

//=============================================================================
// FRC_Protocol2014::readPacket
//=============================================================================

bool FRC_Protocol2014::_readRobotPacket (QByteArray data) {
    Q_UNUSED (data);
    return true;
}

//=============================================================================
// FRC_Protocol2014::getFMSPacket
//=============================================================================

QByteArray FRC_Protocol2014::_getFmsPacket() {
    QByteArray data;
    return data;
}

//=============================================================================
// FRC_Protocol2014::getClientPacket
//=============================================================================

QByteArray FRC_Protocol2014::_getClientPacket() {
    QByteArray data;

    data.resize (1024);
    data.fill   (0x00);

    data.append (DS::intToBytes (sentRobotPackets()));

    // TODO

    return data;
}

//=============================================================================
// FRC_Protocol2014::getJoystickData
//=============================================================================

QByteArray FRC_Protocol2014::_getJoystickData() {
    QByteArray data;

    // TODO

    return data;
}
