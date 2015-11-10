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
#ifndef _LIB_DS_WATCHDOG_H
#define _LIB_DS_WATCHDOG_H

#include <QTimer>
#include "DS_Global.h"

/**
 * \class DS_Watchdog
 *
 * Implements a simple software watchdog with the help of the \c QTimer class.
 *
 * During normal operation, the program regularly restarts the watchdog timer
 * to prevent it from elapsing, or "timing out".
 * If, due to a hardware fault or program error, the program fails to restart the
 * watchdog, the timer will elapse and generate a timeout signal.
 *
 * The timeout signal is used to initiate corrective action or actions.
 * The corrective actions typically include placing the computer system in a
 * safe state and restoring normal system operation.
 */
class LIB_DS_DECL DS_Watchdog : public QObject
{
    Q_OBJECT

public:
    explicit DS_Watchdog();

public slots:
    /**
     * Restarts the watchdog, used when an operation is
     * completed successfully and we want the software or
     * operation to continue executing normally.
     *
     * Think of this as "kicking the dog".
     */
    void restart();

signals:
    /**
     * Emitted when the timer has elapsed and a timeout signal
     * was emitted. You should implement a method to abort the current
     * operation or to initiate corrective action in the case you
     * receive this signal
     */
    void timeout();

private:
    /**
     * The timer that regulates the behavior of the class
     */
    QTimer m_timer;
};

#endif
