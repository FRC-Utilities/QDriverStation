/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#ifndef _WINDOW_H
#define _WINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <DriverStation.h>

namespace Ui {
class Window;
}

class Window : public QMainWindow {
    Q_OBJECT

  public:
    explicit Window (QWidget* parent = 0);
    ~Window();

  public slots:
    void onEnabledChanged();
    void updateControlMode();
    void updateVoltage (QString voltage);

  private:
    Ui::Window* m_ui;
    DriverStation* m_driverStation;
};

#endif
