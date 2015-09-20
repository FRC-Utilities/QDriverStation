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
#include <QObject>

#include "Common.h"

QString DS_GetStaticIp (int team, int host)
{
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

QString DS_GetControlModeString (DS_ControlMode mode)
{
    switch (mode) {
    case DS_Test:
        return QObject::tr ("Test");
        break;
    case DS_TeleOp:
        return QObject::tr ("TeleOp");
        break;
    case DS_Disabled:
        return QObject::tr ("Disabled");
        break;
    case DS_Autonomous:
        return QObject::tr ("Autonomous");
        break;
    case DS_EmergencyStop:
        return QObject::tr ("Emergency Stop");
        break;
    }

    return "ERR_INVALID_CONTROL_MODE";
}
