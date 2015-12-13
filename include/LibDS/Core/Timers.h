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
#ifndef _LIB_DS_TIMERS_H
#define _LIB_DS_TIMERS_H

#include "LibDS/Core/Common.h"

class QTimer;
class QThread;

/**
 * \class DS_Timer
 *
 * Implements a set of timers that are separeted from the main thread to
 * guarantee that they are updated in a more timely manner.
 * The timers can be used by objects and classes that are not related to
 * the library for different functions.
 */
class LIB_DS_DECL DS_Timers : public QObject {
    Q_OBJECT

  public:
    static DS_Timers* GetInstance();
    ~DS_Timers();

  public slots:
    void Start();

  signals:
    void Timeout20();
    void Timeout100();
    void Timeout1000();

  protected:
    DS_Timers();
    static DS_Timers* s_instance;

  private:
    QTimer* t20;
    QTimer* t100;
    QTimer* t1000;
    QThread* m_thread;
};

#endif
