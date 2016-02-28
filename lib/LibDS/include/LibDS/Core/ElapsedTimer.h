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

namespace DS_Core {

///
/// This class calculates the elapsed time since the robot was enabled, disabled
/// and/or changed control modes.
///
/// After the elapsed time is obtained, the class formats the data as a human-readable
/// string and calls the appropiate signals to notify the \c DriverStation.
///
/// While the library does not make use of this class, you can use the information provided
/// to update the user interface controls of the application (e.g. like the QDriverStation does).
///
class LIB_DS_DECL ElapsedTime : public QObject {
    Q_OBJECT

  public slots:
    ///
    /// Pauses the timer.
    /// This function is called when the user disables the robot
    ///
    void stopTimer();

    ///
    /// Clears the elapsed time and starts counting again.
    /// This function is called when the user switches operation modes.
    ///
    void resetTimer();

  signals:
    ///
    /// Emitted periodically containing the human-readable time format.
    /// This signal is received by the \c DriverStation, which, in turn, sends it
    /// to the rest of the application for further use.
    ///
    void elapsedTimeChanged (QString);

  public:
    explicit ElapsedTime (QObject* parent);

  private:
    bool m_enabled;
    QElapsedTimer m_time;

  private slots:
    void getElapsedTime();
};

}

#endif

