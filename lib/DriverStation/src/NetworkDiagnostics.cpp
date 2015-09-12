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
    QString team = QString ("%1").arg (m_teamNumber);

    switch (team.length()) {
    case 1:
        team = "00.0" + team;
        break;
    case 2:
        team = "00." + team;
        break;
    case 3:
        team = QString ("0%1.%2%3").arg (team.at (0),
                                         team.at (1),
                                         team.at (2));
        break;
    case 4:
        team = QString ("%1%2.%3%4").arg (team.at (0),
                                          team.at (1),
                                          team.at (2),
                                          team.at (3));
        break;
    }

    return QString ("10.%1.1").arg (team);
}

QString DS_NetworkDiagnostics::roboRioIpAddress()
{
    if (!m_customRioAddress.isEmpty())
        return m_customRioAddress;

    return QString ("roboRIO-%1.local").arg (m_teamNumber);
}

void DS_NetworkDiagnostics::refresh()
{
    m_rioIsAlive = (m_rioSocket.state() == QTcpSocket::ConnectedState);
    m_radioIsAlive = (m_radioSocket.state() == QTcpSocket::ConnectedState);

    if (m_rioSocket.state() != QTcpSocket::HostLookupState) {
        m_rioSocket.abort();
        m_rioSocket.connectToHost (roboRioIpAddress(), 80);
    }

    if (m_radioSocket.state() != QTcpSocket::HostLookupState) {
        m_radioSocket.abort();
        m_radioSocket.connectToHost (radioIpAddress(), 80);
    }
}

void DS_NetworkDiagnostics::setTeamNumber (int team)
{
    m_teamNumber = team;
}

void DS_NetworkDiagnostics::setCustomAddress (QString address)
{
    m_customRioAddress = address;
}
