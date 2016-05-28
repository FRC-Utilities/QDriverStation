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

#ifndef _QJOYSTICKS_MAIN_H
#define _QJOYSTICKS_MAIN_H

#include <QObject>
#include <QJoysticks/SDL_Joysticks.h>
#include <QJoysticks/VirtualJoystick.h>
#include <QJoysticks/JoysticksCommon.h>

///
/// \brief The QJoysticks class
///
/// This class manages all input interfaces (SDL & virtual joystick) and implements
/// redundant functions for easier usage and for QML support.
///
/// Once the application has been initialized, connect the SIGNALS/SLOTS of this class
/// with the rest of your application and call \c init() to initialze the event system.
///
class QJoysticks : public QObject
{
    Q_OBJECT

public:
    ///
    /// Returns the only instance of the class
    ///
    static QJoysticks* getInstance();

    ///
    /// Returns the number of joysticks registered
    ///
    Q_INVOKABLE int count();

    ///
    /// Returns the number of joysticks registered that are not blacklisted
    ///
    Q_INVOKABLE int nonBlacklistedCount();

    ///
    /// Returns the number of axes in the given \a joystick index
    ///
    Q_INVOKABLE int getNumAxes (int index);

    ///
    ///Returns the number of POVs in the given \a joystick index
    ///
    Q_INVOKABLE int getNumPOVs (int index);

    ///
    ///Returns the number of buttons in the given \a joystick index
    ///
    Q_INVOKABLE int getNumButtons (int index);

    ///
    /// Returns a list with the names of all the input interfaces
    ///
    Q_INVOKABLE QStringList deviceNames();

    ///
    /// Returns a list with all the input interfaces
    ///
    QList<QJoystickDevice*> inputDevices() const;

    ///
    /// Returns \c true if the device in the \a index is set to
    /// be ignored by the application.
    ///
    Q_INVOKABLE bool isBlacklisted (int index);

    ///
    /// Returns the input interface with the given \a id
    ///
    QJoystickDevice* getInputDevice (int id);

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

    ///
    /// Changes the virtual joystick range directly
    ///
    void setVirtualJoystickRange (float range = 0);

    ///
    /// Changes the virtual joystick enabled state directly
    ///
    void setVirtualJoystickEnabled (bool enabled = false);

protected:
    explicit QJoysticks();

signals:
    ///
    /// Emitted when an interface is added or removed
    ///
    void countChanged();

    ///
    /// Emitted when the system detects a POV event from
    /// any of the connected interfaces/devices
    ///
    void POVEvent (QJoystickPOVEvent event);

    ///
    /// Emitted when the system detects an axis event from
    /// any of the connected interfaces/devices
    ///
    void axisEvent (QJoystickAxisEvent event);

    ///
    /// Emitted when the system detects a button event from
    /// any of the connected interfaces/devices
    ///
    void buttonEvent (QJoystickButtonEvent event);

    ///
    /// Emitted when the system detects a POV event from
    /// any of the connected interfaces/devices.
    ///
    /// \note This signal is QML friendly
    ///
    void povChanged (int js, int pov, int angle);

    ///
    /// Emitted when the system detects an axis event from
    /// any of the connected interfaces/devices
    ///
    /// \note This signal is QML friendly
    ///
    void axisChanged (int js, int axis, double value);

    ///
    /// Emitted when the system detects a button event from
    /// any of the connected interfaces/devices
    ///
    /// \note This signal is QML friendly
    ///
    void buttonChanged (int js, int button, bool pressed);

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
    /// Registers the \a device to the system
    ///
    void addInputDevice (QJoystickDevice* device);

    ///
    /// Allows us to emit the QML-friendly signal
    ///
    void onPOVEvent (QJoystickPOVEvent event);

    ///
    /// Allows us to emit the QML-friendly signal
    ///
    void onAxisEvent (QJoystickAxisEvent event);

    ///
    /// Allows us to emit the QML-friendly signal
    ///
    void onButtonEvent (QJoystickButtonEvent event);

private:
    SDL_Joysticks* m_sdlJoysticks;
    VirtualJoystick* m_virtualJoystick;

    QList<QJoystickDevice*> m_devices;
};

#endif
