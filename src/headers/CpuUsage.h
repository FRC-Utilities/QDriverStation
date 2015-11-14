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

#pragma once
#ifndef _QDRIVER_STATION_CPU_USAGE_H
#define _QDRIVER_STATION_CPU_USAGE_H

/**
 * @class CpuUsage
 * @brief Provides information about the CPU usage of the host computer
 * @warning You must call CpuUsage::init() before using the class
 *
 * The \c CpuUsage class provides information regarding the total usage of the
 * CPU of the host computer. It currently supports Windows, Mac and Linux.
 *
 * The class was implemented for the sole use of the CPU progress bar in
 * the \c MainWindow, but we isolated its functions for readibility reasons.
 */
class CpuUsage
{
public:
    /**
     * Starts the processor time querying process on Microsoft Windows
     */
    static void init ();

    /**
     * Uses the native API calls of the target operating system to obtain the
     * current CPU usage levels.
     *
     * If the target operating system is Mac or Linux, reads the output of a
     * command line utility to determine the CPU usage level.
     *
     * @return an \c int between 0 and 100 that represents the CPU usage
     */
    static int getUsage ();
};

#endif
