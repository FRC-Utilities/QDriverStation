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

#include "LibDS/Core/Common.h"
#include "LibDS/Core/Discovery/MDNS.h"

MDNS::MDNS() {
    connect (&m_IPv4_receiver, SIGNAL (readyRead()), this, SLOT (readIPv4Socket()));
    connect (&m_IPv6_receiver, SIGNAL (readyRead()), this, SLOT (readIPv6Socket()));

    QUdpSocket::BindFlag flag = QUdpSocket::ShareAddress;
    m_IPv6_receiver.bind (QHostAddress ("FF02::FB"),    5353, flag);
    m_IPv4_receiver.bind (QHostAddress ("224.0.0.251"), 5353, flag);
}

MDNS::~MDNS() {
    m_IPv4_sender.abort();
    m_IPv6_sender.abort();
    m_IPv4_receiver.abort();
    m_IPv6_receiver.abort();
}

void MDNS::query (QString domain) {
    QByteArray data;
    QByteArray header;
    QByteArray packet;

    /* Ensure that the domain is valid */
    domain = domain.replace (".local.", "");
    domain = domain.replace (".local",  "");

    /* Check that domain length is valid */
    if (domain.length() > 0xFF)
        return;

    /* Create header & flags */
    header.append ((char) 0x00);
    header.append ((char) 0x00);
    header.append ((char) 0x00);
    header.append ((char) 0x00);
    header.append ((char) 0x00);
    header.append ((char) 0x02);
    header.append ((char) 0x00);
    header.append ((char) 0x00);
    header.append ((char) 0x00);
    header.append ((char) 0x00);
    header.append ((char) 0x00);
    header.append ((char) 0x00);

    /* Add domain data */
    data.append (domain.length());
    data.append (domain.toUtf8());

    /* Add '.local' section */
    QString local = "local";
    data.append (local.length());
    data.append (local.toUtf8());
    data.append ((char) 0x00);
    data.append ((char) 0x00);
    data.append ((char) 0x01);
    data.append ((char) 0x00);
    data.append ((char) 0x01);
    data.append ((char) 0xC0);
    data.append ((char) 0x0C);
    data.append ((char) 0x00);
    data.append ((char) 0x1C);
    data.append ((char) 0x00);
    data.append ((char) 0x01);

    /* Create the complete packet */
    packet.append (header);
    packet.append (data);

    /* Send the generated packet */
    send (packet);
}

void MDNS::readIPv4Socket() {
    processResponse (DS_GetSocketData (&m_IPv4_receiver));
}

void MDNS::readIPv6Socket() {
    processResponse (DS_GetSocketData (&m_IPv6_receiver));
}

void MDNS::send (QByteArray data) {
    if (!data.isEmpty()) {
        m_IPv6_sender.writeDatagram (data, QHostAddress ("FF02::FB"),    5353);
        m_IPv4_sender.writeDatagram (data, QHostAddress ("224.0.0.251"), 5353);
    }
}

void MDNS::processResponse (QByteArray response) {
    /* TODO */
    qDebug() << response.toHex();
}
