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

#include <QDir>
#include <QApplication>

#include "sse4_crc32/crc32c.h"
#include "LibDS/Core/Common.h"
#include "LibDS/DriverStation.h"

//==================================================================================================
// Ugly logger hacks
//==================================================================================================

static bool _LOG_INIT      = false;
const char* _PRINT_FMT     = "%-26s %-12s %-10s\n";
const QString _TIME_FORMAT = "dd/MMM/yyyy hh:mm:ss::zzz";

//==================================================================================================
// DS::timezone
//==================================================================================================

QString DS::timezone() {
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

//==================================================================================================
// DS::sendMessage
//==================================================================================================

void DS::sendMessage (QString message) {
    DriverStation::getInstance()->newMessage (message);
}

//==================================================================================================
// DS::log
//==================================================================================================

void DS::log (ErrorLevel type,  QString message) {
    /* Get level & time */
    QString level;
    QString time = QDateTime::currentDateTime().toString (_TIME_FORMAT);

    /* Open log output file */
    if (!_LOG_INIT) {
        _LOG_INIT = true;
        fprintf (stderr, _PRINT_FMT, "DATE/TIME", "ERROR LEVEL", "MESSAGE");
    }

    /* Get the error level */
    switch (type) {
    case kLibLevel:
        level = "DS_LIB";
        break;
    case kInfoLevel:
        level = "DS_CLIENT";
        break;
    case kWarnLevel:
        level = "DS_WARN";
        break;
    case kErrorLevel:
        level = "DS_ERROR";
        break;
    case kCriticalLevel:
        level = "DS_CRITICAL";
        break;
    }

    /* Write log message */
    fprintf (stderr,
             _PRINT_FMT,
             time.toStdString().c_str(),
             level.toStdString().c_str(),
             message.toStdString().c_str());
}

//==================================================================================================
// DS::getStaticIp
//==================================================================================================

QString DS::getStaticIP (int team, int host) {
    return DS::getStaticIP (10, team, host);
}

//==================================================================================================
// DS::getStaticIp
//==================================================================================================

QString DS::getStaticIP (int net, int team, int host) {
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

    return QString ("%1.%2.%3").arg (QString::number (net),
                                     string,
                                     QString::number (host));
}

//==================================================================================================
// DS::controlModeString
//==================================================================================================

QString DS::controlModeString (ControlMode mode) {
    QString string;

    switch (mode) {
    case kControlTest:
        string = "Test";
        break;
    case kControlTeleoperated:
        string = "Teleoperated";
        break;
    case kControlAutonomous:
        string = "Autonomous";
        break;
    case kControlInvalid:
        string = "";
        break;
    }

    return string;
}

//==================================================================================================
// DS::readSocket
//==================================================================================================

QByteArray DS::readSocket (QUdpSocket* socket) {
    QByteArray buffer;

    buffer.resize (socket->pendingDatagramSize());
    socket->readDatagram (buffer.data(), buffer.size());

    return buffer;
}

//==================================================================================================
// DS::readSocket
//==================================================================================================

QByteArray DS::readSocket (QTcpSocket* socket) {
    return socket->readAll();
}

//==================================================================================================
// DS::intToBytes
//==================================================================================================

QByteArray DS::intToBytes (int number) {
    QByteArray array;

    if (number == -1) {
        array.append (char (0xff));
        array.append (char (0xff));
    }

    else {
        array.append (char ((number & 0xff00) >> 8));
        array.append (char ((number & 0xff)));
    }

    return array;
}

//==================================================================================================
// DS::crc32
//==================================================================================================

uint32_t DS::crc32 (QByteArray data) {
    return hwCrc32c (0, data.toStdString().c_str(), data.length());
}
