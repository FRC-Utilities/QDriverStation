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

DS_JoystickManager::DS_JoystickManager()
{
    m_currentJoystick = 0;
    m_tempJoystick = new Joystick;
}

DS_JoystickManager::~DS_JoystickManager()
{
    for (int i = 0; i < m_joysticks.count(); ++i) {
        delete m_joysticks.at (i)->axes;
        delete m_joysticks.at (i)->hats;
        delete m_joysticks.at (i)->buttons;
    }

    delete m_tempJoystick;
}

QByteArray DS_JoystickManager::getData()
{
    QByteArray data;
    QBitArray buttonData;

    for (int i = 0; i < m_joysticks.count(); ++i) {
        m_tempJoystick = m_joysticks.at (i);
        data.append ((uint8_t) getJoystickSize (m_tempJoystick) - 1);
        data.append ((uint8_t) 0x0c);

        /* Add axis data */
        data.append ((uint8_t) m_tempJoystick->numAxes);
        for (int i = 0; i < m_tempJoystick->numAxes; ++i)
            data.append ((uint8_t) m_tempJoystick->axes [i]);

        /* Add button data as bits*/
        data.append ((uint8_t) m_tempJoystick->numButtons);
        for (int i = 0; i < m_tempJoystick->numButtons; ++i)
            buttonData.setBit (i, m_tempJoystick->buttons [i]);

        /* Append button bits to the main data file */
        buttonData.setBit (m_tempJoystick->numButtons,
                           getButtonBytes (m_tempJoystick) * 8);
        data.append (bitsToBytes (buttonData));

        /* Add hat/pov data */
        data.append ((uint8_t) m_tempJoystick->numHats);
        for (int i = 0; i < m_tempJoystick->numHats; ++i) {
            QByteArray ba;
            ba.resize (1);
            ba.fill   (0);
            ba.append (m_tempJoystick->hats [i]);
            data.append (ba.at (0));
            data.append (ba.at (1));
        }
    }

    return data;
}

void DS_JoystickManager::removeAllJoysticks()
{
    m_joysticks.clear();
}

void DS_JoystickManager::removeJoystick (const short& js)
{
    if (m_joysticks.count() > js)
        m_joysticks.removeAt (js);
}

void DS_JoystickManager::addJoystick (const short& axes,
                                      const short& buttons,
                                      const short& hats)
{
    Joystick* js = new Joystick;

    js->numAxes = axes;
    js->numHats = hats;
    js->numButtons = buttons;

    js->axes = new char [js->numAxes];
    js->hats = new char [js->numHats];
    js->buttons = new bool [js->numButtons];

    for (int i = 0; i < js->numAxes; ++i)
        js->axes [i] = 0;

    for (int i = 0; i < js->numHats; ++i)
        js->hats [i] = -1;

    for (int i = 0; i < js->numButtons; ++i)
        js->buttons [i] = false;

    m_joysticks.append (js);
}

void DS_JoystickManager::updateHat (const short& js,
                                    const short& hat,
                                    const int& angle)
{
    if (m_joysticks.count() > js && angle >= 0 && angle <= 360)
        m_joysticks.at (js)->hats [hat] = angle;
}

void DS_JoystickManager::updateAxis (const short& js,
                                     const short& axis,
                                     const double& value)
{
    if (m_joysticks.count() > js)
        m_joysticks.at (js)->axes [axis] = value * (0xff / 2);
}

void DS_JoystickManager::updateButton (const short& js,
                                       const short& button,
                                       const bool& pressed)
{
    if (m_joysticks.count() > js)
        m_joysticks.at (js)->buttons [button] = pressed;
}

int DS_JoystickManager::getJoystickSize (const Joystick* joystick)
{
    return  5
            + (joystick->numAxes > 0 ? joystick->numAxes : 0)
            + (joystick->numButtons > 0 ? getButtonBytes (joystick) : 0)
            + (joystick->numHats > 0 ? joystick->numHats * 2 : 0);
}

int DS_JoystickManager::getButtonBytes (const Joystick* joystick)
{
    return  0
            + (joystick->numButtons / 8)
            + (joystick->numButtons % 8 == 0 ? 0 : 1);
}

QByteArray DS_JoystickManager::bitsToBytes (QBitArray bits)
{
    QByteArray bytes;
    bytes.resize (bits.count() / (8 + 1));
    bytes.fill (0);

    for (int i = 0; i < bits.count(); ++i)
        bytes [i / 8] = (bytes.at (i / 8) | ((bits [i] ? 1 : 0) << (i % 8)));

    return bytes;
}
