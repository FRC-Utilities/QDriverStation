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

#include "LibDS/Core/NetworkScanner.h"

using namespace DS_Core;

//==================================================================================================
// NetworkScanner::NetworkScanner
//==================================================================================================

NetworkScanner::NetworkScanner() {
    m_input = 0;
    m_output = 0;
    m_enabled = 0;
    m_scannerCount = 0;
}

//==================================================================================================
// NetworkScanner::inputPort
//==================================================================================================

int NetworkScanner::inputPort() const {
    return m_input;
}

//==================================================================================================
// NetworkScanner::outputPort
//==================================================================================================

int NetworkScanner::outputPort() const {
    return m_output;
}

//==================================================================================================
// NetworkScanner::scannerCount
//==================================================================================================

bool NetworkScanner::isEnabled() const {
    return m_enabled;
}

//==================================================================================================
// NetworkScanner::scannerCount
//==================================================================================================

int NetworkScanner::scannerCount() const {
    return m_scannerCount;
}

//==================================================================================================
// NetworkScanner::update
//==================================================================================================

void NetworkScanner::update() {
    if (!isEnabled())
        return;

    if (m_list.count() > m_iterator + scannerCount())
        m_iterator += scannerCount();
    else
        m_iterator = 0;

    for (int i = 0; i < m_inputSockets.count(); ++i) {
        if (scannerCount() > i && m_list.count() > m_iterator + i) {
            m_inputSockets.at (i)->disconnectFromHost();
            m_inputSockets.at (i)->bind (QHostAddress (m_list.at (m_iterator + i)),
                                         m_input, QUdpSocket::ShareAddress);
        }
    }
}

//==================================================================================================
// NetworkScanner::setInputPort
//==================================================================================================

void NetworkScanner::setInputPort (int port) {
    m_input = port;
}

//==================================================================================================
// NetworkScanner::setOutputPort
//==================================================================================================

void NetworkScanner::setOutputPort (int port) {
    m_output = port;
}

//==================================================================================================
// NetworkScanner::setEnabled
//==================================================================================================

void NetworkScanner::setEnabled (bool enabled) {
    m_enabled = enabled;
}

//==================================================================================================
// NetworkScanner::sendData
//==================================================================================================

void NetworkScanner::sendData (QByteArray data) {
    if (!isEnabled())
        return;

    for (int i = 0; i < m_outputSockets.count(); ++i) {
        if (scannerCount() > i && m_list.count() > m_iterator + i) {
            m_outputSockets.at (i)->writeDatagram (data,
                                                   QHostAddress (m_list.at (m_iterator + i)),
                                                   m_output);
        }
    }
}

//==================================================================================================
// NetworkScanner::setScannerOutput
//==================================================================================================

void NetworkScanner::setScannerCount (int count) {
    if (count >= 0 && count != scannerCount()) {
        m_scannerCount = count;
        m_inputSockets.clear();
        m_outputSockets.clear();

        foreach (QUdpSocket* socket, findChildren<QUdpSocket*>())
            delete socket;

        for (int i = 0; i < scannerCount(); ++i) {
            QUdpSocket* input = new QUdpSocket (this);
            QUdpSocket* output = new QUdpSocket (this);

            input->setObjectName (QString::number (i));
            output->setObjectName (QString::number (i));
            input->setSocketOption (QAbstractSocket::MulticastLoopbackOption, 0);

            connect (input, SIGNAL (readyRead()), this, SLOT (readInputData()));

            m_inputSockets.append (input);
            m_outputSockets.append (output);
        }
    }
}

//==================================================================================================
// NetworkScanner::setScanningList
//==================================================================================================

void NetworkScanner::setScanningList (const QStringList& list) {
    m_list = list;
    m_iterator = 0;

    setScannerCount (list.count() / 8);
}

//==================================================================================================
// NetworkScanner::readInputData
//==================================================================================================

void NetworkScanner::readInputData() {
    QUdpSocket* socket = qobject_cast<QUdpSocket*> (sender());
    int socket_id = socket->objectName().toInt();

    if (scannerCount() > socket_id) {
        QByteArray data;
        data.resize (socket->pendingDatagramSize());
        socket->readDatagram (data.data(), data.size());

        setEnabled (false);
        emit dataReceived (m_list.at (socket_id + m_iterator + 1), data);
    }
}
