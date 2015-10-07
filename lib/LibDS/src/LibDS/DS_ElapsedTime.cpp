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

#include "LibDS/DS_Timers.h"
#include "LibDS/DS_ElapsedTime.h"

DS_ElapsedTime::DS_ElapsedTime() {
    stop();
    calculateElapsedTime();

    connect (DS_Timers::getInstance(), SIGNAL (timeout100()),
             this,                     SLOT   (calculateElapsedTime()));
}

void DS_ElapsedTime::stop() {
    m_enabled = false;
}

void DS_ElapsedTime::reset() {
    m_enabled = true;
    m_time.restart();
}

void DS_ElapsedTime::calculateElapsedTime() {
    if (m_enabled) {
        int msec = m_time.elapsed();
        int secs = (msec / 1000);
        int mins = (secs / 60) % 60;

        secs = secs % 60;
        msec = msec % 1000;

        emit elapsedTimeChanged (QString ("%1:%2.%3")
                                 .arg (mins, 2, 10, QLatin1Char ('0'))
                                 .arg (secs, 2, 10, QLatin1Char ('0'))
                                 .arg (QString::number (msec).at (0)));
    }
}
