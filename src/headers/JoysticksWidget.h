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
#ifndef _QDRIVER_STATION_JOYSTICKS_H
#define _QDRIVER_STATION_JOYSTICKS_H

#include "GamepadManager.h"
#include <ui_JoysticksWidget.h>

class QPushButton;
class QProgressBar;

/**
 * @class Joysticks
 * @brief Widget that shows joystick details
 *
 * The \c Joystick class implements a widget that allows the user to see the
 * connected joysticks and see the value of its axes and its buttons of the
 * selected joystick.
 *
 * This widget is used in the "Gamepad" tab of the MainWindow.
 */
class JoysticksWidget : public QWidget {
    Q_OBJECT

  public:
    explicit JoysticksWidget (QWidget* parent = 0);
    ~JoysticksWidget();

  signals:
    /**
     * Emitted when a joystick is removed from the computer
     */
    void joystickRemoved();

    /**
     * Emitted when a joystick is attached or removed from the computer
     *
     * \a joysticksAvailable is \c true if there is at least one joystick
     * attached to the computer.
     *
     * \a joysticksAvailable is \c false when there are no joystick attached
     * to the computer
     */
    void statusChanged (bool joysticksAvailable);

  private:
    /**
     * Represents the UI components of the widget
     */
    Ui::Joysticks ui;

    /**
     * Used for getting joystick input with SDL
     */
    GamepadManager* m_manager;

    /**
     * Represents the axes of the selected joystick
     */
    QList<QProgressBar*> m_axes;

    /**
     * Represents the buttons of the selected joystick
     */
    QList<QPushButton*> m_buttons;

  private slots:
    /**
     * @internal
     * Rumbles the currently selected joystick
     */
    void onRumbleClicked();

    /**
     * @internal
     * Re-creates the indicator widgets for the currently selected joystick
     */
    void onRowChanged (int row);

    /**
     * @internal
     * Re-populates the joystick list when the QJoystickManager detects that
     * the user attached or removed a joystick
     */
    void onCountChanged (const QStringList& list);

    /**
     * @internal
     * Sends the axis event information to the DriverStation and updates the
     * state of one of the axis indicators.
     */
    void onAxisEvent (const GM_Axis& axis);

    /**
     * @internal
     * Sends the button event information to the DriverStation and updates the
     * state of one of the button indicators.
     */
    void onButtonEvent (const GM_Button& button);
};

#endif
