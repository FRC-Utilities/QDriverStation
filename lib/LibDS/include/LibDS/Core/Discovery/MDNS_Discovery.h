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

#ifndef _LDS_MDNS_RESPONDER_H
#define _LDS_MDNS_RESPONDER_H

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
class LIB_DS_DECL MDNS_Discovery : public QObject
{
    Q_OBJECT

public:
    explicit MDNS_Discovery();

public slots:
    /**
     * Looks for the given \a host, the class will emit
     * the \c ipFound() \c SIGNAL when the domain is resolved
     */
    void query (QString host);

signals:
    /**
     * Emitted when the \a ip of the \a address is resolved
     */
    void ipFound (QString address, QString ip);

private slots:
    /**
     * Interprets and decodes a response when one of our
     * sockets recieves data
     */
    void readData();

    /**
     * Called periodically to clear the mDNS Address/IP cache
     */
    void clearCache();

    /**
     * Used to get the IP that the computer's native mDNS
     * implementation obtained
     */
    void lookupFinished (QHostInfo info);

    /**
     * Registers the \a address and the \a ip in a list, which
     * will be used to make mDNS lookups faster if the program
     * requests the same \a address again
     */
    void addToCache (QString address, QString ip);

private:
    /**
     * Used to multicast data to the reserved IPv4 address
     */
    QUdpSocket m_sender;

    /**
     * Used to read mDNS responses from the IPv4 multicast address
     */
    QUdpSocket m_receiver;

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
    QString getHostName (QByteArray data);

    /**
     * Extracts the IPv4 address of a mDNS packet and emits the appropiate
     * signals. When finished, this function will call \c GetIPv6Address()
     * if the IPv4 address cannot be obtained.
     */
    void getIPv4Address (QByteArray data, QString host);
};

#endif
