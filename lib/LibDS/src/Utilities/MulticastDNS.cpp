/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#include "MulticastDNS.h"

const quint16 MDNS_PORT = 5353;
const QHostAddress IPV6_ADDRESS = QHostAddress ("FF02::FB");
const QHostAddress IPV4_ADDRESS = QHostAddress ("224.0.0.251");

enum GeneralFlags {
    kFormatId = 0x00,
};

enum QueryFlags {
    kQueryFlag    = 0x00,
    kQueryQTYPE   = 0x01,
    kQueryQCLASS  = 0x01,
    kQueryQDCOUNT = 0x01,
    kQueryANCOUNT = 0x00,
    kQueryNSCOUNT = 0x00,
    kQueryARCOUNT = 0x00,
};

enum ResponseFlags {
    kResponseFlag = 0x84,
    kResponseClassCodeA = 0x80,
    kResponseClassCodeB = 0x01,
    kResponseIPv4TypeCodeA = 0x00,
    kResponseIPv4TypeCodeB = 0x01,
    kResponseIPv6TypeCodeA = 0x00,
    kResponseIPv6TypeCodeB = 0x1C,
};

/**
 * Formats the given \a number as a 16-bit type (specified in two bytes)
 */
QByteArray TWO_BYTES (qint16 number) {
    QByteArray data;
    data.append ((number & 0xff00) >> 8);
    data.append ((number & 0xff));
    return data;
}

/**
 * Represents the flags that the UDP socket uses to 'listen' for incoming
 * mDNS Packets.
 */
const QUdpSocket::BindMode BIND_MODE = QUdpSocket::ShareAddress |
                                       QUdpSocket::ReuseAddressHint;

MulticastDNS::MulticastDNS() {
    m_IPv4Socket.bind (QHostAddress::AnyIPv4, MDNS_PORT, BIND_MODE);
    m_IPv6Socket.bind (QHostAddress::AnyIPv6, MDNS_PORT, BIND_MODE);
    m_IPv4Socket.joinMulticastGroup (IPV4_ADDRESS);
    m_IPv6Socket.joinMulticastGroup (IPV6_ADDRESS);

    connect (&m_IPv4Socket, SIGNAL (readyRead()), this, SLOT (onReadyRead()));
    connect (&m_IPv6Socket, SIGNAL (readyRead()), this, SLOT (onReadyRead()));
}

/**
 * Performs a mDNS lookup to find the IP address of the given host \a name.
 *
 * \note If the \c alreadySupported() function returns \c true, then this class
 *       will use the native implementation of the OS to find the given host
 *       \a name.
 * \note If the \c alreadySupported() function returns \c false, then this class
 *       will perform the mDNS lookup by itself using Qt socket programming.
 */
void MulticastDNS::lookup (const QString& name) {
    if (!name.isEmpty()) {
        QByteArray data;

        /* Ensure that the domain is valid */
        QString host = name;
        host = host.replace (".local.", "");
        host = host.replace (".local",  "");

        /* Check that domain length is valid */
        if (host.length() > 255)
            return;

        /* Create header & flags */
        data.append (TWO_BYTES (kFormatId));
        data.append (TWO_BYTES (kQueryFlag));
        data.append (TWO_BYTES (kQueryQDCOUNT));
        data.append (TWO_BYTES (kQueryANCOUNT));
        data.append (TWO_BYTES (kQueryNSCOUNT));
        data.append (TWO_BYTES (kQueryARCOUNT));

        /* Add domain data */
        data.append (host.length());
        data.append (host.toUtf8());

        /* Add '.local' section */
        QString local = "local";
        data.append (local.length());
        data.append (local.toUtf8());
        data.append ((qint8) 0x00);
        data.append (TWO_BYTES (kQueryQTYPE));
        data.append (TWO_BYTES (kQueryQCLASS));

        /* Send the datagram */
        m_IPv4Socket.writeDatagram (data, IPV4_ADDRESS, MDNS_PORT);
        m_IPv6Socket.writeDatagram (data, IPV6_ADDRESS, MDNS_PORT);
    }
}

/**
 * Called when we receive data from a mDNS service on the network.
 * \note We will only take into account mDNS response packets, not queries.
 */
void MulticastDNS::onReadyRead() {
    QByteArray data;
    QUdpSocket* socket = qobject_cast<QUdpSocket*> (sender());

    /* Yay! socket is valid! Get the received data */
    if (socket) {
        while (socket->hasPendingDatagrams()) {
            data.resize (socket->pendingDatagramSize());
            socket->readDatagram (data.data(), data.size());
        }
    }

    /* Something bad happened, at least we won't crash */
    if (data.length() < 13)
        return;

    /* Only take into account response packets */
    quint8 flag = data.at (2);
    if (flag == (quint8) kResponseFlag) {
        QString host = getHost (data);
        QList<QHostAddress> addresses = getAddresses (data, host);

        if (!host.isEmpty() && !addresses.isEmpty()) {
            QHostInfo info;
            info.setHostName (host);
            info.setAddresses (addresses);
            info.setError (QHostInfo::NoError);

            emit finished (info);
        }
    }
}

/**
 * Extracts the host name from the \a data received from the mDNS network.
 * The host name begins at byte #12 (when the header and flags end) and ends
 * with a mandatory NUL character after the domain.
 *
 * The host name is constructed in the following way (without spaces):
 * \c NAME_LENGTH + \c NAME + \c DOMAIN_LENGTH + \c DOMAIN + \c NUL
 *
 * For example, appletv.local would be formatted as:
 * \c 0x07 + \c appletv + \c 0x05 + \c local + \c 0x00
 *
 * Or, if you prefer hex data:
 * \c { 07 61 70 70 6c 65 74 76 05 6c 6f 63 61 6c 00 }
 * \c {  7 a  p  p  l  e  t  v   5 l  o  c  a  l   0 }
 *
 * In order to obtain the full host name (and its mDNS domain), we construct
 * the string backwards. When the code notices that the current character is
 * the same as the domain length, we know that the domain name has been
 * extracted, and thus we can replace the domain length with a dot (.) and
 * begin extracting the host name.
 */
QString MulticastDNS::getHost (const QByteArray& data) {
    QList<char> list;
    QString address = "";

    /* Begin reading host name at byte 13 (byte 12 is the host name length) */
    int n = 13;

    /* Read the host name until we stumble with the null character */
    while (data.at (n) != (char) 0x00) {
        list.append (data.at (n));
        ++n;
    }

    /* Construct the string backwards (to replace domain length with a dot) */
    for (int i = 0; i < list.count(); ++i) {
        char character = list.at (list.count() - i - 1);

        if (character == (char) address.length())
            address.prepend (".");
        else
            address.prepend (character);
    }

    return address;
}

/**
 * Extracts the IPv4 from the \a data received from the mDNS network.
 * The IPv4 data begins when the host name data ends. We can know if the IPv4
 * information is present in the packet by looking at the type codes, which
 * are the first two bytes after the host name data.
 *
 * For the packet to contain IPv4 information, the type codes must be
 * \c 0x00 and \c 0x01 respectively. After the type codes, we have two bytes
 * that represent the IP class codes, the following four bytes contain the TTL
 * (which we do not use FYI). After the TTL, we have a byte that represents
 * the number of bytes that construct the IPv4 address. After that, we finally
 * have the address bytes (which we can extract if we know the IPv4 length,
 * which is usually 4).
 *
 * Here is the 'layout' of the IPv4 section of the packet:
 *
 * - Type codes: \c {0x00 0x01} (used for verification)
 * - Class Codes: \c {0x80 0x01} (used for verification)
 * - TTL (four bytes)
 * - IP length (one byte)
 * - IP address bytes (depend on length)
 *
 * This is an example IPv4 section:
 * \c {00 01 80 01 00 00 78 00 00 04 99 6d 07 5a}
 *
 * Data in example section:
 * - \c {00 01} Type Codes
 * - \c {80 01} Class Codes
 * - \c {00 00 78 00 00} IP TTL
 * - \c {04} Number of address bytes (length in layman's terms)
 * - \c {99 6d 07 5a} IPv4 Address bytes (153, 109, 7, 90)
 */
QString MulticastDNS::getIPv4 (const QByteArray& data, const QString& host) {
    QString ip = "";

    /* n stands for the byte index in which the host name data ends */
    int n = 13 + host.length();

    /* Check if the packet contains the class codes */
    if (data.length() < n + 4)
        return ip;

    /* Get the IP type and class code from the packet */
    quint8 typeA  = data.at (n + 1);
    quint8 typeB  = data.at (n + 2);
    quint8 classA = data.at (n + 3);
    quint8 classB = data.at (n + 4);

    /* Check if type codes are good */
    if (typeA != kResponseIPv4TypeCodeA || typeB != kResponseIPv4TypeCodeB)
        return ip;

    /* Check if class codes are good */
    if (classA != kResponseClassCodeA || classB != kResponseClassCodeB)
        return ip;

    /* Obtain the IPv4 if the packet is valid */
    else if (data.length() > (n + 10 + 4)) {
        /* Skip TTL indicator and obtain the number of address bytes */
        quint8 length = data.at (n + 10);

        /* Append each IPv4 address byte (and decimal dots) to the IP string */
        for (int i = 1; i < length + 1; ++i) {
            ip += QString::number ((quint8) data.at (n + 10 + i));
            ip += (i < length) ? "." : "";
        }
    }

    /* Tada! */
    return ip;
}

/**
 * Extracts the IPv6 from the \a data received from the mDNS network.
 * The IPv6 data begins when the IPv4 data and the two-byte FQDN ends.
 * As with the IPv4 code, we can know if the IPv6 information is present in the
 * packet by looking at the type codes, which are the first two bytes after
 * the IPv4 and FQDN data.
 *
 * For the packet to contain IPv6 information, the type codes must be
 * \c 0x00 and \c 0x1C respectively. After the type codes, we have two bytes
 * that represent the IP class codes, the following four bytes contain the TTL
 * (which we do not use FYI). After the TTL, we have a byte that represents
 * the number of bytes that construct the IPv6 address.
 * After that, we finally have the address bytes (which we can extract if we
 * know the IPv6 length, which is usually 16).
 *
 * Here is the 'layout' of the IPv4 section of the packet:
 *
 * - Type codes: \c {0x00 0xC1} (used for verification)
 * - Class Codes: \c {0x80 0x01} (used for verification)
 * - TTL (four bytes)
 * - IP length (one byte)
 * - IP address bytes (depend on length, usually 16)
 *
 * This is an example IPv6 section:
 * \c { 00 1c 80 01 00 00 78 00 00 10 fe 80 00 }
 * \c { 00 00 00 00 00 02 23 32 ff fe b1 21 52 }
 *
 * Data in example section:
 * - \c {00 1c} Type Codes
 * - \c {80 01} Class Codes
 * - \c {00 00 78 00 00} IP TTL
 * - \c {10} Number of address bytes (length in layman's terms)
 * - \c {fe 80 00 00 ... 52} IPv6 Address bytes (there are 16 of them)
 */
QString MulticastDNS::getIPv6 (const QByteArray& data,
                               const QString& host,
                               const QString& ipv4) {
    QString ip = "";

    /* n stands for the byte index in which the IPv4 data ends */
    int n = 13 + host.length() + 10 + ipv4.split (".").count();

    /* Skip the FQDN offset */
    n += 2;

    /* Check if the packet contains the class codes */
    if (data.length() < n + 4)
        return ip;

    /* Get the IP type and class codes */
    quint8 typeA  = data.at (n + 1);
    quint8 typeB  = data.at (n + 2);
    quint8 classA = data.at (n + 3);
    quint8 classB = data.at (n + 4);

    /* Check if type codes are good */
    if (typeA != kResponseIPv6TypeCodeA || typeB != kResponseIPv6TypeCodeB)
        return ip;

    /* Check if class codes are good */
    if (classA != kResponseClassCodeA || classB != kResponseClassCodeB)
        return ip;

    /* Obtain the IPv6 if the packet is valid */
    else if (data.length() > (n + 10 + 16)) {
        /* Skip TTL indicator and obtain the number of address bytes */
        quint8 length = data.at (n + 10);

        /* Append each IPv6 address byte (encoded as hex) to the IP string */
        for (int i = 1; i < length + 1; ++i) {
            /* Get the hexadecimal representation of the byte */
            QString byte;
            byte.setNum ((quint8) data.at (n + 10 + i), 16);

            /* Add the obtained string */
            ip += byte;

            /* Append colons after even indexes (when we are not last byte) */
            if ((i & 1) == 0 && (i < length))
                ip += ":";
        }
    }

    return ip;
}

/**
 * Obtains the IPv4 and IPv6 addresses from the received data.
 * \note This function will only generate a list with the valid IP addresses.
 */
QList<QHostAddress> MulticastDNS::getAddresses (const QByteArray& data,
                                                const QString& host) {
    QList<QHostAddress> list;
    QString ipv4 = getIPv4 (data, host);
    QString ipv6 = getIPv6 (data, host, ipv4);

    QHostAddress IPv4Address = QHostAddress (ipv4);
    QHostAddress IPv6Address = QHostAddress (ipv6);

    if (!IPv4Address.isNull())
        list.append (IPv4Address);

    if (!IPv6Address.isNull())
        list.append (IPv6Address);

    return list;
}
