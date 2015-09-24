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
#include "Sender.h"

//#include <QDebug>
void DS_Sender::send (int index,
                      QString host,
                      DS_Status status,
                      DS_ControlMode mode,
                      DS_Alliance alliance,
                      QByteArray joystickData)
{
    /* Generate the ping data */
    QByteArray data;
    DS_PingData ping;
    ping.generatePingData (index + 1);

    /* Add ping data */
    data.append (ping.byte1);
    data.append (ping.byte2);

    /* Add this misterious number */
    data.append (0x01);

    /* Add the desired control mode, robot status and alliance data */
    data.append (mode);
    data.append (status);
    data.append (alliance);

    /* Add joystick input information if the robot is in TeleOp */
    if (mode == DS_ControlTeleOp)
        data.append (joystickData);

    /* Send the data */
    m_socket.writeDatagram (data, QHostAddress (host), DS_Ports::RoboRIO);
}
