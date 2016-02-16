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

/**
 * \class DS_NetConsole
 *
 * The DS_NetConsole class receives and decodes messages broadcasted
 * by the robot over the local area network.
 */
class LIB_DS_DECL DS_NetConsole : public QObject
{
    Q_OBJECT

public:
    explicit DS_NetConsole (QObject* parent);

public slots:
    /**
     * Changes the port used by the net console to get robot data
     */
    void setInputPort (int port);

    /**
     * Changes the port used by the net console to send robot data
     */
    void setOutputPort (int port);

    /**
     * Sends a command to the robot through the Console network
     */
    void sendCommand (QString command);

    /**
     * Enables or disables the option to send commands through the NetConsole
     */
    void setAcceptsInput (bool acceptsInput);

signals:
    /**
     * Emitted when a message is received from the robot or the
     * internal Driver Station system
     */
    void newMessage (QString);

private:
    /**
     * The UDP port of the NetConsole
     */
    int m_outPort;

    /**
     * Controls 'write' access to the console
     */
    bool m_acceptsInput;

    /**
     * The network socket in which we receive data from the robot
     */
    QUdpSocket m_socket;

private slots:
    /**
     * @internal
     * Called when we receive data in the network socket.
     * Used to read the input data and process it.
     */
    void readSocket();
};

#endif
