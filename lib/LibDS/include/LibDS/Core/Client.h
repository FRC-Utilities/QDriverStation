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

#ifndef _LDS_CLIENT_H
#define _LDS_CLIENT_H

#include "LibDS/Core/Common.h"

namespace DS_Core {

///
/// Sends and receives data between the robot, the DS and the FMS.
/// The class redirects the recieved data from the robot
/// and the FMS to the current protocol in use and vice-versa.
/// In other words, all networking operations of the LibDS are
/// handled by this class.
///
/// Please note that the robot ports use the UDP protocol, while the
/// FMS ports use the TCP protocol.
///
class LIB_DS_DECL Client : public QObject {
    Q_OBJECT

  public slots:
    ///
    /// Sends the input \a data to the FMS server.
    /// This function is called periodically based on the timing frequency
    /// specified by the current protocol.
    ///
    void sendToFms (QByteArray data);

    ///
    /// Sends the input \a data to the robot controller.
    /// This function is called periodically based on the timing frequency
    /// specified by the current protocol.
    ///
    void sendToRobot (QByteArray data);

    ///
    /// Changes the local port in which we recieve FMS packets.
    /// This function is called when a protocol is loaded by the DS.
    ///
    void setFmsInputPort (int port);

    ///
    /// Changes the remote port in which we send FMS packets.
    /// This function is called when a protocol is loaded by the DS.
    ///
    void setFmsOutputPort (int port);

    ///
    /// Changes the local port in which we recieve robot packets.
    /// This function is called when a protocol is loaded by the DS.
    ///
    void setRobotInputPort (int port);

    ///
    /// Changes the remote port in which we send robot packets.
    /// This function is called when a protocol is loaded by the DS.
    ///
    void setRobotOutputPort (int port);

    ///
    /// Changes the robot address and updates the appropiate socket
    /// configurations.
    ///
    void setRobotAddress (QString address);

  signals:
    ///
    /// Emitted when the local computer recieves a packet from the FMS
    /// The data is sent to the current protocol for interpretation.
    ///
    void fmsPacketReceived (QByteArray);

    ///
    /// Emitted when the local computer recieves a packet from the robot.
    /// The data is sent to the current protocol for interpretation.
    ///
    void robotPacketReceived (QByteArray);

  public:
    explicit Client (QObject* parent);

  private slots:
    void readFmsPacket();
    void readRobotPacket();

  private:
    int m_fmsPort;
    int m_robotPort;
    QString m_robotAddress;
    QTcpSocket m_fmsSender;
    QTcpSocket m_fmsReceiver;
    QUdpSocket m_robotSender;
    QUdpSocket m_robotReceiver;
};

}

#endif
