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

void MDNS::Query (QString host) {
    QByteArray data;
    QByteArray header;
    QByteArray packet;

    /* Ensure that the domain is valid */
    host = host.replace (".local.", "");
    host = host.replace (".local",  "");

    /* Check that domain length is valid */
    if (host.length() > 0xFF)
        return;

    /* Create header & flags */
    header.append ((char) 0x00);
    header.append ((char) 0x00);
    header.append ((char) 0x00);
    header.append ((char) 0x00);
    header.append ((char) 0x00);
    header.append ((char) 0x01);
    header.append ((char) 0x00);
    header.append ((char) 0x00);
    header.append ((char) 0x00);
    header.append ((char) 0x00);
    header.append ((char) 0x00);
    header.append ((char) 0x00);

    /* Add domain data */
    data.append (host.length());
    data.append (host.toUtf8());

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

    /* Try to obtain the IPv4 address, if it fails, this function will
     * call \c GetIPv6Address() to obtain the IPv6 address */
    GetIPv4Address (data, GetHostName (data));
}

//=============================================================================
// MDNS::GetHostName
//=============================================================================

QString MDNS::GetHostName (QByteArray data) {
    int i = 12;
    QString rawData;
    QString address;

    /* The host name starts when the packet flags end (byte #12) and ends
     * with a (obligatory) null character */
    while (data.at (i) != (char) 0x00) {
        ++i;
        rawData.append (data.at (i));
    }

    /* The '.local' section cannot be used when obtained directly from the
     * packet, since it contains additional characters.
     * For the moment, we will only extract the host name.
     * In other words, from roboRIO.local we will obtain only 'roboRIO' */
    for (int i = 0; i < rawData.length() - 7; ++i)
        address.append (rawData.at (i));

    /* If the obtained host name is not empty, it deserves to have the
     * '.local' section appended */
    if (!address.isEmpty())
        return address + ".local";

    /* The address is invalid */
    return QString ("");
}

//=============================================================================
// MDNS::GetIPv4Address
//=============================================================================

void MDNS::GetIPv4Address (QByteArray data, QString host) {
    int iterator = 12 + host.length();

    /* We cannot obtain IP of non-existent host */
    if (host.isEmpty())
        return;

    /* Skip IPv4 information until we get to a place with the bytes '00 04',
     * which give us the size of the IP address (which should be always 4) */
    while ((data.at (iterator + 0) != (char) 0x00) ||
            (data.at (iterator + 1) != (char) 0x04))
        ++iterator;

    /* IP address begins AFTER 00 04, therefore, skip those bytes too */
    iterator += 2;

    /* Construct a string with the IP bytes */
    QString ip = QString ("%1.%2.%3.%4")
                 .arg (QString::number ((quint8) data.at (iterator)))
                 .arg (QString::number ((quint8) data.at (iterator + 1)))
                 .arg (QString::number ((quint8) data.at (iterator + 2)))
                 .arg (QString::number ((quint8) data.at (iterator + 3)));

    /* If the obtained IP is valid, notify other objects */
    if (QHostAddress (ip).protocol() == QAbstractSocket::IPv4Protocol)
        emit IpFound (host, ip);

    /* If the obtained IP is not good enough for us, try to get the IPv6 */
    else
        GetIPv6Address (data, host, iterator + 4);
}

//=============================================================================
// MDNS::GetIPv6Address
//=============================================================================

void MDNS::GetIPv6Address (QByteArray data, QString host, int iterator) {
    /* We cannot obtain IP of non-existent host */
    if (host.isEmpty())
        return;

    /* Skip IPv6 information until we get to a place with the bytes '00 10',
     * which give us the size of the IP address (which should be always 32) */
    while ((data.at (iterator + 0) != (char) 0x00) ||
            (data.at (iterator + 1) != (char) 0x10)) {
        ++iterator;

        QByteArray ba;
        ba.append (data.at (iterator));
    }

    /* IP address begins AFTER 00 10, therefore, skip those bytes too */
    iterator += 2;

    /* Construct the IPv6 string with the IP bytes */
    QString ip;
    QByteArray* hexNumber = new QByteArray;
    for (int i = 0; i < 0x10; i += 2) {
        /* Get a 16-bit number from two 8-bit numbers */
        quint8 upper = data.at (iterator + i);
        quint8 lower = data.at (iterator + i + 1);
        quint16 nmbr = (upper << 8) | lower;

        /* Get hexadecimal representation of the number */
        hexNumber->clear();
        hexNumber->append (nmbr);

        /* Append hex number to string */
        ip.append (QString::fromUtf8 (hexNumber->toHex()));
        ip.append (":");
    }

    /* Remove the last colon and delete hexNumber */
    ip.chop (1);
    free (hexNumber);

    /* If the obtained IP is valid, notify other objects */
    if (QHostAddress (ip).protocol() == QAbstractSocket::IPv6Protocol)
        emit IpFound (host, ip);
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
