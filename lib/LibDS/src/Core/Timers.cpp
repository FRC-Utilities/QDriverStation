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

#include "LibDS/Core/Timers.h"

#include <QTimer>
#include <QThread>

DS_Timers* DS_Timers::s_instance = Q_NULLPTR;

//=============================================================================
// DS_Timers::DS_Timers
//=============================================================================

DS_Timers::DS_Timers()
{
    t20  = new QTimer  (Q_NULLPTR);
    t100 = new QTimer  (Q_NULLPTR);
    t1000 = new QTimer (Q_NULLPTR);
    m_thread = new QThread (this);

    t20->moveToThread   (m_thread);
    t100->moveToThread  (m_thread);
    t1000->moveToThread (m_thread);

    t20->setInterval   (16);
    t100->setInterval  (100);
    t1000->setInterval (1000);

    t20->setTimerType   (Qt::PreciseTimer);
    t100->setTimerType  (Qt::PreciseTimer);
    t1000->setTimerType (Qt::PreciseTimer);

    connect (t20,   SIGNAL (timeout()), this, SIGNAL (Timeout20()));
    connect (t100,  SIGNAL (timeout()), this, SIGNAL (Timeout100()));
    connect (t1000, SIGNAL (timeout()), this, SIGNAL (Timeout1000()));
}

//=============================================================================
// DS_Timers::~DS_Timers
//=============================================================================

DS_Timers::~DS_Timers()
{
    delete t20;
    delete t100;
    delete t1000;
    delete m_thread;
    delete s_instance;
}

//=============================================================================
// DS_Timers::GetInstance
//=============================================================================

DS_Timers* DS_Timers::GetInstance()
{
    if (s_instance == Q_NULLPTR)
        s_instance = new DS_Timers;

    return s_instance;
}

//=============================================================================
// DS_Timers::Start
//=============================================================================

void DS_Timers::Start()
{
    connect (m_thread, SIGNAL (started()), t20,   SLOT (start()));
    connect (m_thread, SIGNAL (started()), t100,  SLOT (start()));
    connect (m_thread, SIGNAL (started()), t1000, SLOT (start()));

    m_thread->start (QThread::HighPriority);
}
