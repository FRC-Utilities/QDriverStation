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

#include "SDL_Joysticks.h"
#include "VirtualJoystick.h"
#include "JoystickManager.h"

//=============================================================================
// JoystickManager::JoystickManager
//=============================================================================

JoystickManager::JoystickManager()
{
    m_sdlJoysticks    = new SDL_Joysticks();
    m_virtualJoystick = new VirtualJoystick();

    /* Setup SDL joystick signals/slots */
    connect (sdlJoysticks(),    SIGNAL (POVEvent         (QDS_POVEvent)),
             this,              SIGNAL (POVEvent         (QDS_POVEvent)));
    connect (sdlJoysticks(),    SIGNAL (axisEvent        (QDS_AxisEvent)),
             this,              SIGNAL (axisEvent        (QDS_AxisEvent)));
    connect (sdlJoysticks(),    SIGNAL (buttonEvent      (QDS_ButtonEvent)),
             this,              SIGNAL (buttonEvent      (QDS_ButtonEvent)));
    connect (sdlJoysticks(),    SIGNAL (countChanged     (void)),
             this,                SLOT (updateInterfaces (void)));

    /* Setup virtual joystick signals/slots */
    connect (virtualJoystick(), SIGNAL (axisEvent        (QDS_AxisEvent)),
             this,              SIGNAL (axisEvent        (QDS_AxisEvent)));
    connect (virtualJoystick(), SIGNAL (buttonEvent      (QDS_ButtonEvent)),
             this,              SIGNAL (buttonEvent      (QDS_ButtonEvent)));
    connect (virtualJoystick(), SIGNAL (enabledChanged   (void)),
             this,                SLOT (updateInterfaces (void)));

    /* Send all joystick input to DS */
    connect (this,              SIGNAL (POVEvent         (QDS_POVEvent)),
             this,                SLOT (onPOVEvent       (QDS_POVEvent)));
    connect (this,              SIGNAL (axisEvent        (QDS_AxisEvent)),
             this,                SLOT (onAxisEvent      (QDS_AxisEvent)));
    connect (this,              SIGNAL (buttonEvent      (QDS_ButtonEvent)),
             this,                SLOT (onButtonEvent    (QDS_ButtonEvent)));
    connect (DS(),              SIGNAL (protocolChanged  (void)),
             this,                SLOT (updateInterfaces (void)));
}

//=============================================================================
// JoystickManager::~JoystickManager
//=============================================================================

JoystickManager::~JoystickManager()
{
    delete m_virtualJoystick;
}

//=============================================================================
// JoystickManager::joystickNames
//=============================================================================

QStringList JoystickManager::deviceNames()
{
    QStringList names;

    foreach (QDS_InputDevice joystick, inputDevices())
        names.append (joystick.name);

    return names;
}

//=============================================================================
// JoystickManager::joysticks
//=============================================================================

QList<QDS_InputDevice> JoystickManager::inputDevices()
{
    return m_devices;
}

//=============================================================================
// JoystickManager::getJoystick
//=============================================================================

QDS_InputDevice JoystickManager::getInputDevice (int id)
{
    if (inputDevices().count() > id)
        return inputDevices().at (id);

    QDS_InputDevice device;

    device.id         = -1;
    device.numAxes    = -1;
    device.numPOVs    = -1;
    device.numButtons = -1;
    device.name       = "Invalid";

    return device;
}

//=============================================================================
// JoystickManager::sdlJoystick
//=============================================================================

SDL_Joysticks* JoystickManager::sdlJoysticks()
{
    return m_sdlJoysticks;
}


//=============================================================================
// JoystickManager::virtualJoystick
//=============================================================================

VirtualJoystick* JoystickManager::virtualJoystick()
{
    return m_virtualJoystick;
}

//=============================================================================
// JoystickManager::resetJoysticks
//=============================================================================

void JoystickManager::resetJoysticks()
{
    m_devices.clear();
    DS()->resetJoysticks();

    emit countChanged();
}

//=============================================================================
// JoystickManager::updateInterfaces
//=============================================================================

void JoystickManager::updateInterfaces()
{
    resetJoysticks();

    foreach (QDS_InputDevice joystick, sdlJoysticks()->joysticks())
        addInputDevice (joystick);

    if (virtualJoystick()->joystickEnabled())
        {
            addInputDevice (*virtualJoystick()->joystick());
            virtualJoystick()->setJoystickID (inputDevices().count() - 1);
        }

    foreach (QDS_InputDevice joystick, inputDevices())
        {
            DS()->addJoystick (joystick.numAxes,
                               joystick.numButtons,
                               joystick.numPOVs);
        }
}

//=============================================================================
// JoystickManager::addJoystick
//=============================================================================

void JoystickManager::addInputDevice (QDS_InputDevice device)
{
    m_devices.append (device);
    emit countChanged();
}

//=============================================================================
// JoystickManager::onPOVEvent
//=============================================================================

void JoystickManager::onPOVEvent (QDS_POVEvent event)
{
    DS()->updateJoystickPOV (event.joystick.id,
                             event.pov,
                             event.angle);
}

//=============================================================================
// JoystickManager::onAxisEvent
//=============================================================================

void JoystickManager::onAxisEvent (QDS_AxisEvent event)
{
    DS()->updateJoystickAxis (event.joystick.id,
                              event.axis,
                              event.value);
}

//=============================================================================
// JoystickManager::onButtonEvent
//=============================================================================

void JoystickManager::onButtonEvent (QDS_ButtonEvent event)
{
    DS()->updateJoystickButton (event.joystick.id,
                                event.button,
                                event.pressed);
}
