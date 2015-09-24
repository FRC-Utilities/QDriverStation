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

        /* Read the socket data */
        QByteArray data;
        data.resize (m_socket.pendingDatagramSize());
        m_socket.readDatagram (data.data(), data.size());

        /* Process downloaded data into a DS_RobotPacket */
        DS_RobotPacket packet = getRobotPacket (data);

        /* Read the values of the packet and notify other objects */
        emit voltageChanged (packet.voltage);
        emit voltageChanged (packet.voltageString);

        /* Only notify about code change if necessary */
        if (m_code != packet.hasCode) {
            m_code = packet.hasCode;
            emit userCodeChanged (m_code);
        }

        /* Send current date/time to robot */
        if (packet.requestDateTime) {
            /* TODO */
        }
    }
}

DS_RobotPacket DS_Receiver::getRobotPacket (QByteArray data)
{
    DS_RobotPacket receiver;

    /* Get communication version, god knows if we will use it... */
    receiver.commVersion = data.at (2);

    /* Get the pong data */
    receiver.pongData.byte1 = data.at (0);
    receiver.pongData.byte2 = data.at (1);

    /* Decide if we must send current date/time to robot */
    receiver.requestDateTime = (data.at (7) == 0x01);

    /* Get the confirmation code of control mode */
    receiver.controlMode = (DS_ControlMode) data.at (3);

    /* Get current mode and determine if user code is present */
    receiver.programMode = (DS_ProgramMode) data.at (4);
    receiver.hasCode = (receiver.programMode != DS_ProgramNoCode);

    /* Calculate the voltage */
    QString major = QString::number (data.at (5));
    QString minor = QString::number (data.at (6));

    if (minor.length() > 2)
        minor = QString ("%1%2").arg (minor.at (0), minor.at (1));

    else if (minor.length() == 1)
        minor = QString ("0%1").arg (minor);

    receiver.voltageString = QString ("%1.%2").arg (major, minor);
    receiver.voltageString = receiver.voltageString.replace ("-", "");
    receiver.voltage = receiver.voltageString.toDouble();

    return receiver;
}
