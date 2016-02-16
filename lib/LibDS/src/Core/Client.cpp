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

#include "LibDS/Core/Client.h"

//=============================================================================
// DS_Client::DS_Client
//=============================================================================

DS_Client::DS_Client (QObject* parent) : QObject (parent)
{
    connect (&m_fmsReceiver,   &QTcpSocket::readyRead,
             this,             &DS_Client::readFmsPacket);
    connect (&m_robotReceiver, &QUdpSocket::readyRead,
             this,             &DS_Client::readRobotPacket);
}

//=============================================================================
// DS_Client::sendFmsPacket
//=============================================================================

void DS_Client::sendToFms (QByteArray data)
{
    m_fmsSender.write (data);
}

//=============================================================================
// DS_Client::sendClientPacket
//=============================================================================

void DS_Client::sendToRobot (QByteArray data)
{
    m_robotSender.writeDatagram (data, QHostAddress (m_robotAddress), m_robotPort);
}

//=============================================================================
// DS_Client::setFmsInputPort
//=============================================================================

void DS_Client::setFmsInputPort (int port)
{
    m_fmsReceiver.disconnectFromHost();
    m_fmsReceiver.bind (QHostAddress::Any, port, QTcpSocket::ShareAddress);
    m_fmsReceiver.setSocketOption (QAbstractSocket::MulticastLoopbackOption, 0);

    DS_LogMessage (kLibLevel,
                   "FMS input port set to: " + QString::number (port));
}

//=============================================================================
// DS_Client::setFmsOutputPort
//=============================================================================

void DS_Client::setFmsOutputPort (int port)
{
    m_fmsPort = port;
    m_fmsSender.disconnectFromHost();
    m_fmsSender.connectToHost (QHostAddress::Any, port);

    DS_LogMessage (kLibLevel,
                   "FMS output port set to: " + QString::number (port));
}

//=============================================================================
// DS_Client::setRobotInputPort
//=============================================================================

void DS_Client::setRobotInputPort (int port)
{
    m_robotReceiver.disconnectFromHost();
    m_robotReceiver.bind (QHostAddress::Any, port, QUdpSocket::ShareAddress);
    m_robotReceiver.setSocketOption (QAbstractSocket::MulticastLoopbackOption, 0);

    DS_LogMessage (kLibLevel,
                   "RIO input port set to: " + QString::number (port));
}

//=============================================================================
// DS_Client::setRobotOutputPort
//=============================================================================

void DS_Client::setRobotOutputPort (int port)
{
    m_robotPort = port;
    DS_LogMessage (kLibLevel,
                   "RIO output port set to: " + QString::number (port));
}

//=============================================================================
// DS_Client::setRobotAddress
//=============================================================================

void DS_Client::setRobotAddress (QString address)
{
    m_robotAddress = address;
}

//=============================================================================
// DS_Client::readFmsPacket
//=============================================================================

void DS_Client::readFmsPacket()
{
    emit fmsPacketReceived (DS_GetSocketData (&m_fmsReceiver));
}

//=============================================================================
// DS_Client::readRobotPacket
//=============================================================================

void DS_Client::readRobotPacket()
{
    emit robotPacketReceived (DS_GetSocketData (&m_robotReceiver));
}
