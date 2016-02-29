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

#ifndef _LDS_PROTOCOL_MANAGER_H
#define _LDS_PROTOCOL_MANAGER_H

#include "LibDS/Core/Common.h"

namespace DS_Core {

class ProtocolBase;

///
/// Allows the dynamic loading and usage of different protocols by configuring them
/// so that the \c DriverStation can send and recieve information from the protocol.
///
class LIB_DS_DECL ProtocolManager : public QObject {
    Q_OBJECT

  public:
    ///
    /// Returns a pointer to the current protocol in use by the DS
    ///
    ProtocolBase* currentProtocol() const;

    ///
    /// Returns \c true if the current protocol is not pointing to a
    /// NULL address
    ///
    bool isValid() const;

    ///
    /// Returns the number of joysticks registered in the \c LibDS system
    ///
    int joystickCount() const;

  public slots:
    ///
    /// Loads and configures the given \a protocol
    ///
    void setProtocol (ProtocolBase* protocol);

    ///
    /// Instructs the current protocol to interpret the given robot \a data
    ///
    void readRobotPacket (QByteArray data);

  public:
    explicit ProtocolManager (QObject* parent);

  signals:
    void fmsChanged (bool);
    void codeChanged (bool);
    void emergencyStopped();
    void cpuUsageChanged (int);
    void ramUsageChanged (int);
    void diskUsageChanged (int);
    void radioCommChanged (bool);
    void voltageChanged (QString);
    void CANInfoReceived (DS::CAN);
    void libVersionChanged (QString);
    void rioVersionChanged (QString);
    void pdpVersionChanged (QString);
    void pcmVersionChanged (QString);
    void robotAddressChanged (QString);
    void voltageBrownoutChanged (bool);
    void controlModeChanged (DS::ControlMode);
    void communicationsChanged (DS::DS_CommStatus);

  private:
    ProtocolBase* m_protocol;
};

}

#endif
