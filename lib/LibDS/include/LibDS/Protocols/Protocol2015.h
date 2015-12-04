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
#ifndef _LIB_DS_PROTOCOL_2015_H
#define _LIB_DS_PROTOCOL_2015_H

#include "LibDS/Core/ProtocolBase.h"

/**
 * \class DS_Protocol2015
 * \brief Implements the 2015 communication protocol
 * \note  the virtual functions are already documented in the
 *        \c DS_ProtocolBase class
 */
class LIB_DS_DECL DS_Protocol2015 : public DS_ProtocolBase {
    Q_OBJECT

  public:
    explicit DS_Protocol2015();

    int robotPort();
    int clientPort();
    QString defaultRadioAddress();
    QString defaultRobotAddress();

  public slots:
    void reboot();
    void restartCode();

  private slots:
    void resetProtocol();
    void downloadRobotInformation();
    void readRobotData (QByteArray data);
    void onDownloadFinished (QNetworkReply* reply);

  private:
    QByteArray generateClientPacket();
    QByteArray generateJoystickData();
    QByteArray generateTimezoneData();
    DS_ControlMode getControlMode (int byte);

    int getControlCode (DS_ControlMode mode);
    int getAllianceCode (DS_Alliance alliance);
    int getJoystickSize (DS_Joystick* joystick);

    QNetworkAccessManager m_manager;
};

#endif
