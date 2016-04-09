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

#include "LibDS/Core/Watchdog.h"

using namespace DS_Core;

//==================================================================================================
// Watchdog::DS_Watchdog
//==================================================================================================

Watchdog::Watchdog() {
    connect (&m_timer, &QTimer::timeout, this, &Watchdog::timeout);
}

//==================================================================================================
// Watchdog::expirationTime
//==================================================================================================

int Watchdog::expirationTime() const {
    return m_timer.interval();
}

//==================================================================================================
// Watchdog::restart
//==================================================================================================

void Watchdog::restart() {
    m_timer.stop();
    m_timer.start();
}

//==================================================================================================
// Watchdog::setTimeout
//==================================================================================================

void Watchdog::setTimeout (int time) {
    m_timer.setInterval (time);
    restart();
}
