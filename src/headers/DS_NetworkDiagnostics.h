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

#include <QString>
#include <QTcpSocket>

/**
 * \class DS_NetworkDiagnostics
 *
 * The DS_NetworkDiagnostics class is in charge of calculating the IP of the
 * network components of the robot and checking which components are accesible
 * to the DriverStation.
 */
class DS_NetworkDiagnostics : public QObject {
    Q_OBJECT

public:
    explicit DS_NetworkDiagnostics();

public slots:
    /**
     * Pings the robot and the robot radio and updates the internal values
     */
    void refresh();

    /**
     * Returns \c true if the class was able to ping the robot
     */
    bool robotIsAlive();

    /**
     * Returns \c true if the class was able to ping the wireless radio
     */
    bool radioIsAlive();

    /**
     * Changes the address that we use to communicate with the radio
     */
    void setRadioAddress (QString address);

    /**
     * Changes the address that we use to communicate with the robot
     */
    void setRobotAddress (QString address);

private:
    bool m_robotIsAlive;
    bool m_radioIsAlive;
    QString m_robotAddress;
    QString m_radioAddress;
    QTcpSocket m_robotSocket;
    QTcpSocket m_radioSocket;
};
