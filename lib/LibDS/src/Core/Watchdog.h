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
