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

#include "Settings.h"
#include "GamepadManager.h"
#include "VirtualJoystick.h"

VirtualJoystick::VirtualJoystick()
{
    ui.setupUi (this);

    connect (ui.Close, SIGNAL (clicked()),
             this,     SLOT   (hide()));

    connect (ui.Range, SIGNAL (valueChanged   (double)),
             this,     SLOT   (onRangeChanged (double)));
    connect (ui.UseKeyboardCheckbox, SIGNAL (clicked (bool)),
             this,                   SLOT   (setVirtualJoystickEnabled (bool)));

    connect (GamepadManager::getInstance(), SIGNAL (hatEvent   (GM_Hat)),
             this,                          SIGNAL (hatEvent   (GM_Hat)));
    connect (GamepadManager::getInstance(), SIGNAL (axisEvent   (GM_Axis)),
             this,                          SIGNAL (axisEvent   (GM_Axis)));
    connect (GamepadManager::getInstance(), SIGNAL (buttonEvent (GM_Button)),
             this,                          SIGNAL (buttonEvent (GM_Button)));

    connect (GamepadManager::getInstance(), SIGNAL (countChanged   (int)),
             this,                          SLOT   (onCountChanged (int)));
    connect (GamepadManager::getInstance(), SIGNAL (countChanged   (QStringList)),
             this,                          SLOT   (onCountChanged (QStringList)));

    m_joystick.numAxes = 6;
    m_joystick.numButtons = 10;
    m_joystick.displayName = tr ("Virtual Joystick");

    resizeToFit();
    GamepadManager::getInstance()->init();
}

void VirtualJoystick::readSettings()
{
    ui.Range->setValue (Settings::get ("Axis Range", 1.0).toDouble());
    setVirtualJoystickEnabled (Settings::get ("Virtual Joystick", false).toBool());
}

//------------------------------------------------------------------------------
// 'MERGE' THE VIRTUAL JOYSTICK AND THE SDL (REAL) JOYSTICKS
//------------------------------------------------------------------------------

int VirtualJoystick::getNumAxes (int js)
{
    int count = GamepadManager::getInstance()->joystickList().count();

    if (js >= count && m_enabled)
        return m_joystick.numAxes;

    else if (js < count)
        return GamepadManager::getInstance()->getNumAxes (js);

    return 0;
}

int VirtualJoystick::getNumButtons (int js)
{
    int count = GamepadManager::getInstance()->joystickList().count();

    if (js >= count && m_enabled)
        return m_joystick.numButtons;

    else if (js < count)
        return GamepadManager::getInstance()->getNumButtons (js);

    return 0;
}

void VirtualJoystick::onCountChanged (int count)
{
    m_joystick.id = count;
    emit countChanged (count + (m_enabled ? 1 : 0));
}

void VirtualJoystick::onCountChanged (QStringList input)
{
    if (m_enabled)
        input.append (m_joystick.displayName);

    emit countChanged (input);
}

//------------------------------------------------------------------------------
// READ AND PROCESS KEYBOARD INPUT
//------------------------------------------------------------------------------

void VirtualJoystick::registerKeyPress (QKeyEvent* event)
{
    readButtons (event->key(), true);
    readAxes (event->key(), ui.Range->value());

    event->ignore();
}

void VirtualJoystick::registerKeyRelease (QKeyEvent* event)
{
    readAxes (event->key(), 0.0);
    readButtons (event->key(), false);

    event->ignore();
}

void VirtualJoystick::readAxes (int key, double value)
{
    int axis = -1;

    /* Vertical axis on thumb 1 */
    if (key == Qt::Key_W)
        axis = 0;
    else if (key == Qt::Key_S) {
        axis = 0;
        value *= -1;
    }

    /* Horizontal axis on thumb 1 */
    if (key == Qt::Key_D)
        axis = 1;
    else if (key == Qt::Key_A) {
        axis = 1;
        value *= -1;
    }

    /* Trigger 1 */
    if (key == Qt::Key_Up)
        axis = 2;
    else if (key == Qt::Key_Down) {
        axis = 2;
        value *= -1;
    }

    /* Trigger 2 */
    if (key == Qt::Key_PageUp)
        axis = 3;
    else if (key == Qt::Key_PageDown) {
        axis = 3;
        value *= -1;
    }

    /* Vertical axis on thumb 2 */
    if (key == Qt::Key_I)
        axis = 4;
    else if (key == Qt::Key_K) {
        axis = 4;
        value *= -1;
    }

    /* Horizontal axis on thumb 2 */
    if (key == Qt::Key_L)
        axis = 5;
    else if (key == Qt::Key_J) {
        axis = 5;
        value *= -1;
    }

    if (axis != -1) {
        GM_Axis data;
        data.rawId = axis;
        data.value = value;
        data.joystick = m_joystick;
        data.identifier = QString ("Axis %1").arg (axis);

        emit axisEvent (data);
    }
}

void VirtualJoystick::readButtons (int key, bool pressed)
{
    int button = -1;

    if (key == Qt::Key_0)
        button = 0;
    else if (key == Qt::Key_1)
        button = 1;
    else if (key == Qt::Key_2)
        button = 2;
    else if (key == Qt::Key_3)
        button = 3;
    else if (key == Qt::Key_4)
        button = 4;
    else if (key == Qt::Key_5)
        button = 5;
    else if (key == Qt::Key_6)
        button = 6;
    else if (key == Qt::Key_7)
        button = 7;
    else if (key == Qt::Key_8)
        button = 8;
    else if (key == Qt::Key_9)
        button = 9;

    if (button != -1) {
        GM_Button data;
        data.rawId = button;
        data.pressed = pressed;
        data.joystick = m_joystick;
        data.identifier = QString ("Button %1").arg (button);

        emit buttonEvent (data);
    }
}

//------------------------------------------------------------------------------
// MISC. FUNCTIONS
//------------------------------------------------------------------------------

void VirtualJoystick::resizeToFit()
{
    resize (0, 0);
    setFixedSize (size());
}

void VirtualJoystick::onRangeChanged (double value)
{
    Settings::set ("Axis Range", value);
}

void VirtualJoystick::setVirtualJoystickEnabled (bool enabled)
{
    m_enabled = enabled;

    Settings::set ("Virtual Joystick", m_enabled);
    ui.UseKeyboardCheckbox->setChecked (m_enabled);

    QStringList list = GamepadManager::getInstance()->joystickList();

    onCountChanged (list);
    onCountChanged (list.count());
}
