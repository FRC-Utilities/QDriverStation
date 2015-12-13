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

#include "Battery.h"

#if defined Q_OS_WIN
#include <windows.h>
static SYSTEM_POWER_STATUS power;
#else
#include <QProcess>
#endif

//=============================================================================
// Battery::IsConnectedToPowerSupply
//=============================================================================

bool Battery::IsConenctedToPowerSupply() {
#if defined Q_OS_WIN
    GetSystemPowerStatus (&power);
    return power.ACLineStatus != 0;
#endif

#if defined Q_OS_MAC
    QByteArray data;
    QProcess process;

    process.start ("pmset -g batt");

    while (process.waitForReadyRead())
        data.append (process.readAll());

    if (data.contains ("discharging"))
        return false;

    return true;
#endif

#if defined Q_OS_LINUX
    QByteArray data;
    QProcess process;

    process.start ("bash -c \"upower -i $(upower -e | grep 'BAT') | "
                   "grep -E 'state|to\\ full|percentage'\"");

    while (process.waitForReadyRead())
        data.append (process.readAll());

    if (data.contains ("discharging"))
        return false;

    return true;
#endif
}

//=============================================================================
// Battery::CurrentLevel
//=============================================================================

int Battery::CurrentLevel() {
#if defined Q_OS_WIN
    GetSystemPowerStatus (&power);
    return static_cast<int> (power.BatteryLifePercent);
#endif

#if defined Q_OS_MAC
    QByteArray data;
    QProcess process;

    process.start ("pmset -g batt");

    while (process.waitForReadyRead())
        data.append (process.readAll());

    /* Parse the digits of the percentage */
    int h = data.at (data.indexOf ("%") - 3) - '0'; // Hundreds
    int t = data.at (data.indexOf ("%") - 2) - '0'; // Tens
    int u = data.at (data.indexOf ("%") - 1) - '0'; // Units

    /* Process data is invalid or we do not know how to read */
    if (h < 0) h = 0;
    if (t < 0) t = 0;
    if (u < 0) u = 0;

    return (h * 100) + (t * 10) + u;
#endif

#if defined Q_OS_LINUX
    QByteArray data;
    QProcess process;

    process.start ("bash -c \"upower -i $(upower -e | grep 'BAT') | "
                   "grep -E 'state|to\\ full|percentage'\"");

    while (process.waitForReadyRead())
        data.append (process.readAll());

    /* Parse the digits of the percentage */
    int h = data.at (data.indexOf ("%") - 3) - '0'; // Hundreds
    int t = data.at (data.indexOf ("%") - 2) - '0'; // Tens
    int u = data.at (data.indexOf ("%") - 1) - '0'; // Units

    /* Process data is invalid or we do not know how to read */
    if (h < 0) h = 0;
    if (t < 0) t = 0;
    if (u < 0) u = 0;

    return (h * 100) + (t * 10) + u;
#endif
}
