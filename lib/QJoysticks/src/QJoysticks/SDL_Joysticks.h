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

#ifndef _QJOYSTICKS_SDL_JOYSTICK_H
#define _QJOYSTICKS_SDL_JOYSTICK_H

#include <SDL.h>
#include <QObject>
#include <QJoysticks/JoysticksCommon.h>

class SDL_Joysticks : public QObject
{
    Q_OBJECT

public:
    explicit SDL_Joysticks();
    QList<QJoystickDevice*> joysticks();

public slots:
    void rumble (const QJoystickRumble& request);

signals:
    void countChanged();
    void POVEvent (const QJoystickPOVEvent& event);
    void axisEvent (const QJoystickAxisEvent& event);
    void buttonEvent (const QJoystickButtonEvent& event);

private slots:
    void update();
    void configureJoystick (const SDL_Event* event);

private:
    int getDynamicID (int id);

    QJoystickDevice* getJoystick (int id);
    QJoystickPOVEvent getPOVEvent (const SDL_Event* event);
    QJoystickAxisEvent getAxisEvent (const SDL_Event* event);
    QJoystickButtonEvent getButtonEvent (const SDL_Event* event);

    int m_tracker;
    QList<QJoystickDevice> m_joysticks;
};

#endif
