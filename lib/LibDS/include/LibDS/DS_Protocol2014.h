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
#ifndef _LIB_DS_PROTOCOL_2014_H
#define _LIB_DS_PROTOCOL_2014_H

#include "DS_Global.h"
#include "DS_Protocol.h"

#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>

/**
 * \class DS_Protocol2014
 * Implements the 2014 communication protocol
 */
class LIB_DS_DECL DS_Protocol2014 : public DS_Protocol {
    Q_OBJECT

  public:
    explicit DS_Protocol2014();

  public slots:
    void reset();
    void reboot();
    int robotPort();
    int clientPort();
    void restartCode();
    QString robotAddress();
    QString radioAddress();
    void downloadRobotInformation();
    QByteArray generateClientPacket();
    QByteArray generateJoystickData();
    void readRobotData (QByteArray data);
    char getControlCode (DS_ControlMode mode);
    DS_ControlMode getControlMode (char byte);
    char getAllianceCode (DS_Alliance alliance);
};

#endif
