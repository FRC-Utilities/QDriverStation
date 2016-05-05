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

#ifndef _LDS_NETWORK_SCANNER_H
#define _LDS_NETWORK_SCANNER_H

#include "LibDS/Core/Common.h"

namespace DS_Core {

///
/// The \c SocketManager class allows us to send and detect responses from
/// the robot controller with a given IP list, send and receive robot packets
/// and send and receive FMS packets.
///
/// The idea behind this is to use a multiple I/O socket pairs in order to
/// make the network scanning process much faster and efficient than if we
/// were to use a single I/O socket pair to scan the network.
///
class SocketManager : public QObject {
    Q_OBJECT

  public:
    explicit SocketManager();

    ///
    /// Returns the current number of parallel socket pairs scanning
    /// the network for the robot controller
    ///
    int scannerCount() const;

    ///
    /// Returns the current robot address
    ///
    QString robotAddress() const;

  public slots:
    ///
    /// Configures the socket pairs in order to scan the next round
    /// of IP addresses
    ///
    void refreshIPs();

    ///
    /// Reconfigures the FMS receiver socket to use the given \a port
    ///
    void setFmsInputPort (int port, QAbstractSocket::BindMode flags);

    ///
    /// Reconfigures the FMS output socket to send data to the
    /// selected \a port
    ///
    void setFmsOutputPort (int port);

    ///
    /// Configures the scanner to read incomming data in the given \a port
    ///
    void setRobotInputPort (int port, QAbstractSocket::BindMode flags);

    ///
    /// Configures the scanner to send data to the given \a port
    ///
    void setRobotOutputPort (int port);

    ///
    /// Changes the address in which we send data to the robot.
    /// If the value is left blank, the automatic network scanner will
    /// be enabled and will change the robot address as needed
    ///
    void setRobotAddress (QString address);

    ///
    /// Sends the given \a data to the FMS server
    ///
    void sendFmsPacket (QByteArray data);

    ///
    /// Sends the given \a data to the robot.
    /// If the network scanner is active and we don't have communications
    /// with the robot, the \a data will be sent to multiple IO sockets
    ///
    void sendRobotPacket (QByteArray data);

    ///
    /// Changes the list of possible IPs that the robot may have
    ///
    void setRobotIPs (const QStringList& list);

  private slots:
    void readFmsPacket();
    void readRobotPacket();
    void setScannerCount (int scanner_count);

  signals:
    void fmsPacketReceived (QByteArray data);
    void robotPacketReceived (QByteArray data);

  private:
    int m_iterator;
    int m_fmsOutput;
    int m_robotInput;
    int m_robotOutput;

    QString m_robotAddress;

    int m_scannerCount;
    QStringList m_list;

    QAbstractSocket::BindMode m_robotInputFlags;

    QUdpSocket m_fmsInputSocket;
    QUdpSocket m_fmsOutputSocket;
    QUdpSocket m_robotInputSocket;
    QUdpSocket m_robotOutputSocket;

    QList<QUdpSocket*> m_inputSockets;
    QList<QUdpSocket*> m_outputSockets;
};

}

#endif
