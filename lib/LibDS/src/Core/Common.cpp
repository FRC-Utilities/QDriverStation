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

#include "LibDS/Core/Common.h"

//=============================================================================
// Ugly logger hacks
//=============================================================================

static bool _LOG_INIT = false;
#define _FORMAT "%-26s %-12s %-10s\n"
const QString _TIME_FORMAT = "dd/MMM/yyyy hh:mm:ss::zzz";

//=============================================================================
// DS_GetTimezoneCode
//=============================================================================

QString DS_GetTimezoneCode()
{
    switch (QDateTime::currentDateTime().offsetFromUtc() / 3600)
        {
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

//=============================================================================
// DS_SendMessage
//=============================================================================

void DS_SendMessage (QString message)
{
    QUdpSocket* socket = new QUdpSocket;
    socket->writeDatagram (message.toUtf8(), QHostAddress::LocalHost, 6666);

    delete socket;
}

//=============================================================================
// DS_LogMessage
//=============================================================================

void DS_LogMessage (DS_MessageType type,  QString message)
{
    /* Get level & time */
    QString level;
    QString time = QDateTime::currentDateTime().toString (_TIME_FORMAT);

    /* Open log output file */
    if (!_LOG_INIT)
        {
            _LOG_INIT = true;
            fprintf (stderr, _FORMAT, "DATE/TIME", "ERROR LEVEL", "MESSAGE");
        }

    /* Get the error level */
    switch (type)
        {
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
             _FORMAT,
             time.toStdString().c_str(),
             level.toStdString().c_str(),
             message.toStdString().c_str());
}

//=============================================================================
// DS_GetStaticIp
//=============================================================================

QString DS_GetStaticIp (int team, int host)
{
    return DS_GetStaticIp (10, team, host);
}

//=============================================================================
// DS_GetStaticIp
//=============================================================================

QString DS_GetStaticIp (int net, int team, int host)
{
    QString string = QString ("%1").arg (team);

    switch (string.length())
        {
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

//=============================================================================
// DS_GetControlModeString
//=============================================================================

QString DS_GetControlModeString (DS_ControlMode mode)
{
    QString string;

    switch (mode)
        {
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

//=============================================================================
// DS_GetSocketData
//=============================================================================

QByteArray DS_GetSocketData (QUdpSocket* socket)
{
    QByteArray buffer;

    buffer.resize (socket->pendingDatagramSize());
    socket->readDatagram (buffer.data(), buffer.size());

    return buffer;
}

//=============================================================================
// DS_GetSocketData
//=============================================================================

QByteArray DS_GetSocketData (QTcpSocket* socket)
{
    return socket->readAll();;
}

//=============================================================================
// DS_ToBytes
//=============================================================================

QByteArray DS_ToBytes (int data)
{
    QByteArray array;

    if (data == -1)
        {
            array.append (char (0xFF));
            array.append (char (0xFF));
        }

    else
        {
            array.append (char ((data >> 8) & 0xFF));
            array.append (char (data & 0xFF));
        }

    return array;
}
