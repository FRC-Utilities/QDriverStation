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
 * LIABILITY, WHETHER IN AN ACTION O F CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "Ports.h"
#include "Common.h"
#include "NetworkDiagnostics.h"

DS_NetworkDiagnostics::DS_NetworkDiagnostics()
{
    m_teamNumber = 0;
    m_rioIsAlive = false;
    m_radioIsAlive = false;
    m_customRioAddress = "";
}

bool DS_NetworkDiagnostics::roboRioIsAlive()
{
    return m_rioIsAlive;
}

bool DS_NetworkDiagnostics::robotRadioIsAlive()
{
    return m_radioIsAlive;
}

QString DS_NetworkDiagnostics::radioIpAddress()
{
    return DS_GetStaticIp (m_teamNumber, 1);
}

QString DS_NetworkDiagnostics::roboRioIpAddress()
{
    /* Use the custom address provided by the user */
    if (!m_customRioAddress.isEmpty())
        return m_customRioAddress;

    /* Use the default mDNS name of the roboRIO */
    return QString ("roboRIO-%1.local").arg (m_teamNumber);
}

void DS_NetworkDiagnostics::refresh()
{
    /* Use the values obtained since the last time the function was called */
    m_rioIsAlive = (m_rioSocket.state() == QTcpSocket::ConnectedState);
    m_radioIsAlive = (m_radioSocket.state() == QTcpSocket::ConnectedState);

    /* Connect to roboRIO */
    if (m_rioSocket.state() != QTcpSocket::HostLookupState) {
        m_rioSocket.abort();
        m_rioSocket.connectToHost (roboRioIpAddress(), DS_Ports::RioTest);
    }

    /* Connect to robot radio */
    if (m_radioSocket.state() != QTcpSocket::HostLookupState) {
        m_radioSocket.abort();
        m_radioSocket.connectToHost (radioIpAddress(), DS_Ports::RadioTest);
    }
}

void DS_NetworkDiagnostics::setTeamNumber (const int& team)
{
    m_teamNumber = team;
}

void DS_NetworkDiagnostics::setCustomAddress (const QString& address)
{
    m_customRioAddress = address;
}
