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

#include "LibDS/Core/SocketManager.h"

using namespace DS_Core;

//==================================================================================================
// SocketManager::SocketManager
//==================================================================================================

SocketManager::SocketManager() {
    m_fmsOutput = 0;
    m_robotInput = 0;
    m_robotOutput = 0;
    m_scannerCount = 0;
    m_robotAddress = "";
    m_robotInputFlags = QAbstractSocket::ShareAddress | QAbstractSocket::ReuseAddressHint;

    connect (&m_fmsInputSocket, SIGNAL (readyRead()), this, SLOT (readFmsPacket()));
}

//==================================================================================================
// SocketManager::scannerCount
//==================================================================================================

int SocketManager::scannerCount() const {
    return m_scannerCount;
}

//==================================================================================================
// SocketManager::robotAddress
//==================================================================================================

QString SocketManager::robotAddress() const {
    return m_robotAddress;
}

//==================================================================================================
// SocketManager::refreshIPs
//==================================================================================================

void SocketManager::refreshIPs() {
    if (robotAddress().isEmpty()) {
        if (m_list.count() > m_iterator + scannerCount())
            m_iterator += scannerCount();

        else
            m_iterator = 0;

        for (int i = 0; i < m_inputSockets.count(); ++i) {

            if (scannerCount() > i && m_list.count() > m_iterator + i) {
                m_inputSockets.at (i)->disconnectFromHost();
                m_inputSockets.at (i)->bind (QHostAddress (m_list.at (m_iterator + i)),
                                             m_robotInput, m_robotInputFlags);
            }
        }
    }
}

//==================================================================================================
// SocketManager::setFmsInputPort
//==================================================================================================

void SocketManager::setFmsInputPort (int port, QAbstractSocket::BindMode flags) {
    m_fmsInputSocket.bind (QHostAddress::Any, port, flags);
    m_fmsInputSocket.setSocketOption (QAbstractSocket::MulticastLoopbackOption, 0);
}

//==================================================================================================
// SocketManager::setFmsOutputPort
//==================================================================================================

void SocketManager::setFmsOutputPort (int port) {
    m_fmsOutput = port;
}

//==================================================================================================
// SocketManager::setRobotInputPort
//==================================================================================================

void SocketManager::setRobotInputPort (int port, QAbstractSocket::BindMode flags) {
    m_robotInput = port;
    m_robotInputFlags = flags;
}

//==================================================================================================
// SocketManager::setRobotOutputPort
//==================================================================================================

void SocketManager::setRobotOutputPort (int port) {
    m_robotOutput = port;
}

//==================================================================================================
// SocketManager::setRobotAddress
//==================================================================================================

void SocketManager::setRobotAddress (QString address) {
    m_robotAddress = address;
}

//==================================================================================================
// SocketManager::sendFmsPacket
//==================================================================================================

void SocketManager::sendFmsPacket (QByteArray data) {
    if (!data.isEmpty())
        m_fmsOutputSocket.writeDatagram (data, QHostAddress::Any, m_fmsOutput);
}

//==================================================================================================
// SocketManager::sendRobotPacket
//==================================================================================================

void SocketManager::sendRobotPacket (QByteArray data) {
    if (data.isEmpty())
        return;

    if (!robotAddress().isEmpty())
        m_robotOutputSocket.writeDatagram (data, QHostAddress (robotAddress()), m_robotOutput);

    else {
        for (int i = 0; i < m_outputSockets.count(); ++i) {
            if (scannerCount() > i && m_list.count() > m_iterator + i) {
                QHostAddress address = QHostAddress (m_list.at (m_iterator + i));
                m_outputSockets.at (i)->writeDatagram (data, address, m_robotOutput);
            }
        }
    }
}

//==================================================================================================
// SocketManager::setScanningList
//==================================================================================================

void SocketManager::setRobotIPs (const QStringList& list) {
    /* Invalid list, get out of here */
    if (list.isEmpty())
        return;

    /* Update internal values */
    m_list = list;
    m_iterator = 0;

    /* Adjust the scanner count */
    setScannerCount (m_list.count() / 6);
}

//==================================================================================================
// SocketManager::readFmsPacket
//==================================================================================================

void SocketManager::readFmsPacket() {
    emit fmsPacketReceived (DS::readSocket (&m_fmsInputSocket));
}

//==================================================================================================
// SocketManager::readRobotPacket
//==================================================================================================

void SocketManager::readRobotPacket() {
    /* Get the socket information */
    QUdpSocket* socket = qobject_cast<QUdpSocket*> (sender());
    QByteArray data = DS::readSocket (qobject_cast<QUdpSocket*> (sender()));

    /* This is the first packet, configure the permanent socket */
    if (robotAddress().isEmpty() && !data.isEmpty())
        setRobotAddress (socket->peerAddress().toString());

    /* Tell the protocol to read the packet */
    if (!data.isEmpty())
        emit robotPacketReceived (data);
}

//==================================================================================================
// SocketManager::setScannerCount
//==================================================================================================

void SocketManager::setScannerCount (int count) {
    if (count < 1)
        count = 1;

    if (count >= 0 && count != scannerCount()) {
        m_scannerCount = count;
        m_inputSockets.clear();
        m_outputSockets.clear();

        foreach (QUdpSocket* socket, findChildren<QUdpSocket*>())
            delete socket;

        for (int i = 0; i < scannerCount(); ++i) {
            QUdpSocket* input = new QUdpSocket (this);
            QUdpSocket* output = new QUdpSocket (this);

            m_inputSockets.append (input);
            m_outputSockets.append (output);

            input->setSocketOption (QAbstractSocket::MulticastLoopbackOption, 0);
            connect (input, SIGNAL (readyRead()), this, SLOT (readRobotPacket()));
        }
    }
}
