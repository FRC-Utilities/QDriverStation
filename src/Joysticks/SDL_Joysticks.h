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

#ifndef _QDS_SDL_JOYSTICK_H
#define _QDS_SDL_JOYSTICK_H

#include "SDL.h"
#include "JoysticksCommon.h"

/**
 * Implements a layer between Qt, the application and SDL to get
 * input from external joysticks.
 *
 * TODO:
 *   - Implement a 'unversal' mapping system so that the joystick
 *     mappings won't differ between drivers, operating systems
 *     and/or hardware. If possible, use the same mappings as
 *     the official FRC Driver Station
 */
class SDL_Joysticks : public QObject {
    Q_OBJECT

  public:
    explicit SDL_Joysticks();

    QStringList joystickNames();
    QList<QDS_InputDevice> joysticks();

  public slots:
    void rumble (QDS_RumbleRequest request);

  signals:
    void countChanged();
    void POVEvent (QDS_POVEvent event);
    void axisEvent (QDS_AxisEvent event);
    void buttonEvent (QDS_ButtonEvent event);

  private slots:
    void update();

  private:
    int getDynamicID (int id);

    QDS_InputDevice getJoystick (int id);
    QDS_POVEvent getPOVEvent (const SDL_Event* sdl_event);
    QDS_AxisEvent getAxisEvent (const SDL_Event* sdl_event);
    QDS_ButtonEvent getButtonEvent (const SDL_Event* sdl_event);

    int m_tracker;
    QList<QDS_InputDevice> m_joysticks;
};

#endif
