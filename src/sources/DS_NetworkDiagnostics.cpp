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

#include "../headers/DS_NetworkDiagnostics.h"

DS_NetworkDiagnostics::DS_NetworkDiagnostics() {
    m_robotAddress = "";
    m_radioAddress = "";
    m_robotIsAlive = false;
    m_radioIsAlive = false;
}

void DS_NetworkDiagnostics::refresh() {
    m_robotIsAlive = (m_robotSocket.state() == QTcpSocket::ConnectedState);
    m_radioIsAlive = (m_radioSocket.state() == QTcpSocket::ConnectedState);

    if (m_robotSocket.state() != QTcpSocket::HostLookupState) {
        m_robotSocket.abort();
        m_robotSocket.connectToHost (m_robotAddress, 80);
    }

    if (m_radioSocket.state() != QTcpSocket::HostLookupState) {
        m_radioSocket.abort();
        m_radioSocket.connectToHost (m_radioAddress, 80);
    }
}

bool DS_NetworkDiagnostics::robotIsAlive() {
    return m_robotIsAlive;
}

bool DS_NetworkDiagnostics::radioIsAlive() {
    return m_radioIsAlive;
}

void DS_NetworkDiagnostics::setRadioAddress (QString address) {
    m_radioAddress = address;
}

void DS_NetworkDiagnostics::setRobotAddress (QString address) {
    m_robotAddress = address;
}
