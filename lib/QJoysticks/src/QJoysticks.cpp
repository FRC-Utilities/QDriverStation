/*
 * Copyright (c) 2015-2016 Alex Spataru <alex_spataru@outlook.com>
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

#include <QDebug>
#include <QJoysticks.h>
#include <QJoysticks/SDL_Joysticks.h>
#include <QJoysticks/VirtualJoystick.h>

/**
 * \file QJoysticks.h
 * \class QJoysticks
 *
 * The \c QJoysticks class is the "god-object" of this system. It manages every
 * input system used by the application (e.g. SDL for real joysticks and
 * keyboard for virtual joystick) and communicates every module/input system
 * with the rest of the application through standarized types.
 *
 * The joysticks are assigned a numerical ID, which the \c QJoysticks can use to
 * identify them. The ID's start with \c 0 (as with a QList). The ID's are
 * refreshed when a joystick is attached or removed. The first joystick that
 * has been connected to the computer will have \c 0 as an ID, the second
 * joystick will have \c 1 as an ID, and so on...
 *
 * \note the virtual joystick will ALWAYS be the last joystick to be registered,
 *       even if it has been enabled before any SDL joystick has been attached.
 */

/**
 * Appended to the name of all joystick blacklisted
 */
const QString BLACKLISTED_STR = " (" + QObject::tr ("Disabled") + ")";

/**
 * Initializes and configures the SDL and virtual joystick modules.
 */
QJoysticks::QJoysticks()
{
    m_sdlJoysticks = new SDL_Joysticks();
    m_virtualJoystick = new VirtualJoystick();

    connect (sdlJoysticks(),    &SDL_Joysticks::POVEvent,
             this,              &QJoysticks::POVEvent);
    connect (sdlJoysticks(),    &SDL_Joysticks::axisEvent,
             this,              &QJoysticks::axisEvent);
    connect (sdlJoysticks(),    &SDL_Joysticks::buttonEvent,
             this,              &QJoysticks::buttonEvent);
    connect (sdlJoysticks(),    &SDL_Joysticks::countChanged,
             this,              &QJoysticks::updateInterfaces);

    connect (virtualJoystick(), &VirtualJoystick::povEvent,
             this,              &QJoysticks::POVEvent);
    connect (virtualJoystick(), &VirtualJoystick::axisEvent,
             this,              &QJoysticks::axisEvent);
    connect (virtualJoystick(), &VirtualJoystick::buttonEvent,
             this,              &QJoysticks::buttonEvent);
    connect (virtualJoystick(), &VirtualJoystick::enabledChanged,
             this,              &QJoysticks::updateInterfaces);

    connect (this, &QJoysticks::POVEvent,
             this, &QJoysticks::onPOVEvent);
    connect (this, &QJoysticks::axisEvent,
             this, &QJoysticks::onAxisEvent);
    connect (this, &QJoysticks::buttonEvent,
             this, &QJoysticks::onButtonEvent);
}

/**
 * Returns the one and only instance of this class
 */
QJoysticks* QJoysticks::getInstance()
{
    static QJoysticks joysticks;
    return &joysticks;
}

/**
 * Returns the number of joysticks that are attached to the computer and/or
 * registered with the \c QJoysticks system.
 *
 * \note This count also includes the virtual joystick (if its enabled)
 */
int QJoysticks::count() const
{
    return inputDevices().count();
}

/**
 * Returns the number of joysticks that are not blacklisted.
 * This can be considered the "effective" number of joysticks.
 */
int QJoysticks::nonBlacklistedCount()
{
    int cnt = count();

    for (int i = 0; i < count(); ++i)
        if (isBlacklisted (i))
            --cnt;

    return cnt;
}

/**
 * Returns the number of axes that the joystick at the given \a index has.
 *
 * \note If the joystick does not exist, this function will return \c 0
 */
int QJoysticks::getNumAxes (const int index)
{
    if (joystickExists (index))
        return getInputDevice (index)->numAxes;

    return 0;
}

/**
 * Returns the number of POVs that the joystick at the given \a index has.
 *
 * \note If the joystick does not exist, this function will return \c 0
 */
int QJoysticks::getNumPOVs (const int index)
{
    if (joystickExists (index))
        return getInputDevice (index)->numPOVs;

    return 0;
}

/**
 * Returns the number of buttons that the joystick at the given \a index has.
 *
 * \note If the joystick does not exist, this function will return \c 0
 */
int QJoysticks::getNumButtons (const int index)
{
    if (joystickExists (index))
        return getInputDevice (index)->numButtons;

    return 0;
}

/**
 * Returns \c true if the joystick at the given \a index is blacklisted.
 *
 * \note If the joystick does not exist, this function will also return \c true
 */
bool QJoysticks::isBlacklisted (const int index)
{
    if (joystickExists (index))
        return inputDevices().at (index)->blacklisted;

    return true;
}

/**
 * Returns a list with the names of all registered joystick.
 *
 * \note This list also includes the blacklisted joysticks
 * \note This list also includes the virtual joystick (if its enabled)
 */
QStringList QJoysticks::deviceNames() const
{
    QStringList names;

    foreach (QJoystickDevice* joystick, inputDevices())
        names.append (joystick->name);

    return names;
}

/**
 * Returns a pointer to a list containing all registered joysticks.
 * This can be used for advanced hacks or just to get all properties of each
 * joystick.
 */
QList<QJoystickDevice*> QJoysticks::inputDevices() const
{
    return m_devices;
}

/**
 * Returns a pointer to the SDL joysticks system.
 * This can be used if you need to get more information regarding the joysticks
 * registered and managed with SDL.
 */
SDL_Joysticks* QJoysticks::sdlJoysticks() const
{
    return m_sdlJoysticks;
}

/**
 * Returns a pointer to the virtual joystick system.
 * This can be used if you need to get more information regarding the virtual
 * joystick or want to change its properties directly.
 *
 * \note You can also change the properties of the virtual joysticks using the
 *       functions of the \c QJoysticks system class
 */
VirtualJoystick* QJoysticks::virtualJoystick() const
{
    return m_virtualJoystick;
}

/**
 * Returns a pointer to the device at the given \a index.
 *
 * \note If the joystick does not exist, the program will not crash, however,
 *       it will warn you through the console output
 */
QJoystickDevice* QJoysticks::getInputDevice (const int index)
{
    if (joystickExists (index))
        return inputDevices().at (index);

    /* Troll the user, haha! */
    QJoystickDevice* device = new QJoystickDevice;
    device->id = -1;
    device->numAxes = 0;
    device->numPOVs = 0;
    device->numButtons = 0;
    device->name = "Invalid";
    device->blacklisted = true;

    return device;
}

/**
 * Blacklists or whitelists the joystick at the given \a index.
 *
 * \note The joystick name will be changed based on the value of \a blacklisted
 * \note This function does not have effect if the given joystick does not exist
 */
void QJoysticks::setBlacklisted (const int index, const bool blacklisted)
{
    if (joystickExists (index)) {
        QString name = inputDevices().at (index)->name;

        if (blacklisted && !name.contains (BLACKLISTED_STR, Qt::CaseSensitive)) {
            name.append (BLACKLISTED_STR);

            for (int i = 0; i < inputDevices().at (index)->numAxes; ++i)
                emit axisChanged (index, i, 0);
            for (int i = 0; i < inputDevices().at (index)->numButtons; ++i)
                emit buttonChanged (index, i, false);
            for (int i = 0; i < inputDevices().at (index)->numPOVs; ++i)
                emit povChanged (index, i, 0);
        }

        else if (!blacklisted)
            name.chop (BLACKLISTED_STR.length());

        m_devices.at (index)->name = name;
        m_devices.at (index)->blacklisted = blacklisted;

        emit countChanged();
    }
}

/**
 * Changes the axis value range of the virtual joystick.
 *
 * Take into account that maximum axis values supported by the \c QJoysticks
 * system is from \c -1 to \c 1.
 */
void QJoysticks::setVirtualJoystickRange (const float range)
{
    virtualJoystick()->setAxisRange (range);
}

/**
 * Enables or disables the virtual joystick
 */
void QJoysticks::setVirtualJoystickEnabled (const bool enabled)
{
    virtualJoystick()->setJoystickEnabled (enabled);
}

/**
 * Removes all the registered joysticks and emits appropiate signals.
 */
void QJoysticks::resetJoysticks()
{
    m_devices.clear();
    emit countChanged();
}

/**
 * 'Rescans' for new/removed joysticks and registers them again.
 */
void QJoysticks::updateInterfaces()
{
    /* Remove all the joysticks */
    resetJoysticks();

    /* Register all SDL joysticks */
    foreach (QJoystickDevice* joystick, sdlJoysticks()->joysticks())
        addInputDevice (joystick);

    /* Register the virtual joystick */
    if (virtualJoystick()->joystickEnabled()) {
        addInputDevice (virtualJoystick()->joystick());
        virtualJoystick()->setJoystickID (inputDevices().count() - 1);
    }
}

/**
 * Registers the given \a device to the \c QJoysticks system
 */
void QJoysticks::addInputDevice (QJoystickDevice* device)
{
    if (device) {
        m_devices.append (device);
        emit countChanged();
    }
}

/**
 * Configures the QML-friendly signal based on the information given by the
 * \a event data
 */
void QJoysticks::onPOVEvent (const QJoystickPOVEvent& event)
{
    if (!isBlacklisted (event.joystick->id))
        emit povChanged (event.joystick->id, event.pov, event.angle);
}

/**
 * Configures the QML-friendly signal based on the information given by the
 * \a event data
 */
void QJoysticks::onAxisEvent (const QJoystickAxisEvent& event)
{
    if (!isBlacklisted (event.joystick->id))
        emit axisChanged (event.joystick->id, event.axis, event.value);
}

/**
 * Configures the QML-friendly signal based on the information given by the
 * \a event data
 */
void QJoysticks::onButtonEvent (const QJoystickButtonEvent& event)
{
    if (!isBlacklisted (event.joystick->id))
        emit buttonChanged (event.joystick->id, event.button, event.pressed);
}

/**
 * Returns \c true if the joystick at the given \a index is valid, otherwise,
 * the function returns \c false and warns the user through the console.
 */
bool QJoysticks::joystickExists (const int index)
{
    if ((index >= 0) && (count() > index))
        return true;

    qWarning() << "Hey shithead! You requested an invalid joystick:" << index;
    return false;
}
