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

#pragma once
#ifndef _LIB_DS_MDNS_RESPONDER_H
#define _LIB_DS_MDNS_RESPONDER_H

#include <QTimer>
#include <QCache>
#include <QHostInfo>

#include "LibDS/Core/Common.h"

/**
 * \class MDNSResponder
 *
 * Implements a rundimentary mDNS lookup process that
 * allows us to get the IP of any mDNS device in the
 * local network.
 */
class LIB_DS_DECL MDNS : public QObject
{
    Q_OBJECT

public:
    explicit MDNS();

public slots:
    /**
     * Looks for the given \a host, the class will emit
     * the \c ipFound() \c SIGNAL when the domain is resolved
     */
    void Query (QString host);

signals:
    /**
     * Emitted when the \a ip of the \a address is resolved
     */
    void IpFound (QString address, QString ip);

private slots:
    /**
     * Sends the \a data to the local network
     */
    void Send (QByteArray data);

    /**
     * Interprets and decodes a response when one of our
     * sockets recieves data
     */
    void ReadData();

    /**
     * Called periodically to clear the mDNS Address/IP cache
     */
    void ClearCache();

    /**
     * Registers the \a address and the \a ip in a list, which
     * will be used to make mDNS lookups faster if the program
     * requests the same \a address again
     */
    void AddToCache (QString address, QString ip);

    /**
     * Gets the available addresses from the \a info object and
     * emits the \c ipFound() signal with the appropiate data.
     *
     * This is only called if the system already has a mDNS
     * service installed
     */
    void OnLookupFinished (QHostInfo info);

private:
    /**
     * Used to multicast data to the reserved IPv4 address
     */
    QUdpSocket m_IPv4_sender;

    /**
     * Used to multicast data to the reserved IPv6 address
     */
    QUdpSocket m_IPv6_sender;

    /**
     * Used to read mDNS responses from the IPv4 multicast address
     */
    QUdpSocket m_IPv4_receiver;

    /**
     * Used to read mDNS responses from the IPv6 multicast address
     */
    QUdpSocket m_IPv6_receiver;

    /**
     * Used to store the host names and IP information of the received mDNS packets
     */
    QCache<QString, QString> m_cache;

    /**
     * Clears the cache periodically
     */
    QTimer m_cacheTimer;

    /**
     * Extracts the host name from the \a data of a response packet
     */
    QString GetHostName (QByteArray data);

    /**
     * Extracts the IPv4 address of a mDNS packet and emits the appropiate
     * signals. When finished, this function will call \c GetIPv6Address()
     * if the IPv4 address cannot be obtained.
     */
    void GetIPv4Address (QByteArray data, QString host);

    /**
     * Extracts the IPv6 address of a mDNS packet and emits the appropiate
     * signals.s
     */
    void GetIPv6Address (QByteArray data, QString host, int iterator);

    /**
     * Reads the data of the IPv4 or IPv6 receiver socket based on the
     * object name of the \a caller object.
     */
    QByteArray GetSocketData (QObject* caller);
};

#endif
