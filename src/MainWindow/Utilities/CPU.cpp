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

#include "CPU.h"

#if defined Q_OS_WIN
#include <pdh.h>
#include <tchar.h>
#include <windows.h>
static PDH_HQUERY cpuQuery;
static PDH_HCOUNTER cpuTotal;
#else
#include <QProcess>
#endif

void CPU::init() {
#if defined Q_OS_WIN
    PdhOpenQuery (0, 0, &cpuQuery);
    PdhAddCounter (cpuQuery, L"\\Processor(_Total)\\% Processor Time", 0,
                   &cpuTotal);
    PdhCollectQueryData (cpuQuery);
#endif
}

int CPU::getUsage() {
    int usage = 0;

#if defined Q_OS_WIN
    PDH_FMT_COUNTERVALUE counterVal;
    PdhCollectQueryData (cpuQuery);
    PdhGetFormattedCounterValue (cpuTotal, PDH_FMT_DOUBLE, 0, &counterVal);
    usage = counterVal.doubleValue;
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

    if (usage < 5) usage = 5;
    return usage;
}
