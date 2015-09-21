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

DS_PacketCount DS_Sender::getPingData (int index)
{
    int ping = index + 1;

    if (ping > 0xffff)
        ping = 0;

    DS_PacketCount count;
    count.byte1 = ping / 0xff;
    count.byte2 = ping % 0xff;

    return count;
}

void DS_Sender::send (int index,
                      QString host,
                      DS_JoystickData stick,
                      DS_ControlPacket packet)
{
    Q_UNUSED (stick);

    QByteArray data;
    DS_PacketCount c = getPingData (index);

    data.append (c.byte1);
    data.append (c.byte2);
    data.append (0x01);
    data.append (packet.mode);
    data.append (packet.status);
    data.append (packet.alliance);

    m_socket.writeDatagram (data, QHostAddress (host), DS_Ports::RoboRIO);
}
