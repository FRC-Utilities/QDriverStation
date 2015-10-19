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
#ifndef _LIB_DS_ELAPSED_TIME_H
#define _LIB_DS_ELAPSED_TIME_H

#include "DS_Global.h"
#include <QElapsedTimer>

/**
 * \class DS_ElapsedTime
 *
 * The DS_ElapsedTime class calculates the elapsed time since a specified
 * time in the execution of the application and presents it in human-readable
 * format (mm::ss.ms).
 */
class LIB_DS_DECL DS_ElapsedTime : public QObject
{
    Q_OBJECT

public:
    explicit DS_ElapsedTime();

public slots:
    /**
     * Pauses the elapsed time refresh process
     */
    void stop();

    /**
     * Resets the elapsed timer and starts the refresh process again
     */
    void reset();

signals:
    /**
     * Emitted when the elapsed time is calculated and processed
     * in a human-readable format
     */
    void elapsedTimeChanged (QString time);

private:
    bool m_enabled;
    QElapsedTimer m_time;

private slots:
    /**
     * @internal
     * Uses the value given by the internal timer and processes its
     * information into a human-readable format (mm::ss.ms)
     */
    void calculateElapsedTime();
};

#endif

