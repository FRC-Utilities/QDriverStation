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

#include "LibDS/Core/Client.h"

//=============================================================================
// DS_Client::DS_Client
//=============================================================================

DS_Client::DS_Client()
{
    connect (&m_receiver, SIGNAL (readyRead()), this, SLOT (readPacket()));
}

//=============================================================================
// DS_Client::SendToRobot
//=============================================================================

void DS_Client::sendPacket (QByteArray data)
{
    m_sender.writeDatagram (data, QHostAddress (m_address), m_robotPort);
}

//=============================================================================
// DS_Client::SetRobotPort
//=============================================================================

void DS_Client::setRobotPort (int port)
{
    m_robotPort = port;
}

//=============================================================================
// DS_Client::SetClientPort
//=============================================================================

void DS_Client::setClientPort (int port)
{
    m_receiver.disconnectFromHost();

    m_receiver.bind (QHostAddress::Any, port, QUdpSocket::ShareAddress);
    m_receiver.setSocketOption (QAbstractSocket::MulticastLoopbackOption, 0);
}

//=============================================================================
// DS_Client::SetRobotAddress
//=============================================================================

void DS_Client::setRobotAddress (QString address)
{
    m_address = address;

    qDebug() << address;
}

//=============================================================================
// DS_Client::OnDataReceived
//=============================================================================

void DS_Client::readPacket()
{
    emit dataReceived (DS_GetSocketData (&m_receiver));
}
