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
// Protocol codes/standards
//=============================================================================

enum ProtocolStandards {
    pFMS_Autonomous_RobotPresent     = 0x53,
    pFMS_Autonomous_NoRobotPresent   = 0x51,
    pFMS_Teleoperated_RobotPresent   = 0x43,
    pFMS_Teleoperated_NoRobotPresent = 0x41,
    pFMS_RobotEnabled                = 0x20,
    pFMS_UnknownHeader               = 0x00,
    pFMS_Alliance_Red                = 0x52,
    pFMS_Alliance_Blue               = 0x42,
    pFMS_Station_1                   = 0x31,
    pFMS_Station_2                   = 0x32,
    pFMS_Station_3                   = 0x33,
};

//=============================================================================
//FRC_Protocol2014::DS_FRC_Protocol2014
//=============================================================================

FRC_Protocol2014::FRC_Protocol2014() {
    m_reboot = false;
    m_restartCode = false;
    m_dsVersion = "090210a3";

    QTimer::singleShot (1000, Qt::CoarseTimer,
                        this, SLOT (_showProtocolWarning()));
}

//=============================================================================
//FRC_Protocol2014::fmsFrequency
//=============================================================================

int FRC_Protocol2014::fmsFrequency() {
    return 10;
}

//=============================================================================
//FRC_Protocol2014::robotFrequency
//=============================================================================

int FRC_Protocol2014::robotFrequency() {
    return 50;
}

//=============================================================================
//FRC_Protocol2014::fmsInputPort
//=============================================================================

int FRC_Protocol2014::fmsInputPort() {
    return 1120;
}

//=============================================================================
//FRC_Protocol2014::fmsOutputPort
//=============================================================================

int FRC_Protocol2014::fmsOutputPort() {
    return 1160;
}

//=============================================================================
//FRC_Protocol2014::clientPort
//=============================================================================

int FRC_Protocol2014::robotInputPort() {
    return 1110;
}

//=============================================================================
//FRC_Protocol2014::robotPort
//=============================================================================

int FRC_Protocol2014::robotOutputPort() {
    return 1150;
}

//=============================================================================
//FRC_Protocol2014::tcpProbePort
//=============================================================================

int FRC_Protocol2014::tcpProbesPort() {
    return 80;
}

//=============================================================================
//FRC_Protocol2014::netConsoleInputPort
//=============================================================================

int FRC_Protocol2014::netConsoleInputPort() {
    return 6666;
}

//=============================================================================
//FRC_Protocol2014::netConsoleOutputPort
//=============================================================================

int FRC_Protocol2014::netConsoleOutputPort() {
    return 6668;
}

//=============================================================================
//FRC_Protocol2014::acceptsConsoleCommnds
//=============================================================================

bool FRC_Protocol2014::acceptsConsoleCommands() {
    return true;
}

//=============================================================================
//FRC_Protocol2014::defaultRadioAddress
//=============================================================================

QStringList FRC_Protocol2014::defaultRadioAddress() {
    return QStringList (DS::getStaticIP (10, team(), 1));
}

//=============================================================================
//FRC_Protocol2014::defaultRobotAddress
//=============================================================================

QStringList FRC_Protocol2014::defaultRobotAddress() {
    QStringList list;

    list.append (QString (DS::getStaticIP (10, team(), 2)));
    list.append (QString ("127.0.0.1"));

    return list;
}

//=============================================================================
//FRC_Protocol2014::reboot
//=============================================================================

void FRC_Protocol2014::reboot() {
    m_reboot = true;
}

//=============================================================================
//FRC_Protocol2014::restartCode
//=============================================================================

void FRC_Protocol2014::restartCode() {
    m_restartCode = true;
}

//=============================================================================
//FRC_Protocol2014::resetProtocol
//=============================================================================

void FRC_Protocol2014::_resetProtocol() {
    m_reboot = false;
    m_restartCode = false;
}

//=============================================================================
//FRC_Protocol2014::_showProtocolWarning
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
//FRC_Protocol2014::readFMSPacket
//=============================================================================

bool FRC_Protocol2014::_readFMSPacket (QByteArray data) {
    int packet_num     = data.at (0) * 0xFF + data.at (1);
    int robot_state    = data.at (2);
    int robot_alliance = data.at (3);
    int robot_dstation = data.at (4);

    Q_UNUSED (packet_num);
    Q_UNUSED (robot_state);
    Q_UNUSED (robot_alliance);
    Q_UNUSED (robot_dstation);

    return true;
}

//=============================================================================
//FRC_Protocol2014::readPacket
//=============================================================================

bool FRC_Protocol2014::_readRobotPacket (QByteArray data) {
    Q_UNUSED (data);
    return true;
}

//=============================================================================
//FRC_Protocol2014::getFMSPacket
//=============================================================================

QByteArray FRC_Protocol2014::_getFmsPacket() {
    QByteArray data;

    /* Create 'dynamic' variables */
    quint8 _ip_a     = 0x00;
    quint8 _ip_b     = 0x00;
    quint8 _ip_c     = 0x00;
    quint8 _ip_d     = 0x00;
    quint8 _state    = 0x00;
    quint8 _station  = 0x00;
    quint8 _alliance = 0x00;

    /* Create 'static' variables */
    quint8 _missed_packets = 0;
    quint8 _unknown_header = pFMS_UnknownHeader;

    /* Get robot IP and split it */
    QStringList ip = robotAddress().split (".");
    if (ip.count() == 4) {
        _ip_a = ip.at (0).toInt();
        _ip_b = ip.at (1).toInt();
        _ip_c = ip.at (2).toInt();
        _ip_d = ip.at (3).toInt();
    }

    /* Get robot mode and state */
    if (controlMode() == DS::kControlTeleoperated)
        _state = isConnectedToRobot() ? pFMS_Teleoperated_RobotPresent :
                 pFMS_Teleoperated_NoRobotPresent;

    else if (controlMode() == DS::kControlAutonomous)
        _state = isConnectedToRobot() ? pFMS_Autonomous_RobotPresent :
                 pFMS_Autonomous_NoRobotPresent;

    _state += isEnabled() ? pFMS_RobotEnabled : 0;

    /* Get alliance and station */
    switch (alliance()) {
    case DS::kAllianceRed1:
        _station  = pFMS_Station_1;
        _alliance = pFMS_Alliance_Red;
        break;
    case DS::kAllianceRed2:
        _station  = pFMS_Station_2;
        _alliance = pFMS_Alliance_Red;
        break;
    case DS::kAllianceRed3:
        _station  = pFMS_Station_3;
        _alliance = pFMS_Alliance_Red;
        break;
    case DS::kAllianceBlue1:
        _station  = pFMS_Station_1;
        _alliance = pFMS_Alliance_Blue;
        break;
    case DS::kAllianceBlue2:
        _station  = pFMS_Station_2;
        _alliance = pFMS_Alliance_Blue;
        break;
    case DS::kAllianceBlue3:
        _station  = pFMS_Station_3;
        _alliance = pFMS_Alliance_Blue;
        break;
    }

    /* Construct the packet */
    data.append (DS::intToBytes (sentFmsPackets()));
    data.append (_state);
    data.append (_unknown_header);
    data.append (DS::intToBytes (team()));
    data.append (_ip_a);
    data.append (_ip_b);
    data.append (_ip_c);
    data.append (_ip_d);
    data.append (_alliance);
    data.append (_station);
    data.append (m_robotMacAddress);
    data.append (m_dsVersion);
    data.append (DS::intToBytes (_missed_packets));
    data.append (DS::intToBytes (sentRobotPackets()));
    data.append (DS::intToBytes (1000 / robotFrequency()));
    data.append (DS::intToBytes (12));
    data.append (0xFF);
    data.append (0xFF);
    data.append (0xFF);
    data.append (0xFF);
    data.append (0xFF);
    data.append (0xFF);
    data.append (DS::intToBytes (batteryVoltage()));

    /* Add CRC padding (4 bytes) */
    data.append ((char) 0x00);
    data.append ((char) 0x00);
    data.append ((char) 0x00);
    data.append ((char) 0x00);

    /* Add CRC checksum */
    data.append ((char) 0x00);
    data.append ((char) 0x00);
    data.append ((char) 0x00);
    data.append ((char) 0x00);

    return data;
}

//=============================================================================
//FRC_Protocol2014::getClientPacket
//=============================================================================

QByteArray FRC_Protocol2014::_getClientPacket() {
    QByteArray data;
    return data;
}

//=============================================================================
//FRC_Protocol2014::getJoystickData
//=============================================================================

QByteArray FRC_Protocol2014::_getJoystickData() {
    QByteArray data;
    return data;
}
