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

#ifndef _LDS_PROTOCOL_2015_H
#define _LDS_PROTOCOL_2015_H

#include "LibDS/Core/AbstractProtocol.h"

namespace DS_Protocols {

class LIB_DS_DECL FRC_Protocol2015 : public DS_Core::AbstractProtocol {
    Q_OBJECT

  public:
    virtual QString name();
    virtual int fmsFrequency();
    virtual int robotFrequency();

    virtual int fmsInputPort();
    virtual int fmsOutputPort();
    virtual int tcpProbesPort();
    virtual int robotInputPort();
    virtual int robotOutputPort();
    virtual int netConsoleInputPort();
    virtual bool acceptsConsoleCommands();

  public slots:
    virtual void reboot();
    virtual void restartCode();

  private slots:
    virtual void resetProtocol();

  private:
    virtual bool interpretFmsPacket (QByteArray data);
    virtual bool interpretRobotPacket (QByteArray data);

    virtual QByteArray getJoystickData();
    virtual QByteArray getTimezoneData();
    virtual QByteArray generateFmsPacket();
    virtual QByteArray generateRobotPacket();

    virtual QStringList additionalRobotIPs();

    virtual int getControlCode();
    virtual int getAllianceCode();
    virtual int getJoystickSize (DS::Joystick joystick);

    int m_instructionCode;
};

}

#endif
