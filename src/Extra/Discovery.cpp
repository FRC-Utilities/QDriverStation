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

#include "Extra/Discovery.h"

NetworkDiscovery::NetworkDiscovery()
{
    m_address = "";
}

NetworkDiscovery::AddressType NetworkDiscovery::getAddressType (QString address)
{
    /* Address ends with ".local", treat it as a mDNS address */
    if (address.endsWith (".local", Qt::CaseInsensitive))
        return mDNS_Address;

    /* Address has 4 dots and respects the character length range of an IPv4 address*/
    else if (address.split (".").count() == 4
             && address.length() <= 15
             && address.length() >= 7)
        return IPv4_Address;

    /* Something different */
    return DNS_Address;
}

void NetworkDiscovery::getIp (QString address,
                              QObject* receiver, const char* member)
{
    getIp (address, getAddressType (address), receiver, member);
}

void NetworkDiscovery::getIp (QString address, AddressType type,
                              QObject* receiver, const char* member)
{
    /* Notify caller when we receive a response */
    QObject::connect (this, SIGNAL (ipFound (QString, QString)), receiver, member);

    /* Target host is a mDNS service */
    if (type == mDNS_Address) {
        m_address = address;

        /* TODO:
         *
         * Implement our own way to detect the IP of a mDNS address
         * without using third-party libraries, as Qt does.
         *
         * REASON:
         *
         * - Third party libraries have some issues with Qt.
         *   For example, they may change "roboRIO-XXYY.local" to
         *   "roborio-XXYY.local" and (sometimes) this may cause a failure.
         *   I have seen this behaviour with Mac OS X 10.11
         * - mDNS is not available on Windows, this can be solved by
         *   downloading and installing Bonjour, but this is an additional step
         *   for the users
         * - mDNS is not available on Android, there is no easy way to implement
         *   a Android library with Qt...it would be easier to reinvent the wheel
         *   and provide our own way to use mDNS
         * - I haven't tested on Windows Phone, but my hopes for it to support
         *   mDNS are pretty low
         */
        QHostInfo::lookupHost (address,
                               this, SLOT (onLookupFinished (QHostInfo)));
    }

    /* Target host is an IP address, we can relax a little bit! */
    else if (type == IPv4_Address)
        emit ipFound (address, address);

    /* Target host is a normal DNS address, let Qt do the hard work for us */
    else {
        m_address = address;
        QHostInfo::lookupHost (address,
                               this, SLOT (onLookupFinished (QHostInfo)));
    }
}

void NetworkDiscovery::onLookupFinished (QHostInfo info)
{
    /* The response was invalid or we haven't set a target */
    if (info.addresses().isEmpty() || m_address.isEmpty())
        return;

    /* Iterate to the address list until we find an IPv4 address */
    for (int i = 0; i < info.addresses().count(); ++i) {
        QString ip = info.addresses().at (i).toString();

        if (getAddressType (ip) == IPv4_Address) {
            emit ipFound (m_address, ip);

            m_address = "";
            return;
        }
    }
}
