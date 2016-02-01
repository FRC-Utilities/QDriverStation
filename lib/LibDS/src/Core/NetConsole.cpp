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

#include "LibDS/Core/Common.h"
#include "LibDS/Core/NetConsole.h"

//=============================================================================
// DS_NetConsole::DS_NetConsole
//=============================================================================

DS_NetConsole::DS_NetConsole()
{
    m_port = 0;
    m_acceptsInput = false;
    connect (&m_socket, SIGNAL (readyRead()), this, SLOT (readSocket()));
}

//=============================================================================
// DS_NetConsole::setPort
//=============================================================================

void DS_NetConsole::setPort (int port)
{
    m_port = port;
    m_socket.disconnectFromHost();
    m_socket.bind (QHostAddress::Any, m_port, QUdpSocket::ShareAddress);
}

//=============================================================================
// DS_NetConsole::sendCommand
//=============================================================================

void DS_NetConsole::sendCommand (QString command)
{
    if (m_acceptsInput)
        m_socket.writeDatagram (command.toUtf8(), QHostAddress::Any, m_port);
}

//=============================================================================
// DS_NetConsole::setAcceptsInput
//=============================================================================

void DS_NetConsole::setAcceptsInput (bool acceptsInput)
{
    m_acceptsInput = acceptsInput;
}

//=============================================================================
// DS_NetConsole::readSocket
//=============================================================================

void DS_NetConsole::readSocket()
{
    emit newMessage (QString::fromUtf8 (DS_GetSocketData (&m_socket)));
}
