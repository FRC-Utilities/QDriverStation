/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#include "Watchdog.h"

Watchdog::Watchdog() {
    connect (&m_timer, SIGNAL (timeout()), this, SIGNAL (expired()));
}

/**
 * Returns the expiration time of the watchdog in milliseconds
 */
int Watchdog::expirationTime() const {
    return m_timer.interval();
}

/**
 * Resets the watchdog and prevents it from expiring
 */
void Watchdog::reset() {
    m_timer.stop();
    m_timer.start (expirationTime());
}

/**
 * Changes the expiration time and resets the watchdog
 */
void Watchdog::setExpirationTime (int msecs) {
    m_timer.setInterval (msecs);
    reset();
}
