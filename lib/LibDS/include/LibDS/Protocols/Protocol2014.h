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

#ifndef _LDS_PROTOCOL_2014_H
#define _LDS_PROTOCOL_2014_H

#include "LibDS/Core/ProtocolBase.h"

/**
 * \class DS_Protocol2014
 * \brief Implements the 2014 communication protocol
 * \note  the virtual functions are already documented in the
 *        \c DS_ProtocolBase class
 */
class LIB_DS_DECL DS_Protocol2014 : public DS_ProtocolBase
{
    Q_OBJECT

public:
    virtual int robotPort();
    virtual int clientPort();
    virtual int tcpProbePort();
    virtual int netConsolePort();
    virtual bool acceptsConsoleCommands();
    virtual QStringList defaultRadioAddress();
    virtual QStringList defaultRobotAddress();

public slots:
    virtual void reboot();
    virtual void restartCode();

private slots:
    virtual void resetProtocol();

private:
    virtual bool readPacket (QByteArray data);
    virtual QByteArray getClientPacket();
    virtual QByteArray getJoystickData();
    virtual QByteArray getTimezoneData();

    virtual int getControlCode();
    virtual int getAllianceCode();
    virtual int getJoystickSize (DS_Joystick* joystick);
};

#endif
