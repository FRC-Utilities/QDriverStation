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

#ifndef _LDS_NET_CONSOLE_H
#define _LDS_NET_CONSOLE_H

#include "LibDS/Core/Common.h"

namespace DS_Core {

///
/// This class recieves and sends broadcasted messages over the LAN.
/// The parameters of the NetConsole (such as I/O ports) are specified
/// by each protocol.
///
class LIB_DS_DECL NetConsole : public QObject {
    Q_OBJECT

  public slots:
    ///
    /// Changes the local port in which we listen for robot/DS messages
    ///
    void setInputPort (int port);

    ///
    /// Changes the remote port in which we send commands to the robot.
    /// Please note that this feature is not supported by all protocols,
    /// for example, 2014 robots accept NetConsole commands, while
    /// 2015 robots do not accept NetConsole commands.
    ///
    /// You can know if the current protocol supports NetConsole commands
    /// by calling the \c acceptsConsoleCommands function from the
    /// \c DriverStation class.
    ///
    void setOutputPort (int port);

    ///
    /// Sends the given \a command to the robot.
    /// In the case that the current protocol does not support sending
    /// NetConsole commands, the function will do nothing.
    ///
    void sendCommand (QString command);

    ///
    /// Allows or inhibits sending commands to the robot based on the
    /// value of the \a accepts_input parameter.
    ///
    /// This function is called by the \c DriverStation when a protocol
    /// is loaded.
    ///
    void setAcceptsInput (bool accepts_input);

  signals:
    ///
    /// Emitted when the NetConsole captures a broadcasted message
    ///
    void newMessage (QString);

  public:
    explicit NetConsole (QObject* parent);

  private:
    int m_outPort;
    bool m_acceptsInput;
    QUdpSocket m_socket;

  private slots:
    void readSocket();
};

}

#endif
