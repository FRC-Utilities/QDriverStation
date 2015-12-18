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
#ifndef _LIB_DS_NETWORK_RESOLVER_H
#define _LIB_DS_NETWORK_RESOLVER_H

#include <QHostInfo>
#include "LibDS/Core/Discovery/MDNS.h"

/**
 * Allows the library to get the robot IP from a DNS or
 * mDNS address without relying on third-party applications
 * or libraries.
 */
class LIB_DS_DECL NetworkDiscovery : public QObject {
    Q_OBJECT

  public:
    explicit NetworkDiscovery();

    /**
     * Represents the possible types of addresses that we
     * can use to communicate with the robot through a direct
     * connection
     */
    enum AddressType {
        kMDNS,
        kIPv4,
        kIPv6,
        kUnknown
    };

    /**
     * Looks for patterns in the \a address to determine the
     * address type
     */
    AddressType GetAddressType (QString address);

  public slots:
    /**
     * Uses the \c getAddressType() function to 'guess' the \a address
     * type and the steps that we need to take to resolve it.
     */
    void GetIP (QString address);

    /**
     * Resolves the given \a address based on the address \a type.
     */
    void GetIP (QString address, AddressType type);

  signals:
    /**
     * Emitted when the \a ip of the \a address is resolved
     */
    void IpFound (QString address, QString ip);

  private slots:
    /**
     * Gets the available addresses from the \a info object and
     * emits the \c ipFound() signal with the appropiate data
     */
    void OnLookupFinished (QHostInfo info);

  private:
    /**
     * Figures out the IP address of a mDNS domain
     */
    MDNS m_responder;
};

#endif
