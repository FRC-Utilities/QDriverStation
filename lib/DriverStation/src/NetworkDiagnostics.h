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
 * LIABILITY, WHETHER IN AN ACTION O F CONTRACT, TORT OR OTHERWISE, ARISING FROM,
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
class DS_NetworkDiagnostics
{
public:
    /**
     * Entry point of the class
     */
    DS_NetworkDiagnostics();

    /**
     * Returns \c true if the class was able to ping the roboRIO
     */
    bool roboRioIsAlive();

    /**
     * Returns \c true if the class was able to ping the wireless radio
     */
    bool robotRadioIsAlive();

    /**
     * Returns the address of the wireless radio
     */
    QString radioIpAddress();

    /**
     * Returns the address of the roboRIO
     */
    QString roboRioIpAddress();

    /**
     * Pings the roboRIO and the wireless radio
     */
    void refresh();

    /**
     * Changes the team number, which is used to find the roboRIO and the
     * wireless radio
     */
    void setTeamNumber (int team);

    /**
     * Changes the address to use for the roboRIO
     */
    void setCustomAddress (QString address);

private:
    int m_teamNumber;
    bool m_rioIsAlive;
    bool m_radioIsAlive;

    QTcpSocket m_rioSocket;
    QTcpSocket m_radioSocket;
    QString m_customRioAddress;
};
