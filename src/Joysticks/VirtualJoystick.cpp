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

//=============================================================================
// VirtualJoystick::VirtualJoystick
//=============================================================================

VirtualJoystick::VirtualJoystick() {
    ui.setupUi (this);

    /* React to UI events */
    connect (ui.Close, SIGNAL (clicked()),
             this,     SLOT   (hide()));
    connect (ui.Range, SIGNAL (valueChanged   (double)),
             this,     SLOT   (OnRangeChanged (double)));
    connect (ui.UseKeyboardCheckbox, SIGNAL (clicked (bool)),
             this,                   SLOT   (SetVirtualJoystickEnabled (bool)));

    /* React to SDL events */
    connect (SDL_Layer::GetInstance(), SIGNAL (HatEvent       (GM_Hat)),
             this,                     SIGNAL (HatEvent       (GM_Hat)));
    connect (SDL_Layer::GetInstance(), SIGNAL (AxisEvent      (GM_Axis)),
             this,                     SIGNAL (AxisEvent      (GM_Axis)));
    connect (SDL_Layer::GetInstance(), SIGNAL (ButtonEvent    (GM_Button)),
             this,                     SIGNAL (ButtonEvent    (GM_Button)));
    connect (SDL_Layer::GetInstance(), SIGNAL (CountChanged   (int)),
             this,                     SLOT   (OnCountChanged (int)));
    connect (SDL_Layer::GetInstance(), SIGNAL (CountChanged   (QStringList)),
             this,                     SLOT   (OnCountChanged (QStringList)));

    /* React to key presses/releases */
    connect (&m_filter, SIGNAL (KeyEvent    (QKeyEvent*, bool)),
             this,      SLOT   (RegisterKey (QKeyEvent*, bool)));

    /* React to my own events */
    connect (this, SIGNAL (HatEvent      (GM_Hat)),
             this, SLOT   (OnHatEvent    (GM_Hat)));
    connect (this, SIGNAL (AxisEvent     (GM_Axis)),
             this, SLOT   (OnAxisEvent   (GM_Axis)));
    connect (this, SIGNAL (ButtonEvent   (GM_Button)),
             this, SLOT   (OnButtonEvent (GM_Button)));

    /* Configure the virtual joystick */
    m_joystick.numAxes = 6;
    m_joystick.numHats = 0;
    m_joystick.numButtons = 10;
    m_joystick.displayName = tr ("Virtual Joystick");

    /* Get key presses/releases from the application itself */
    qApp->installEventFilter (&m_filter);

    /* Resize the window to the smallest size */
    ResizeToFit();

    /* Initialize the SDL loop */
    SDL_Layer::GetInstance()->Init();
}

//=============================================================================
// VirtualJoystick::ReadSettings
//=============================================================================

void VirtualJoystick::ReadSettings() {
    ui.Range->setValue (Settings::Get ("Axis Range", 0.80).toDouble());
    SetVirtualJoystickEnabled (Settings::Get ("Virtual Joystick", false).toBool());
}

//=============================================================================
// VirtualJoystick::GetNumAxes
//=============================================================================

int VirtualJoystick::GetNumAxes (int js) {
    int count = SDL_Layer::GetInstance()->JoystickList().count();

    /* Application wants to get virtual joystick data */
    if (js >= count && m_enabled)
        return m_joystick.numAxes;

    /* Application wants to get SDL joystick data */
    else if (js < count)
        return SDL_Layer::GetInstance()->GetNumAxes (js);

    /* Joystick does not exist */
    return 0;
}

//=============================================================================
// VirtualJoystick::GetNumHats
//=============================================================================

int VirtualJoystick::GetNumHats (int js) {
    int count = SDL_Layer::GetInstance()->JoystickList().count();

    /* Application wants to get virtual joystick data */
    if (js >= count && m_enabled)
        return m_joystick.numHats;

    /* Application wants to get SDL joystick data */
    else if (js < count)
        return SDL_Layer::GetInstance()->GetNumHats (js);

    /* Joystick does not exist */
    return 0;
}

//=============================================================================
// VirtualJoystick::GetNumButtons
//=============================================================================

int VirtualJoystick::GetNumButtons (int js) {
    int count = SDL_Layer::GetInstance()->JoystickList().count();

    /* Application wants to get virtual joystick data */
    if (js >= count && m_enabled)
        return m_joystick.numButtons;

    /* Application wants to get SDL joystick data */
    else if (js < count)
        return SDL_Layer::GetInstance()->GetNumButtons (js);

    /* Joystick does not exist */
    return 0;
}

//=============================================================================
// VirtualJoystick::OnCountChanged
//=============================================================================

void VirtualJoystick::OnCountChanged (int count) {
    DriverStation::GetInstance()->ClearJoysticks();

    /* Register each joystick with the DS */
    for (int i = 0; i <= count - 1; ++i)
        DriverStation::GetInstance()->AddJoystick (
            SDL_Layer::GetInstance()->GetNumAxes    (i),
            SDL_Layer::GetInstance()->GetNumButtons (i),
            SDL_Layer::GetInstance()->GetNumHats    (i));

    /* Register the virtual joystick with the DS */
    if (m_enabled) {
        m_joystick.id = count;
        DriverStation::GetInstance()->AddJoystick (
            m_joystick.numAxes,
            m_joystick.numButtons,
            m_joystick.numHats);
    }

    /* Notify other objects that JS count changed */
    emit CountChanged (count + (m_enabled ? 1 : 0));
}

//=============================================================================
// VirtualJoystick::OnCountChanged
//=============================================================================

void VirtualJoystick::OnCountChanged (QStringList input) {
    /* Append the virtual joystick to the joystick list */
    if (m_enabled)
        input.append (m_joystick.displayName);

    /* Notify other objects that JS count changed */
    emit CountChanged (input);
}

//=============================================================================
// VirtualJoystick::RegisterKey
//=============================================================================

void VirtualJoystick::RegisterKey (QKeyEvent* event, bool pressed) {
    /* User pressed SPACE or SHIFT, trigger E-STOP */
    if (event->key() == Qt::Key_Shift || event->key() == Qt::Key_Space)
        DriverStation::GetInstance()->StartEmergencyStop();

    /* Parse keyboard data for the virtual joystick */
    ReadButtons (event->key(), pressed);
    ReadAxes    (event->key(), ui.Range->value(), pressed);

    event->ignore();
}

//=============================================================================
// VirtualJoystick::ReadAxes
//=============================================================================

void VirtualJoystick::ReadAxes (int key, double value, bool pressed) {
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

        emit AxisEvent (data);
    }
}

//=============================================================================
// VirtualJoystick::ReadButtons
//=============================================================================

void VirtualJoystick::ReadButtons (int key, bool pressed) {
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

        emit ButtonEvent (data);
    }
}

//=============================================================================
// VirtualJoystick::ResizeToFit
//=============================================================================

void VirtualJoystick::ResizeToFit() {
    resize (0, 0);
    setFixedSize (size());
}

//=============================================================================
// VirtualJoystick::OnRangeChanged
//=============================================================================

void VirtualJoystick::OnRangeChanged (double value) {
    Settings::Set ("Axis Range", value);
}

//=============================================================================
// VirtualJoystick::SetVirtualJoystickEnabled
//=============================================================================

void VirtualJoystick::SetVirtualJoystickEnabled (bool enabled) {
    m_enabled = enabled;

    Settings::Set ("Virtual Joystick", m_enabled);
    ui.UseKeyboardCheckbox->setChecked (m_enabled);

    QStringList list = SDL_Layer::GetInstance()->JoystickList();

    CountChanged (list);
    CountChanged (list.count());
}

//=============================================================================
// VirtualJoystick::OnHatEvent
//=============================================================================

void VirtualJoystick::OnHatEvent (GM_Hat hat) {
    DriverStation::GetInstance()->UpdateJoystickPOV (hat.joystick.id,
            hat.id,
            hat.angle);
}

//=============================================================================
// VirtualJoystick::OnAxisEvent
//=============================================================================

void VirtualJoystick::OnAxisEvent (GM_Axis axis) {
    DriverStation::GetInstance()->UpdateJoystickAxis   (axis.joystick.id,
            axis.id,
            axis.value);
}

//=============================================================================
// VirtualJoystick::OnButtonEvent
//=============================================================================

void VirtualJoystick::OnButtonEvent (GM_Button button) {
    DriverStation::GetInstance()->UpdateJoystickButton (button.joystick.id,
            button.id,
            button.pressed);
}
