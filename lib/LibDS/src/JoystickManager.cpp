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

#include "JoystickManager.h"

QByteArray DS_JoystickManager::getData()
{
    QByteArray data;

    /* TODO */

    return data;
}

void DS_JoystickManager::removeAllJoysticks()
{
    m_joysticks.clear();
}

void DS_JoystickManager::removeJoystick (int joystick)
{
    /* The joystick is not registered, thus is invalid */
    if (m_joysticks.count() <= joystick)
        return;

    /* We can safely remove the selected joystick from the list */
    m_joysticks.removeAt (joystick);
}

void DS_JoystickManager::addJoystick (int axes, int buttons)
{
    Joystick* joystick = new Joystick;

    /* Generate a list with all the axes */
    for (int i = 0; i < axes; ++i) {
        Axis* axis = new Axis;
        axis->id = i;
        axis->value = 0;
        joystick->axes.append (axis);
    }

    /* Generate a list with all the buttons */
    for (int i = 0; i < buttons; ++i) {
        Button* button = new Button;
        button->id = i;
        button->pressed = false;
        joystick->buttons.append (button);
    }

    /* Register the generated joystick */
    m_joysticks.append (joystick);
}

void DS_JoystickManager::updateAxis (int joystick, int axis, double value)
{
    /* The joystick is not registered, thus is invalid */
    if (m_joysticks.count() <= joystick)
        return;

    /* The axis in not registered, thus is invalid */
    if (m_joysticks.at (joystick)->axes.count() <= axis)
        return;

    /* Conditions met, we can safely write data on the list */
    m_joysticks.at (joystick)->axes.at (axis)->value = value;
}

void DS_JoystickManager::updateButton (int joystick, int button, bool pressed)
{
    /* The joystick is not registered, thus is invalid */
    if (m_joysticks.count() <= joystick)
        return;

    /* The button is not registered, thus is invalid */
    if (m_joysticks.at (joystick)->buttons.count() <= button)
        return;

    /* Conditions met, we can safely write data on the list */
    m_joysticks.at (joystick)->buttons.at (button)->pressed = pressed;
}
