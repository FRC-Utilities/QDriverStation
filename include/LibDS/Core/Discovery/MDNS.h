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

#include "LibDS/Core/Common.h"

/**
 * \class MDNSResponder
 *
 * Implements a rundimentary mDNS lookup process that
 * allows us to get the IP of any mDNS device in the
 * local network.
 */
class LIB_DS_DECL MDNS : public QObject {
    Q_OBJECT

  public:
    explicit MDNS();
    ~MDNS();

  public slots:
    /**
     * Looks for the given \a domain, the class will emit
     * the \c ipFound() \c SIGNAL when the domain is resolved
     */
    void query (QString domain);

  signals:
    /**
     * Emitted when the \a ip of the \a address is resolved
     */
    void ipFound (QString address, QString ip);

  private slots:
    /**
     * Reads the data received on the IPv4 socket
     */
    void readIPv4Socket();

    /**
     * Reads the data received on the IPv6 socket
     */
    void readIPv6Socket();

    /**
     * Sends the \a data to the local network
     */
    void send (QByteArray data);

    /**
     * Interprets and decodes the \a response
     */
    void processResponse (QByteArray response);

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
};

#endif
