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

#include <QByteArray>
#include <QUdpSocket>
#include <QHostAddress>

#include "Packets.h"

QUdpSocket _SOCKET;
unsigned short _INDEX (0);
const int _TARGET_PORT (1110);

QByteArray DS_GenerateControlPacket (DS_ControlPacket packet)
{
    _INDEX += 1;

    int byte1 = 0x00;
    int byte2 = 0x00;
    int byte3 = 0x01;

    if (_INDEX <= 0xff)
        byte2 = _INDEX;

    else if (_INDEX <= 0xffff) {
        int index = _INDEX;

        while (index > 0xff) {
            index -= 0xff;
            byte1 += 0x01;
        }

        byte2 = copy;
    }

    else
        DS_ResetIndex();

    QByteArray data;
    data.append ((char) byte1);
    data.append ((char) byte2);
    data.append ((char) byte3);
    data.append ((char) packet.mode);
    data.append ((char) packet.status);
    data.append ((char) packet.alliance);

    return data;
}

void DS_SendControlPacket (DS_ControlPacket packet, QString host)
{
    QByteArray data = DS_GenerateControlPacket (packet);

    _SOCKET.writeDatagram (data.data(),
                           data.size(),
                           QHostAddress (host),
                           _TARGET_PORT);
}

void DS_ResetIndex()
{
    _INDEX = 0;
}
