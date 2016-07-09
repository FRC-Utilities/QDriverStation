/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#ifndef _LIB_DS_WATCHDOG_H
#define _LIB_DS_WATCHDOG_H

#include <QTimer>

/**
 * \brief Implements a simple watchdog used to reset comms. when needed.
 *
 * The \c Watchdog class implements a simple software watchdog with the help of
 * the \c QTimer object.
 *
 * During normal operation, the program periodically resets the watchdog timer
 * to prevent it from expiring. If, due to an error, the program fails to reset
 * or feed the watchdog, it shall expire and emit the appropiate signals.
 *
 * The expiration signal is then received by the current protocol, which in
 * turn will reset itself and try to re-establish communications with the robot
 * controller and the FMS.
 */
class Watchdog : public QObject {
    Q_OBJECT

  signals:
    void expired();

  public:
    Watchdog();
    int expirationTime() const;

  public slots:
    void reset();
    void setExpirationTime (int msecs);

  private:
    QTimer m_timer;
};

#endif
