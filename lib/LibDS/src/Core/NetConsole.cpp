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

DS_NetConsole::DS_NetConsole (QObject* parent) : QObject (parent)
{
    m_outPort = 0;
    m_acceptsInput = false;
    connect (&m_socket, &QUdpSocket::readyRead,
             this,      &DS_NetConsole::readSocket);
}

//=============================================================================
// DS_NetConsole::setPort
//=============================================================================

void DS_NetConsole::setInputPort (int port)
{
    m_socket.disconnectFromHost();
    m_socket.bind (QHostAddress::Any, port, QUdpSocket::ShareAddress);

    DS_LogMessage (kLibLevel,
                   "NetConsole input port set to: " + QString::number (port));
}

//=============================================================================
// DS_NetConsole::setOutputPort
//=============================================================================

void DS_NetConsole::setOutputPort (int port)
{
    m_outPort = port;
    DS_LogMessage (kLibLevel,
                   "NetConsole output port set to: " + QString::number (port));
}

//=============================================================================
// DS_NetConsole::sendCommand
//=============================================================================

void DS_NetConsole::sendCommand (QString command)
{
    if (m_acceptsInput && m_outPort != DS_PROTOCOL_NO_PORT)
        m_socket.writeDatagram (command.toUtf8(), QHostAddress::Any, m_outPort);
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
