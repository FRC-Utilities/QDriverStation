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

#include "LibDS/Core/Discovery/Discovery.h"

//=============================================================================
// NetworkDiscovery::NetworkDiscovery
//=============================================================================

NetworkDiscovery::NetworkDiscovery()
{
    connect (&m_responder, &MDNS_Discovery::ipFound,
             this,         &NetworkDiscovery::ipFound);
}

//=============================================================================
// NetworkDiscovery::GetAddressType
//=============================================================================

NetworkDiscovery::AddressType NetworkDiscovery::getAddressType (
    QString address)
{
    if (address.endsWith (".local", Qt::CaseInsensitive))
        return kMDNS;

    if (QHostAddress (address).protocol() == QAbstractSocket::IPv4Protocol)
        return kIPv4;

    if (QHostAddress (address).protocol() == QAbstractSocket::IPv6Protocol)
        return kIPv6;

    return kUnknown;
}

//=============================================================================
// NetworkDiscovery::GetIP
//=============================================================================

void NetworkDiscovery::getIP (QString address)
{
    getIP (address, getAddressType (address));
}

//=============================================================================
// NetworkDiscovery::GetIP
//=============================================================================

void NetworkDiscovery::getIP (QString address, AddressType type)
{
    switch (type)
        {
        case kMDNS:
            m_responder.query (address);
            break;
        case kIPv4:
            emit ipFound (address, address);
            break;
        case kIPv6:
            emit ipFound (address, address);
            break;
        default:
            QHostInfo::lookupHost (address,
                                   this, SLOT (lookupFinished (QHostInfo)));
            break;
        }
}

//=============================================================================
// NetworkDiscovery::OnLookupFinished
//=============================================================================

void NetworkDiscovery::lookupFinished (QHostInfo info)
{
    foreach (QHostAddress address, info.addresses())
        {
            QString ip = address.toString();
            if (getAddressType (ip) == kIPv4 || getAddressType (ip) == kIPv6)
                {
                    emit ipFound (info.hostName(), ip);
                    return;
                }
        }
}
