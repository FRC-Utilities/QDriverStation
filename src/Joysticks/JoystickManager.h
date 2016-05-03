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

#ifndef _QDS_JOYSTICK_MANAGER_H
#define _QDS_JOYSTICK_MANAGER_H

#include "JoysticksCommon.h"

class SDL_Joysticks;
class VirtualJoystick;

///
/// The joystick manager class allows us to work with different
/// joystick/input interfaces in a simple and efficient way.
///
/// The \c JoystickManager class 'runs' all input interfaces
/// in parallel and emits a selection on 'common' signals to
/// the rest of the application, such as when a joystick
/// axis event is triggered.
///
class JoystickManager : public QObject {
    Q_OBJECT

  public:
    explicit JoystickManager();

    ///
    /// Returns a list with the names of all the input interfaces
    ///
    QStringList deviceNames();

    ///
    /// Returns a list with all the input interfaces
    ///
    QList<QDS_InputDevice*> inputDevices();

    ///
    /// Returns \c true if the device in the \a index is set to
    /// be ignored by the application.
    ///
    bool isBlacklisted (int index);

    ///
    /// Returns the input interface with the given \a id
    ///
    QDS_InputDevice* getInputDevice (int id);

    ///
    /// Gives us direct access to the SDL interface
    ///
    SDL_Joysticks* sdlJoysticks();

    ///
    /// Gives us direct access to the virtual joystick
    ///
    VirtualJoystick* virtualJoystick();

  public slots:
    ///
    /// Tells the system to ignore the selected joystick
    ///
    void setBlacklisted (int index, bool blacklist);

  signals:
    ///
    /// Emitted when an interface is added or removed
    ///
    void countChanged();

    ///
    /// Emitted when the system detects a POV event from
    /// any of the connected interfaces/devices
    ///
    void POVEvent (QDS_POVEvent event);

    ///
    /// Emitted when the system detects an axis event from
    /// any of the connected interfaces/devices
    ///
    void axisEvent (QDS_AxisEvent event);

    ///
    /// Emitted when the system detects a button event from
    /// any of the connected interfaces/devices
    ///
    void buttonEvent (QDS_ButtonEvent event);

  private slots:
    ///
    /// Removes/unregisters all interfaces from the system
    ///
    void resetJoysticks();

    ///
    /// Scans for new input devices and registers them to the system
    ///
    void updateInterfaces();

    ///
    /// Sends null values to the Driver Station for the selected \a device
    ///
    void resetDeviceInputs (int device);

    ///
    /// Registers the \a device to the system
    ///
    void addInputDevice (QDS_InputDevice* device);

    ///
    /// Sends any POV event to the DS for further processing
    ///
    void onPOVEvent (QDS_POVEvent event);

    ///
    /// Sends any axis event to the DS for further processing
    ///
    void onAxisEvent (QDS_AxisEvent event);

    ///
    /// Sends any button event to the DS for further processing
    ///
    void onButtonEvent (QDS_ButtonEvent event);

  private:
    SDL_Joysticks* m_sdlJoysticks;
    VirtualJoystick* m_virtualJoystick;

    QList<QDS_InputDevice*> m_devices;
};

#endif
