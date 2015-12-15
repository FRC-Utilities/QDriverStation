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

const QUdpSocket::BindMode BIND_MODE = QAbstractSocket::ShareAddress |
                                       QAbstractSocket::ReuseAddressHint;

//=============================================================================
// MDNS::MDNS
//=============================================================================

MDNS::MDNS() {
    m_IPv4_receiver.setObjectName ("IPv4 receiver");
    m_IPv6_receiver.setObjectName ("IPv6 receiver");

    connect (&m_IPv4_receiver, SIGNAL (readyRead()), this, SLOT (ReadData()));
    connect (&m_IPv6_receiver, SIGNAL (readyRead()), this, SLOT (ReadData()));

    m_IPv4_receiver.bind (QHostAddress::AnyIPv4, 5353, BIND_MODE);
    m_IPv6_receiver.bind (QHostAddress::AnyIPv6, 5353, BIND_MODE);

    m_IPv6_receiver.joinMulticastGroup (QHostAddress ("FF02::FB"));
    m_IPv4_receiver.joinMulticastGroup (QHostAddress ("224.0.0.251"));
}

//=============================================================================
// MDNS::Query
//=============================================================================

void MDNS::Query (QString domain) {
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
    Send (packet);
}

//=============================================================================
// MDNS::Send
//=============================================================================

void MDNS::Send (QByteArray data) {
    if (!data.isEmpty()) {
        m_IPv6_sender.writeDatagram (data, QHostAddress ("FF02::FB"),    5353);
        m_IPv4_sender.writeDatagram (data, QHostAddress ("224.0.0.251"), 5353);
    }
}

//=============================================================================
// MDNS::ReadData
//=============================================================================

void MDNS::ReadData() {
    /* Read socket data */
    QByteArray data = GetSocketData (QObject::sender());

    /* The packet is not valid or is not a mDNS response */
    if (data.length() < 12 || data.at (2) != (char) 0x84u)
        return;

    /* Extract information from packet */
    QString host = GetHostName (data);
    QString ipv4 = GetIPv4Address (data, host);
    QString ipv6 = GetIPv6Address (data, host);

    /* Emit signals, prefer IPv4 over IPv6 */
    if (!host.isEmpty()) {
        if (!ipv4.isEmpty())
            emit IpFound (host, ipv4);

        else if (!ipv6.isEmpty())
            emit IpFound (host, ipv6);
    }
}

//=============================================================================
// MDNS::GetHostName
//=============================================================================

QString MDNS::GetHostName (QByteArray data) {
    int i = 12;
    QString rawData;
    QString address;

    while (data.at (i) != (char) 0x00) {
        ++i;
        rawData.append (data.at (i));
    }

    for (int i = 0; i < rawData.length() - 7; ++i)
        address.append (rawData.at (i));

    if (!address.isEmpty())
        return address + ".local";

    return address;
}

//=============================================================================
// MDNS::GetIPv4Address
//=============================================================================

QString MDNS::GetIPv4Address (QByteArray data, QString hostName) {
    int ip4_section_start = 12 + hostName.length() + 4;
    int iterator = ip4_section_start + 4;

    /* We cannot obtain IP of non-existent host */
    if (hostName.isEmpty() || !hostName.endsWith (".local"))
        return QString ("");

    /* 00 01 represents the start of IPv4 information */
    if (data.at (ip4_section_start + 2) == (char) 0x00 &&
            data.at (ip4_section_start + 3) == (char) 0x01) {

        /* Skip IPv4 information until we get to a place
         * with the bytes 00 04, which give us the size
         * of the IP address (which should be 4) */
        while ((data.at (iterator) != (char) 0x00) ||
                (data.at (iterator + 1) != (char) 0x04))
            ++iterator;

        /* IP address begins after 00 04, therefore, skip those bytes */
        iterator += 2;

        /* Get the IP address values */
        quint8 a = data.at (iterator);
        quint8 b = data.at (iterator + 1);
        quint8 c = data.at (iterator + 2);
        quint8 d = data.at (iterator + 3);

        /* Construct a string with the obtained values */
        return QString ("%1.%2.%3.%4")
               .arg (QString::number (a))
               .arg (QString::number (b))
               .arg (QString::number (c))
               .arg (QString::number (d));
    }

    /* No IPv4 addresses for us today... */
    return QString ("");
}

//=============================================================================
// MDNS::GetIPv6Address
//=============================================================================

QString MDNS::GetIPv6Address (QByteArray data, QString hostName) {
    Q_UNUSED (data);
    Q_UNUSED (hostName);

    /* We cannot obtain IP of non-existent host */
    if (hostName.isEmpty() || !hostName.endsWith (".local"))
        return QString ("");

    /* TODO */
    return QString ("");
}

//=============================================================================
// MDNS::GetSocketData
//=============================================================================

QByteArray MDNS::GetSocketData (QObject* caller) {
    if (caller != Q_NULLPTR) {
        if (caller->objectName() == m_IPv4_receiver.objectName())
            return DS_GetSocketData (&m_IPv4_receiver);

        else if (caller->objectName() == m_IPv6_receiver.objectName())
            return DS_GetSocketData (&m_IPv6_receiver);
    }

    return QByteArray ("");
}
