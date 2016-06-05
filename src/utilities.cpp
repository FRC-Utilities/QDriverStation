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

#include "utilities.h"

#if defined Q_OS_WIN
#include <pdh.h>
#include <tchar.h>
#include <windows.h>
static PDH_HQUERY cpuQuery;
static PDH_HCOUNTER cpuTotal;
static SYSTEM_POWER_STATUS power;
#else
#include <QProcess>
#endif

/**
 * Initializes the CPU querying process under Microsoft Windows
 */
Utilities::Utilities()
{
#if defined Q_OS_WIN
    PdhOpenQuery (0, 0, &cpuQuery);
    PdhAddCounter (cpuQuery,
                   L"\\Processor(_Total)\\% Processor Time",
                   0,
                   &cpuTotal);
    PdhCollectQueryData (cpuQuery);
#endif
}

/**
 * Returns the current CPU usage (from 0 to 100)
 */
int Utilities::getCpuUsage()
{
    int usage = 0;

#if defined Q_OS_WIN
    PDH_FMT_COUNTERVALUE counterVal;
    PdhCollectQueryData (cpuQuery);
    PdhGetFormattedCounterValue (cpuTotal, PDH_FMT_DOUBLE, 0, &counterVal);
    usage = (int) counterVal.doubleValue;
#endif

#if defined Q_OS_MAC
    QByteArray data;
    QProcess process;

    /* Launch a CLI command that gives us the CPU usage */
    process.start ("bash -c \"ps -A -o %cpu | awk '{s+=$1} END {print s}'\"");

    /* Read process output */
    while (process.waitForReadyRead())
        data.append (process.readAll());

    /* Parse the digits of the percentage */
    int t = data.at (0) - '0';
    int u = data.at (1) - '0';

    /* Process data is invalid or we do not know how to read */
    if (t < 0) t = 0;
    if (u < 0) u = 0;

    usage = (t * 10) + u;
#endif

#if defined Q_OS_LINUX
    QByteArray data;
    QProcess process;

    /* Launch a CLI command that gives us the CPU usage */
    process.start ("bash -c \"grep 'cpu ' /proc/stat | "
                   "awk '{usage=($2+$4)*100/($2+$4+$5)} "
                   "END {print usage}'\"");

    /* Read process output */
    while (process.waitForReadyRead())
        data.append (process.readAll());

    /* Parse the digits of the percentage */
    int t = data.at (0) - '0';
    int u = data.at (1) - '0';

    /* Process data is invalid or we do not know how to read */
    if (t < 0) t = 0;
    if (u < 0) u = 0;

    usage = (t * 10) + u;
#endif

    if (usage < 10)
        usage = 10;

    return usage;
}

/**
 * Returns the current battery level (from 0 to 100)
 */
int Utilities::getBatteryLevel()
{
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

/**
 * Returns \c true if the computer is connected to a power source or the
 * battery is not discharging.
 */
bool Utilities::isConnectedToPowerSource()
{
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

    return !data.contains ("discharging");
#endif

#if defined Q_OS_LINUX
    QByteArray data;
    QProcess process;

    process.start ("bash -c \"upower -i $(upower -e | grep 'BAT') | "
                   "grep -E 'state|to\\ full|percentage'\"");

    while (process.waitForReadyRead())
        data.append (process.readAll());

    return !data.contains ("discharging");
#endif
}
