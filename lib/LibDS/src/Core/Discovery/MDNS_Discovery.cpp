/*
 * Copyright (c) 2015-2016 WinT 3794 <http://wint3794.org>
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
#include "LibDS/Core/Discovery/MDNS_Discovery.h"

//=============================================================================
// Class variables
//=============================================================================

#define PORT    5353
#define IPv4_IP QHostAddress ("224.0.0.251")

#define BIND_MODE QAbstractSocket::ShareAddress | QAbstractSocket::ReuseAddressHint

//=============================================================================
// MDNS::MDNS
//=============================================================================

MDNS_Discovery::MDNS_Discovery()
{
    connect (&m_receiver,   &QUdpSocket::readyRead,
             this,          &MDNS_Discovery::readData);
    connect (&m_cacheTimer, &QTimer::timeout,
             this,          &MDNS_Discovery::clearCache);

    m_receiver.bind (QHostAddress::AnyIPv4, PORT, BIND_MODE);

    m_receiver.joinMulticastGroup (IPv4_IP);
    m_receiver.setSocketOption (QAbstractSocket::MulticastTtlOption, 255);
    m_receiver.setSocketOption (QAbstractSocket::MulticastLoopbackOption, 1);

    m_cacheTimer.setInterval (120 * 1000);
    m_cacheTimer.start();
}

//=============================================================================
// MDNS::Query
//=============================================================================

void MDNS_Discovery::query (QString host)
{
    /* The device may support mDNS, try to use the native implementation */
    QHostInfo::lookupHost (host, this, SLOT (lookupFinished (QHostInfo)));

    /* Host is already in the cache, use the IP from there */
    if (m_cache.contains (host))
        emit ipFound (host, m_cache.object (host)->toLower());

    /* Host is not on cache, find it! */
    else
        {
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
            m_sender.writeDatagram (packet, IPv4_IP, PORT);
        }
}

//=============================================================================
// MDNS::ReadData
//=============================================================================

void MDNS_Discovery::readData()
{
    /* Read socket data */
    QByteArray data = DS_GetSocketData (&m_receiver);

    /* The packet is not valid */
    if (data.length() < 12)
        return;

    /* This is not a mDNS response */
    if (data.at (2) != (char) 0x84)
        return;

    /* Try to obtain the IPv4 address, if it fails, this function will
     * call \c GetIPv6Address() to obtain the IPv6 address */
    getIPv4Address (data, getHostName (data));
}

//=============================================================================
// MDNS::ClearCache
//=============================================================================

void MDNS_Discovery::clearCache()
{
    m_cache.clear();
}

//=============================================================================
// MDNS::OnLookupFinished
//=============================================================================

void MDNS_Discovery::lookupFinished (QHostInfo info)
{
    foreach (QHostAddress address, info.addresses())
        {
            if (address.protocol() == QAbstractSocket::AnyIPProtocol)
                {
                    addToCache (info.hostName(), address.toString());
                    emit ipFound (info.hostName(), address.toString());
                    return;
                }
        }
}

//=============================================================================
// MDNS::AddToCache
//=============================================================================

void MDNS_Discovery::addToCache (QString address, QString ip)
{
    if (m_cache.contains (address))
        m_cache.remove (address);

    m_cache.insert (address, new QString (ip));
}

//=============================================================================
// MDNS::GetHostName
//=============================================================================

QString MDNS_Discovery::getHostName (QByteArray data)
{
    int i = 12;
    QString rawData;
    QString address;

    /* The host name starts when the packet flags end (byte #12) and ends
     * with a (obligatory) null character */
    while (data.at (i) != (char) 0x00)
        {
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

void MDNS_Discovery::getIPv4Address (QByteArray data, QString host)
{
    int iterator = 12 + host.length();

    /* We cannot obtain IP of non-existent host */
    if (host.isEmpty())
        return;

    /* Skip IPv4 information until we get to a place with the bytes '00 04',
     * which give us the size of the IP address (which should be always 4) */
    while ((data.at  (iterator + 0) != (char) 0x00) ||
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
        {
            addToCache (host, ip);
            emit ipFound (host, ip);
        }
}
