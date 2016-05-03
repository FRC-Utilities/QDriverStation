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

#ifndef _LDS_WATCHDOG_H
#define _LDS_WATCHDOG_H

#include <QTimer>
#include "LibDS/Core/Common.h"

namespace DS_Core {

///
/// Implements a simple software watchdog with the help of the \c QTimer class.
///
/// During normal operation, the program periodically restarts the watchdog timer
/// to prevent it from expiring. If, due to hardware or software error, the program
/// fails to restart the watchdog timer, it shall expire and generate a timeout
/// signal.
///
/// The timeout signal is then received and interpreted by the current protocol,
/// which in turn will "reset" itself and try to re-establish communications
/// with the robot controller and (if needed) with the FMS.
///
class LIB_DS_DECL Watchdog : public QObject {
    Q_OBJECT

  public:
    explicit Watchdog();
    int expirationTime() const;

  public slots:
    void restart();
    void setTimeout (int time);

  signals:
    void timeout();

  private:
    QTimer m_timer;
};

}

#endif
