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

#include <QTimer>
#include <QUdpSocket>

#include "NetConsole.h"

/* The ports used for the NetConsole */
#define _UDP_IN_PORT 6666
#define _UDP_OUT_PORT 6668

NetConsole* NetConsole::m_instance = nullptr;

NetConsole::NetConsole()
{
    m_inSocket = new QUdpSocket (this);
    m_outSocket = new QUdpSocket (this);
    connect (m_inSocket, SIGNAL (readyRead()), this, SLOT (onMessageReceived()));
}

NetConsole* NetConsole::getInstance()
{
    if (m_instance == nullptr)
        m_instance = new NetConsole();

    return m_instance;
}

void NetConsole::init()
{
    m_address.setAddress ("255.255.255.255");
    m_inSocket->bind (m_address, _UDP_IN_PORT);
    m_outSocket->bind (m_address, _UDP_OUT_PORT);

    emit newMessage ("INFO: Welcome to the QDriverStation!");
    emit newMessage ("");
}

void NetConsole::onMessageReceived()
{
    while (m_inSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize (m_inSocket->pendingDatagramSize());
        m_inSocket->readDatagram (datagram.data(), datagram.size());

        emit newMessage (QString::fromUtf8 (datagram));
    }
}
