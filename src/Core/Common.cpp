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

#include "Core/Common.h"

QString DS_GetTimezoneCode() {
    switch (QDateTime::currentDateTime().offsetFromUtc() / 3600) {
    case -11:
        return "BST11BDT";
        break;
    case -10:
        return "HST10HDT";
        break;
    case -9:
        return "AST9ADT";
        break;
    case -8:
        return "PST8PDT";
        break;
    case -7:
        return "MST7MDT";
        break;
    case -6:
        return "CST6CDT";
        break;
    case -5:
        return "EST5EDT";
        break;
    case -4:
        return "AST4ADT";
        break;
    case -3:
        return "GRNLNDST3GRNLNDDT";
        break;
    case -2:
        return "FALKST2FALKDT";
        break;
    case -1:
        return "AZOREST1AZOREDT";
        break;
    case 0:
        return "GMT0BST";
        break;
    case 1:
        return "NFT-1DFT";
        break;
    case 2:
        return "WET-2WET";
        break;
    case 3:
        return "MEST-3MEDT";
        break;
    case 4:
        return "WST-4WDT";
        break;
    case 5:
        return "PAKST-5PAKDT";
        break;
    case 6:
        return "TASHST-6TASHDT";
        break;
    case 7:
        return "THAIST-7THAIDT";
        break;
    case 8:
        return "WAUST-8WAUDT";
        break;
    case 9:
        return "JST-9JSTDT";
        break;
    case 10:
        return "EET-10EETDT";
        break;
    case 11:
        return "MET-11METDT";
        break;
    case 12:
        return "NZST-12NZDT";
        break;
    }

    return "GMT0BST";
}

QString DS_GetStaticIp (DS_Char team, DS_Char host) {
    QString string = QString ("%1").arg (team);

    switch (string.length()) {
    case 1:
        string = "00.0" + string;
        break;
    case 2:
        string = "00." + string;
        break;
    case 3:
        string = QString ("0%1.%2%3").arg (string.at (0),
                                           string.at (1),
                                           string.at (2));
        break;
    case 4:
        string = QString ("%1%2.%3%4").arg (string.at (0),
                                            string.at (1),
                                            string.at (2),
                                            string.at (3));
        break;
    }

    return QString ("10.%1.%2").arg (string, QString::number (host));
}

QString DS_GetControlModeString (DS_ControlMode mode) {
    switch (mode) {
    case kControlTest:
        return QString ("Test");
        break;
    case kControlTeleoperated:
        return QString ("Teleoperated");
        break;
    case kControlDisabled:
        return QString ("Disabled");
        break;
    case kControlAutonomous:
        return QString ("Autonomous");
        break;
    case kControlEmergencyStop:
        return QString ("Emergency Stopped");
        break;
    case kControlNoCommunication:
        return QString ("No Robot Communication");
        break;
    }

    return QString ("");
}

QByteArray DS_GetSocketData (QUdpSocket* socket) {
    QByteArray data;

    while (socket->hasPendingDatagrams()) {
        data.resize (socket->pendingDatagramSize());
        socket->readDatagram (data.data(), data.size());
    }

    return data;
}
