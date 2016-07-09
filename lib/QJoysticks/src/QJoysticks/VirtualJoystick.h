/*
 * Copyright (c) 2015-2016 Alex Spataru <alex_spataru@outlook.com>
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

#ifndef _QJOYSTICKS_JOYSTICK_H
#define _QJOYSTICKS_JOYSTICK_H

#include <QWidget>
#include <QKeyEvent>
#include <QApplication>
#include <QJoysticks/JoysticksCommon.h>

/**
 * \brief Translates keyboard input to joystick input
 *
 * This class implements a virtual joystick device that uses the computer's
 * keyboard as means to get the axis, button and POV values of the joystick.
 */
class VirtualJoystick : public QObject {
    Q_OBJECT

  signals:
    void enabledChanged();
    void povEvent (const QJoystickPOVEvent& event);
    void axisEvent (const QJoystickAxisEvent& event);
    void buttonEvent (const QJoystickButtonEvent& event);

  public:
    explicit VirtualJoystick();

    qreal axisRange() const;
    bool joystickEnabled() const;
    QJoystickDevice* joystick();

  public slots:
    void setJoystickID (int id);
    void setAxisRange (qreal range);
    void setJoystickEnabled (bool enabled);

  private slots:
    void readAxes (int key, bool pressed);
    void readPOVs (int key, bool pressed);
    void readButtons (int key, bool pressed);
    void processKeyEvent (QKeyEvent* event, bool pressed);

  protected:
    bool eventFilter (QObject* object, QEvent* event);

  private:
    qreal m_axisRange;
    bool m_joystickEnabled;
    QJoystickDevice m_joystick;
};

#endif
