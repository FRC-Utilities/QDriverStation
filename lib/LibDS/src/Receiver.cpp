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

#include "Ports.h"
#include "Receiver.h"

DS_Receiver::DS_Receiver()
{
    m_socket.bind (DS_Ports::Client, QUdpSocket::ShareAddress);
    connect (&m_socket, SIGNAL (readyRead()), this, SLOT (onDataReceived()));
}

void DS_Receiver::onDataReceived()
{
    while (m_socket.hasPendingDatagrams()) {
        /* Read 'raw' data */
        QByteArray data;
        data.resize (m_socket.pendingDatagramSize());
        m_socket.readDatagram (data.data(), data.size());

        /* Process downloaded data into a DS_RobotPacket */
        DS_RobotPacket packet = getRobotPacket (data);

        /* Read the values of the packet and notify other objects */
        emit voltageChanged (packet.voltage);
        emit voltageChanged (packet.voltageString);
        emit dataReceived (packet.pongData.getPingIndex());

        /* Only notify about code change if necessary */
        if (m_code != packet.hasCode) {
            m_code = packet.hasCode;
            emit userCodeChanged (m_code);
        }
    }
}

DS_RobotPacket DS_Receiver::getRobotPacket (QByteArray data)
{
    DS_RobotPacket receiver;

    receiver.commVersion = data.at (2);
    receiver.pongData.byte1 = data.at (0);
    receiver.pongData.byte2 = data.at (1);
    receiver.requestDataTime = (data.at (7) == 0x01);
    receiver.controlMode = (DS_ControlMode) data.at (3);
    receiver.programMode = (DS_ProgramMode) data.at (4);
    receiver.hasCode = receiver.programMode != DS_ProgramNoCode;
    receiver.voltageString = QString::number (data.at (5)) + "." +
                             QString::number (data.at (6));
    receiver.voltage = receiver.voltageString.toDouble();

    return receiver;
}
