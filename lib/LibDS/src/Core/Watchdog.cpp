/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#include "Watchdog.h"

/**
 * @brief The Watchdog class
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
Watchdog::Watchdog()
{
    connect (&m_timer, SIGNAL (timeout()), this, SIGNAL (expired()));
}

/**
 * Returns the expiration time of the watchdog in milliseconds
 */
int Watchdog::expirationTime() const
{
    return m_timer.interval();
}

/**
 * Resets the watchdog and prevents it from expiring
 */
void Watchdog::reset()
{
    m_timer.stop();
    m_timer.start (expirationTime());
}

/**
 * Changes the expiration time and resets the watchdog
 */
void Watchdog::setExpirationTime (int msecs)
{
    m_timer.setInterval (msecs);
    reset();
}
