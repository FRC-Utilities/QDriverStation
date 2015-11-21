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
#include "LibDS/Core/Discovery/MDNSResponder.h"

#define dPort        5335
#define dIPv6Address "FF02::FB"
#define dIPv4Address "224.0.0.251"

MDNSResponder::MDNSResponder() {
    connect (&m_IPv4_receiver, SIGNAL (readyRead()), this, SLOT (readIPv4Socket()));
    connect (&m_IPv6_receiver, SIGNAL (readyRead()), this, SLOT (readIPv6Socket()));

    m_IPv4_receiver.bind (QHostAddress (dIPv4Address), dPort,
                          QUdpSocket::ShareAddress);
    m_IPv6_receiver.bind (QHostAddress (dIPv6Address), dPort,
                          QUdpSocket::ShareAddress);
}

void MDNSResponder::query (QString domain) {
    QByteArray data;
    domain = domain.replace (".local", "").toLower();
    sendPacket (data);
}

void MDNSResponder::readIPv4Socket() {
    processResponse (DS_GetSocketData (&m_IPv4_receiver));
}

void MDNSResponder::readIPv6Socket() {
    processResponse (DS_GetSocketData (&m_IPv6_receiver));
}

void MDNSResponder::sendPacket (QByteArray data) {
    m_IPv4_sender.writeDatagram (data, QHostAddress (dIPv4Address), dPort);
    m_IPv6_sender.writeDatagram (data, QHostAddress (dIPv6Address), dPort);
}

void MDNSResponder::processResponse (QByteArray response) {
    qDebug() << response.toHex();
}
