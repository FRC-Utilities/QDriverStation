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

#ifndef _DRIVER_STATION_NET_CONSOLE_H
#define _DRIVER_STATION_NET_CONSOLE_H

#include <QString>
#include <QObject>
#include <QHostAddress>

class QUdpSocket;
class DriverStation;

class NetConsole : public QObject
{
    friend class DriverStation;

    Q_OBJECT

public slots:
    /**
     * Returns the only instance of the class
     */
    static NetConsole* getInstance();

signals:
    /**
     * Emitted when a new UDP datagram is received on the input port
     */
    void newMessage (QString message);

protected:
    explicit NetConsole();

private:
    static NetConsole* m_instance;

    QHostAddress m_address;
    QUdpSocket* m_inSocket;
    QUdpSocket* m_outSocket;

private slots:
    /**
     * @internal
     * Parses the received data and emits a singal to connected objects
     */
    void onMessageReceived();

    /**
     * Sends a welcome message to the connected objects and starts
     * the UDP broadcasting/listening process.
     */
    void init();
};

#endif /* _QDS_NET_CONSOLE_H */
