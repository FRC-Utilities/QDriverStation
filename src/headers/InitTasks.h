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
#ifndef _QDRIVER_STATION_INIT_TASKS_H
#define _QDRIVER_STATION_INIT_TASKS_H

#include <DriverStation.h>

/**
 * @class InitTasks
 * @brief Isolates complex first run tasks from the MainWindow
 *
 * The purpose of this class is to isolate the tasks that the application must
 * perform during initalization and - specifically - during the first run,
 * such as asking the user for his/her team number of downloading additional
 * drivers required by the Open DriverStation.
 */
class InitTasks {
  public:
    /**
     * Returns the team number to be used to control the robot.
     * If this is the first launch, it will ask the user to input his/her team
     * number using a \c QInputDialog
     */
    static int getTeamNumber();

    /**
     * Initializes the CPU querying process and prompts the user to download
     * additional software required for the correct functionality of the
     * Open DriverStation
     */
    static void executeFirstRunTasks();
};

#endif
