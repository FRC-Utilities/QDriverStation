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

#include "VirtualJoystick.h"
#include "Global/Settings.h"

//=============================================================================
// VirtualJoystick::VirtualJoystick
//=============================================================================

VirtualJoystick::VirtualJoystick() {
    qApp->installEventFilter (this);

    m_axisRange            = 1;
    m_joystickEnabled      = false;

    m_joystick             = new QDS_InputDevice;
    m_joystick->numAxes    = 6;
    m_joystick->numPOVs    = 0;
    m_joystick->numButtons = 10;
    m_joystick->name       = tr ("Virtual Joystick");
}

//=============================================================================
// VirtualJoystick::~VirtualJoystick
//=============================================================================

VirtualJoystick::~VirtualJoystick() {
    delete m_joystick;
}

//=============================================================================
// VirtualJoystick::axisRange
//=============================================================================

float VirtualJoystick::axisRange() const {
    return m_axisRange;
}

//=============================================================================
// VirtualJoystick::joystickEnabled
//=============================================================================

bool VirtualJoystick::joystickEnabled() const {
    return m_joystickEnabled;
}

//=============================================================================
// VirtualJoystick::joystick
//=============================================================================

QDS_InputDevice* VirtualJoystick::joystick() const {
    return m_joystick;
}

//=============================================================================
// VirtualJoystick::setJoystickID
//=============================================================================

void VirtualJoystick::setJoystickID (int id) {
    m_joystick->id = id;
}

//=============================================================================
// VirtualJoystick::setAxisRange
//=============================================================================

void VirtualJoystick::setAxisRange (float range) {
    m_axisRange = range;
    Settings::set ("Axis Range", range);
}

//=============================================================================
// VirtualJoystick::setJoystickEnabled
//=============================================================================

void VirtualJoystick::setJoystickEnabled (bool enabled) {
    Settings::set ("Virtual Joystick", enabled);

    m_joystickEnabled = enabled;
    emit enabledChanged();
}

//=============================================================================
// VirtualJoystick::readAxes
//=============================================================================

void VirtualJoystick::readAxes (int key, bool pressed) {
    int axis = -1;
    float value = axisRange() * (pressed ? 1 : 0);

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

    if (axis != -1 && joystickEnabled()) {
        QDS_AxisEvent event;
        event.axis     = axis;
        event.value    = value;
        event.joystick = *joystick();

        emit axisEvent (event);
    }
}

//=============================================================================
// VirtualJoystick::readButtons
//=============================================================================

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

    if (button != -1 && joystickEnabled()) {
        QDS_ButtonEvent event;
        event.button   = button;
        event.pressed  = pressed;
        event.joystick = *joystick();

        emit buttonEvent (event);
    }
}

//=============================================================================
// VirtualJoystick::readShortcuts
//=============================================================================

void VirtualJoystick::readShortcuts (int key, bool pressed) {
    Q_UNUSED (pressed);

    if (key == Qt::Key_Space || key == Qt::Key_Shift)
        DS()->setEmergencyStop (true);
}

//=============================================================================
// VirtualJoystick::processKeyEvent
//=============================================================================

void VirtualJoystick::processKeyEvent (QKeyEvent* event, bool pressed) {
    readShortcuts (event->key(), pressed);

    if (joystickEnabled()) {
        readAxes    (event->key(), pressed);
        readButtons (event->key(), pressed);
    }
}

//=============================================================================
// VirtualJoystick::eventFilter
//=============================================================================

bool VirtualJoystick::eventFilter (QObject* object, QEvent* event) {
    Q_UNUSED (object);

    switch (event->type()) {
    case QEvent::KeyPress:
        processKeyEvent (static_cast <QKeyEvent*> (event), true);
        break;
    case QEvent::KeyRelease:
        processKeyEvent (static_cast <QKeyEvent*> (event), false);
        break;
    default:
        break;
    }

    return false;
}
