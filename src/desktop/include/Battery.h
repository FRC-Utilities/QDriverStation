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

#ifndef _QDS_BATTERY_H
#define _QDS_BATTERY_H

/**
 * @class Battery
 * @brief Provides information about the battery of the host computer
 *
 * The \c Battery class provides information about the state of the battery
 * of the host computer. It currently supports Windows, Mac and Linux.
 *
 * The class was implemented for the sole use of the battery progress bar in
 * the \c MainWindow, but we isolated its functions for readability reasons.
 */
class Battery
{
public:
    /**
     * Uses native API calls for the target operating system to obtain the
     * status of the AC line power. Used to display the 'plug' icon near the
     * battery progress bar in the MainWindow.
     *
     * If the target operating system is Mac or Linux, reads the output of a
     * command line utility to determine if the computer is connected to
     * an AC power supply.
     *
     * @return \c true if the computer is connected to an AC power supply
     */
    static bool isPlugged();

    /**
     * Uses the native API calls of the target operating system to obtain the
     * current battery percentage. Used to display the current level of the
     * battery in the MainWindow.
     *
     * If the target operating system is Mac or Linux, reads the output of a
     * command line utility to determine the battery level of the laptop.
     *
     * @return an \c int between 0 and 100 that represents the battery level
     */
    static int currentLevel();
};

#endif /* _QDS_BATTERY_H */
