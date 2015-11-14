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

#include "LibDS/DS_Common.h"

#include <QHostInfo>

/**
 * Allows the library to get the robot IP from a DNS or
 * mDNS address without relying on third-party applications
 * or libraries. Well, you will need to have the base networking libraries
 * for your operating system...
 *
 * \warning This class is intended to work for FRC-related configurations,
 *          I strongly discourage you to use it for other purposes, as it may
 *          be incomplete and may not respect some networking standards.
 *          For example, ".local" can be used in both mDNS and 'normal' DNS
 *          services, but this class will asume that ".local" refers to
 *          a mDNS service.
 */
class LIB_DS_DECL NetworkDiscovery : public QObject
{
    Q_OBJECT

public:
    /**
     * Represents the available types of addresses
     * that the user can input to this class
     */
    enum AddressType {
        DNS_Address,
        mDNS_Address,
        IPv4_Address
    };

    explicit NetworkDiscovery ();

    /**
     * Guesses the address type of the inputed \a address
     */
    AddressType getAddressType (QString address);

public slots:
    /**
     * Finds the IP of the \a address
     *
     * \note This function is overloaded, the address type
     *       will be determined automatically
     */
    void getIp (QString address,
                QObject* receiver, const char* member);

    /**
     * Finds the IP of the \a address
     */
    void getIp (QString address, AddressType type,
                QObject* receiver, const char* member);

signals:
    /**
     * Emitted when the \a ip of the \a address is found/resolved
     */
    void ipFound (QString address, QString ip);

private:
    QString m_address;

private slots:
    /**
     * Extracts the IP address from the host \a info
     */
    void onLookupFinished (QHostInfo info);
};

#endif
