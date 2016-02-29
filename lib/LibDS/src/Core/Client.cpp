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

using namespace DS_Core;

//=============================================================================
//Client::DS_Client
//=============================================================================

Client::Client (QObject* parent) : QObject (parent) {
    connect (&m_fmsReceiver,   &QTcpSocket::readyRead,
             this,             &Client::readFmsPacket);
    connect (&m_robotReceiver, &QUdpSocket::readyRead,
             this,             &Client::readRobotPacket);
}

//=============================================================================
//Client::sendFmsPacket
//=============================================================================

void Client::sendToFms (QByteArray data) {
    m_fmsSender.write (data);
}

//=============================================================================
//Client::sendClientPacket
//=============================================================================

void Client::sendToRobot (QByteArray data) {
    m_robotSender.writeDatagram (data,
                                 QHostAddress (m_robotAddress),
                                 m_robotPort);
}

//=============================================================================
//Client::setFmsInputPort
//=============================================================================

void Client::setFmsInputPort (int port) {
    m_fmsReceiver.disconnectFromHost();
    m_fmsReceiver.bind (QHostAddress::Any, port, QTcpSocket::ShareAddress);
    m_fmsReceiver.setSocketOption (QAbstractSocket::MulticastLoopbackOption, 0);

    DS::log (DS::kLibLevel,
             "FMS input port set to: " + QString::number (port));
}

//=============================================================================
//Client::setFmsOutputPort
//=============================================================================

void Client::setFmsOutputPort (int port) {
    m_fmsPort = port;
    m_fmsSender.disconnectFromHost();
    m_fmsSender.connectToHost (QHostAddress::Any, port);

    DS::log (DS::kLibLevel,
             "FMS output port set to: " + QString::number (port));
}

//=============================================================================
//Client::setRobotInputPort
//=============================================================================

void Client::setRobotInputPort (int port) {
    m_robotReceiver.disconnectFromHost();
    m_robotReceiver.bind (QHostAddress::Any, port, QUdpSocket::ShareAddress);
    m_robotReceiver.setSocketOption (QAbstractSocket::MulticastLoopbackOption, 0);

    DS::log (DS::kLibLevel,
             "Robot input port set to: " + QString::number (port));
}

//=============================================================================
//Client::setRobotOutputPort
//=============================================================================

void Client::setRobotOutputPort (int port) {
    m_robotPort = port;
    DS::log (DS::kLibLevel,
             "Robot output port set to: " + QString::number (port));
}

//=============================================================================
//Client::setRobotAddress
//=============================================================================

void Client::setRobotAddress (QString address) {
    m_robotAddress = address;
}

//=============================================================================
//Client::readFmsPacket
//=============================================================================

void Client::readFmsPacket() {
    emit fmsPacketReceived (DS::readSocket (&m_fmsReceiver));
}

//=============================================================================
//Client::readRobotPacket
//=============================================================================

void Client::readRobotPacket() {
    emit robotPacketReceived (DS::readSocket (&m_robotReceiver));
}
