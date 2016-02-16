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

#ifndef _LDS_ELAPSED_TIME_H
#define _LDS_ELAPSED_TIME_H

#include <QTimer>
#include <QElapsedTimer>

#include "LibDS/Core/Common.h"

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
    explicit DS_ElapsedTime (QObject* parent);

public slots:
    /**
     * Pauses the elapsed time refresh process
     */
    void stopTimer();

    /**
     * Resets the elapsed timer and starts the refresh process again
     */
    void resetTimer();

signals:
    /**
     * Emitted when the elapsed time is calculated and processed
     * in a human-readable format
     */
    void elapsedTimeChanged (QString);

private:
    /**
     * If set to \c true, the timer will be enabled and the class
     * will emit the \a elapsedTimeChanged() signal periodically
     */
    bool m_enabled;

    /**
     * The timer that is used to calculate the elapsed time and
     * provide it in a more human-friendly format
     */
    QElapsedTimer m_time;

private slots:
    /**
     * Uses the value given by the internal timer and processes its
     * information into a human-readable format (mm::ss.ms)
     */
    void getElapsedTime();
};

#endif

