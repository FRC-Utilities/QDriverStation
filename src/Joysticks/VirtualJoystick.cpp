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

#include "Global/Settings.h"

#include "SDL_Layer.h"
#include "VirtualJoystick.h"

#include <LibDS/DriverStation.h>

VirtualJoystick::VirtualJoystick() {
    ui.setupUi (this);

    connect (ui.Close, SIGNAL (clicked()),
             this,     SLOT   (hide()));
    connect (ui.Range, SIGNAL (valueChanged   (double)),
             this,     SLOT   (onRangeChanged (double)));
    connect (ui.UseKeyboardCheckbox, SIGNAL (clicked (bool)),
             this,                   SLOT   (setVirtualJoystickEnabled (bool)));

    connect (SDL_Layer::getInstance(), SIGNAL (hatEvent    (GM_Hat)),
             this,                     SIGNAL (hatEvent    (GM_Hat)));
    connect (SDL_Layer::getInstance(), SIGNAL (axisEvent   (GM_Axis)),
             this,                     SIGNAL (axisEvent   (GM_Axis)));
    connect (SDL_Layer::getInstance(), SIGNAL (buttonEvent (GM_Button)),
             this,                     SIGNAL (buttonEvent (GM_Button)));
    connect (SDL_Layer::getInstance(), SIGNAL (countChanged   (int)),
             this,                     SLOT   (onCountChanged (int)));
    connect (SDL_Layer::getInstance(), SIGNAL (countChanged   (QStringList)),
             this,                     SLOT   (onCountChanged (QStringList)));

    connect (&m_filter, SIGNAL (keyPress    (QKeyEvent*, bool)),
             this,      SLOT   (registerKey (QKeyEvent*, bool)));

    connect (this, SIGNAL (axisEvent     (GM_Axis)),
             this, SLOT   (onAxisEvent   (GM_Axis)));
    connect (this, SIGNAL (buttonEvent   (GM_Button)),
             this, SLOT   (onButtonEvent (GM_Button)));

    m_joystick.numAxes = 6;
    m_joystick.numHats = 0;
    m_joystick.numButtons = 10;
    m_joystick.displayName = tr ("Virtual Joystick");

    qApp->installEventFilter (&m_filter);

    resizeToFit();
    SDL_Layer::getInstance()->init();
}

void VirtualJoystick::readSettings() {
    ui.Range->setValue (Settings::get ("Axis Range", 0.80).toDouble());
    setVirtualJoystickEnabled (Settings::get ("Virtual Joystick", false).toBool());
}

//------------------------------------------------------------------------------
// 'MERGE' THE VIRTUAL JOYSTICK AND THE SDL (REAL) JOYSTICKS
//------------------------------------------------------------------------------

int VirtualJoystick::getNumAxes (int js) {
    int count = SDL_Layer::getInstance()->joystickList().count();

    if (js >= count && m_enabled)
        return m_joystick.numAxes;

    else if (js < count)
        return SDL_Layer::getInstance()->getNumAxes (js);

    return 0;
}

int VirtualJoystick::getNumHats (int js) {
    int count = SDL_Layer::getInstance()->joystickList().count();

    if (js >= count && m_enabled)
        return m_joystick.numHats;

    else if (js < count)
        return SDL_Layer::getInstance()->getNumHats (js);

    return 0;
}

int VirtualJoystick::getNumButtons (int js) {
    int count = SDL_Layer::getInstance()->joystickList().count();

    if (js >= count && m_enabled)
        return m_joystick.numButtons;

    else if (js < count)
        return SDL_Layer::getInstance()->getNumButtons (js);

    return 0;
}

void VirtualJoystick::onCountChanged (int count) {
    DriverStation::getInstance()->clearJoysticks();

    for (int i = 0; i <= count - 1; ++i)
        DriverStation::getInstance()->addJoystick (
            SDL_Layer::getInstance()->getNumAxes    (i),
            SDL_Layer::getInstance()->getNumButtons (i),
            SDL_Layer::getInstance()->getNumHats    (i));

    if (m_enabled) {
        m_joystick.id = count;
        DriverStation::getInstance()->addJoystick (
            m_joystick.numAxes,
            m_joystick.numButtons,
            m_joystick.numHats);
    }

    emit countChanged (count + (m_enabled ? 1 : 0));
}

void VirtualJoystick::onCountChanged (QStringList input) {
    if (m_enabled)
        input.append (m_joystick.displayName);

    emit countChanged (input);
}

//------------------------------------------------------------------------------
// READ AND PROCESS KEYBOARD INPUT
//------------------------------------------------------------------------------

void VirtualJoystick::registerKey (QKeyEvent* event, bool pressed) {
    if (event->key() == Qt::Key_Shift || event->key() == Qt::Key_Space)
        DriverStation::getInstance()->startEmergencyStop();

    readButtons (event->key(), pressed);
    readAxes (event->key(), ui.Range->value(), pressed);

    event->ignore();
}

void VirtualJoystick::readAxes (int key, double value, bool pressed) {
    int axis = -1;
    value *= pressed ? 1 : 0;

    /* Horizontal axis on thumb 1 */
    if (key == Qt::Key_D)
        axis = 0;
    else if (key == Qt::Key_A) {
        axis = 0;
        value *= -1;
    }

    /* Vertical axis on thumb 1 */
    if (key == Qt::Key_S)
        axis = 1;
    else if (key == Qt::Key_W) {
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

    /* Horizontal axis on thumb 2 */
    if (key == Qt::Key_L)
        axis = 4;
    else if (key == Qt::Key_J) {
        axis = 4;
        value *= -1;
    }

    /* Vertical axis on thumb 2 */
    if (key == Qt::Key_I)
        axis = 5;
    else if (key == Qt::Key_K) {
        axis = 5;
        value *= -1;
    }

    if (axis != -1 && m_enabled) {
        GM_Axis data;
        data.id = axis;
        data.value = value;
        data.joystick = m_joystick;

        emit axisEvent (data);
    }
}

void VirtualJoystick::readButtons (int key, bool pressed) {
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

    if (button != -1 && m_enabled) {
        GM_Button data;
        data.id = button;
        data.pressed = pressed;
        data.joystick = m_joystick;

        emit buttonEvent (data);
    }
}

//------------------------------------------------------------------------------
// MISC. FUNCTIONS
//------------------------------------------------------------------------------

void VirtualJoystick::resizeToFit() {
    resize (0, 0);
    setFixedSize (size());
}

void VirtualJoystick::onRangeChanged (double value) {
    Settings::set ("Axis Range", value);
}

void VirtualJoystick::setVirtualJoystickEnabled (bool enabled) {
    m_enabled = enabled;

    Settings::set ("Virtual Joystick", m_enabled);
    ui.UseKeyboardCheckbox->setChecked (m_enabled);

    QStringList list = SDL_Layer::getInstance()->joystickList();

    onCountChanged (list);
    onCountChanged (list.count());
}

//------------------------------------------------------------------------------
// REACT TO EVENTS AND SEND THEM TO THE DRIVER STATION
//------------------------------------------------------------------------------

void VirtualJoystick::onHatEvent (GM_Hat hat) {
    DriverStation::getInstance()->updateJoystickPovHat (hat.joystick.id,
            hat.id, hat.value);
}

void VirtualJoystick::onAxisEvent (GM_Axis axis) {
    DriverStation::getInstance()->updateJoystickAxis (axis.joystick.id,
            axis.id, axis.value);
}

void VirtualJoystick::onButtonEvent (GM_Button button) {
    DriverStation::getInstance()->updateJoystickButton (button.joystick.id,
            button.id, button.pressed);
}
