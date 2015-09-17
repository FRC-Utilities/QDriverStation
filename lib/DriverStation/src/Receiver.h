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

#ifndef _DRIVER_STATION_RECEIVER_H
#define _DRIVER_STATION_RECEIVER_H

#include "Common.h"
#include <QUdpSocket>

/**
 * \class DS_Receiver
 *
 * The DS_Receiver class in charge of receiving and processing the packets sent
 * by the roboRIO to the DriverStation.
 *
 * For example, this class is used to extract the battery voltage of the robot
 * by analyzing and processing the received packets.
 */
class DS_Receiver : public QObject
{
    Q_OBJECT

public:
    explicit DS_Receiver();

signals:
    /**
     * Emitted when the class receives a packet and gets the robot voltage
     * from the received packet
     */
    void voltageChanged (QString voltage);

    /**
     * Emitted when the class receives a packet and gets the confirmation
     * control mode of the robot. This is used for diagnostic purposes.
     */
    void confirmationReceived (DS_ControlMode mode);

private:
    QUdpSocket m_socket;

private slots:
    void onDataReceived();
};

#endif /* _DRIVER_STATION_RECEIVER_H */
