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

#pragma once
#ifndef _LIB_DS_CLIENT_H
#define _LIB_DS_CLIENT_H

#include "LibDS/Core/Common.h"

/**
 * \class DS_Client
 *
 * Sends and receives data between the robot and the Driver Station.
 * The class "redirects" the received data from the robot to the current
 * protocol that is in use by the Driver Station and vice-versa.
 */
class LIB_DS_DECL DS_Client : public QObject {
    Q_OBJECT

  public:
    explicit DS_Client();
    ~DS_Client();

  public slots:
    /**
     * Sends a the input \a data to the robot
     */
    void sendToRobot (QByteArray data);

    /**
     * Changes the port in which we send the packets to
     */
    void setRobotPort (quint16 port);

    /**
     * Changes the port in where we receive robot packets
     */
    void setClientPort (quint16 port);

    /**
     * Changes the address where we send the packets to
     */
    void setRobotAddress (QString address);

  signals:
    /**
     * Emitted when the client receives a packet from the robot
     */
    void dataReceived (QByteArray);

  private slots:
    /**
     * Reads the received data and sends it to the \c DriverStation
     */
    void onDataReceived();

  private:
    /**
     * The port in which we send data to the robot
     */
    quint16 m_robotPort;

    /**
     * The address of the robot
     */
    QString m_address;

    /**
     * We send data to the robot through this socket
     */
    QUdpSocket m_robotSocket;

    /**
     * We receive data from the robot through this socket
     */
    QUdpSocket m_clientSocket;
};

#endif
