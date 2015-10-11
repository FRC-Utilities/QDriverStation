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

#ifdef SCU_BUILD

/* Include LibDS code */
#include "../lib/LibDS/src/LibDS/DS_Client.cpp"
#include "../lib/LibDS/src/LibDS/DS_Common.cpp"
#include "../lib/LibDS/src/LibDS/DS_ElapsedTime.cpp"
#include "../lib/LibDS/src/LibDS/DS_NetConsole.cpp"
#include "../lib/LibDS/src/LibDS/DS_Protocol.cpp"
#include "../lib/LibDS/src/LibDS/DS_Protocol2014.cpp"
#include "../lib/LibDS/src/LibDS/DS_Protocol2015.cpp"
#include "../lib/LibDS/src/LibDS/DS_ProtocolManager.cpp"
#include "../lib/LibDS/src/LibDS/DS_Timers.cpp"
#include "../lib/LibDS/src/LibDS/DS_Watchdog.cpp"
#include "../lib/LibDS/src/DriverStation.cpp"

/* Include application code */
#include "sources/AdvancedSettings.cpp"
#include "sources/AssemblyInfo.cpp"
#include "sources/Battery.cpp"
#include "sources/BatteryProgressbar.cpp"
#include "sources/CpuUsage.cpp"
#include "sources/Dashboard.cpp"
#include "sources/GamepadManager.cpp"
#include "sources/InitTasks.cpp"
#include "sources/JoysticksWidget.cpp"
#include "sources/MainWindow.cpp"
#include "sources/Settings.cpp"
#include "sources/SmartWindow.cpp"

/* Include entry point code */
#include "main.cpp"

#endif
