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

#include <qdebug.h>
QByteArray DS_JoystickManager::getData()
{
    QByteArray data;

    /* Generate a data packet for each joystick */
    for (int js = 0; js < m_joysticks.count(); ++js) {
        int axesSize = m_joysticks.at (js)->axes.size();
        int buttonsSize = m_joysticks.at (js)->buttons.size();

        /* Put joystick information and section header */
        data.append (getJoystickSize (m_joysticks.at (js)) + 1);
        data.append (0x0c);

        /* Add axis data */
        data.append (axesSize);
        for (int axis = 0; axis < m_joysticks.at (js)->axes.count(); ++axis)
            data.append (m_joysticks.at (js)->axes.at (axis)->value * 127.5);

        /* Add button data */
        data.append (buttonsSize);
        for (int button = 0; button < m_joysticks.at (js)->buttons.count(); ++button)
            data.append ((char) m_joysticks.at (js)->buttons.at (button)->pressed);

        /* POVs */
        data.append (m_joysticks.at (js)->povs.size());
        for (int pov = 0; pov < m_joysticks.at (js)->povs.count(); ++pov)
            data.append (m_joysticks.at (js)->povs.at (pov)->angle);

        /* Finish packet data */
        data.append ((char) 0xff);
        data.append ((char) 0xff);
    }

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
        axis->value = 0;
        joystick->axes.append (axis);
    }

    /* Generate a list with all the buttons */
    for (int i = 0; i < buttons; ++i) {
        Button* button = new Button;
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

void DS_JoystickManager::updatePov (int joystick, int pov, short angle)
{
    /* The joystick is not registered, thus is invalid */
    if (m_joysticks.count() <= joystick)
        return;

    /* The button is not registered, thus is invalid */
    if (m_joysticks.at (joystick)->povs.count() <= pov)
        return;

    /* Conditions met, we can safely write data on the list */
    m_joysticks.at (joystick)->povs.at (pov)->angle = angle;
}

int DS_JoystickManager::getJoystickSize (const Joystick* joystick)
{
    return  5
            + (joystick->axes.size())
            + (joystick->buttons.size() / 8)
            + ((joystick->buttons.size() % 8 == 0) ? 0 : 1)
            + (joystick->povs.size() * 2);
}
