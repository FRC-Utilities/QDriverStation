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

#include "CpuUsage.h"

#if defined _WIN32 || defined _WIN64
#include <pdh.h>
#include <tchar.h>
#include <windows.h>
static PDH_HQUERY cpuQuery;
static PDH_HCOUNTER cpuTotal;
#define COUNTER_PATH L"\\Processor(_Total)\\% Processor Time"
#endif

#if defined __APPLE__ || defined __gnu_linux__
#include <QProcess>
#endif

void CpuUsage::init()
{
#if defined _WIN32 || defined _WIN64
    PdhOpenQuery (0, 0, &cpuQuery);
    PdhAddCounter (cpuQuery, COUNTER_PATH, 0, &cpuTotal);
    PdhCollectQueryData (cpuQuery);
#endif
}

int CpuUsage::getUsage()
{
#if defined _WIN32 || defined _WIN64
    PDH_FMT_COUNTERVALUE counterVal;
    PdhCollectQueryData (cpuQuery);
    PdhGetFormattedCounterValue (cpuTotal, PDH_FMT_DOUBLE, 0, &counterVal);
    return static_cast<int> (counterVal.doubleValue);
#endif

#if defined __APPLE__
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

    /* If the data is shit then make the value 0 */
    if (t < 0) t = 0;
    if (u < 0) u = 0;

    return (t * 10) + u;
#endif

#if defined __gnu_linux__
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

    /* If the data is shit then make the value 0 */
    if (t < 0) t = 0;
    if (u < 0) u = 0;

    return (t * 10) + u;
#endif
}
