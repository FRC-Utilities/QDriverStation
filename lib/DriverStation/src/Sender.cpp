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

#include "Sender.h"

DS_Sender::DS_Sender()
{
    m_index = 0;
}

void DS_Sender::resetIndex()
{
    m_index = 0;
}

void DS_Sender::send (DS_ControlPacket packet, QString host)
{
    m_socket.writeDatagram (generateControlPacket (packet),
                            QHostAddress (host),
                            DS_Ports::RoboRIO);
}

QByteArray DS_Sender::generateControlPacket (DS_ControlPacket packet)
{
    m_index += 1;

    int byte1 = 0x00;
    int byte2 = 0x00;
    int byte3 = 0x01;

    /* Generate ping bytes based on ping index */
    if (m_index > 0 && m_index <= 0xffff) {
        if (m_index <= 0xff)
            byte2 = m_index;

        else if (m_index <= 0xffff) {
            int index = m_index;

            while (index > 0xff) {
                index -= 0xff;
                byte1 += 0x01;
            }

            byte2 = index;
        }
    }

    /* Ping index is negative or exceeds the size limit */
    else
        resetIndex();

    /* Generate packet */
    QByteArray data;
    data.append ((char) byte1);
    data.append ((char) byte2);
    data.append ((char) byte3);
    data.append ((char) packet.mode);
    data.append ((char) packet.status);
    data.append ((char) packet.alliance);

    return data;
}
