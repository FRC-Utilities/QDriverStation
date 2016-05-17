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

#include <QJoysticks.h>

const QString _BLACKLISTED_STR = " (" + QObject::tr ("Disabled") + ")";

//==================================================================================================
// QJoysticks::JoystickManager
//==================================================================================================

QJoysticks::QJoysticks() {
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

//==================================================================================================
// QJoysticks::getInstance
//==================================================================================================

QJoysticks* QJoysticks::getInstance() {
    static QJoysticks joysticks;
    return &joysticks;
}

//==================================================================================================
// QJoysticks::count
//==================================================================================================

int QJoysticks::count() {
    return inputDevices().count();
}

//==================================================================================================
// QJoysticks::nonBlacklistedCount
//==================================================================================================

int QJoysticks::nonBlacklistedCount() {
    int cnt = count();

    for (int i = 0; i < count(); ++i)
        if (isBlacklisted (i))
            --cnt;

    return cnt;
}

//==================================================================================================
// QJoysticks::getNumAxes
//==================================================================================================

int QJoysticks::getNumAxes (int index) {
    if (index < inputDevices().count())
        return getInputDevice (index)->numAxes;

    return 0;
}

//==================================================================================================
// QJoysticks::getNumPOVs
//==================================================================================================

int QJoysticks::getNumPOVs (int index) {
    if (index < inputDevices().count())
        return getInputDevice (index)->numPOVs;

    return 0;
}

//==================================================================================================
// QJoysticks::getNumButtons
//==================================================================================================

int QJoysticks::getNumButtons (int index) {
    if (index < inputDevices().count())
        return getInputDevice (index)->numButtons;

    return 0;
}

//==================================================================================================
// QJoysticks::joystickNames
//==================================================================================================

QStringList QJoysticks::deviceNames() {
    QStringList names;

    foreach (QJoystickDevice* joystick, inputDevices())
        names.append (joystick->name);

    return names;
}

//==================================================================================================
// QJoysticks::joysticks
//==================================================================================================

QList<QJoystickDevice*> QJoysticks::inputDevices() const {
    return m_devices;
}

//==================================================================================================
// QJoysticks::isBlacklisted
//==================================================================================================

bool QJoysticks::isBlacklisted (int index) {
    if (index < inputDevices().count())
        return inputDevices().at (index)->blacklisted;

    return true;
}

//==================================================================================================
// QJoysticks::getJoystick
//==================================================================================================

QJoystickDevice* QJoysticks::getInputDevice (int id) {
    if (inputDevices().count() > id && id >= 0)
        return inputDevices().at (id);

    QJoystickDevice* device = new QJoystickDevice;
    device->id = -1;
    device->numAxes       = 0;
    device->numPOVs       = 0;
    device->numButtons    = 0;
    device->blacklisted   = true;
    device->name          = "Invalid";

    return device;
}

//==================================================================================================
// QJoysticks::sdlJoystick
//==================================================================================================

SDL_Joysticks* QJoysticks::sdlJoysticks() {
    return m_sdlJoysticks;
}

//==================================================================================================
// QJoysticks::virtualJoystick
//==================================================================================================

VirtualJoystick* QJoysticks::virtualJoystick() {
    return m_virtualJoystick;
}

//==================================================================================================
// QJoysticks::setBlacklisted
//==================================================================================================

void QJoysticks::setBlacklisted (int index, bool blacklisted) {
    if (inputDevices().count() > index) {
        QString name = inputDevices().at (index)->name;

        if (blacklisted && !name.contains (_BLACKLISTED_STR, Qt::CaseSensitive)) {
            name.append (_BLACKLISTED_STR);

            for (int i = 0; i < inputDevices().at (index)->numAxes; ++i)
                emit axisChanged (index, i, 0);

            for (int i = 0; i < inputDevices().at (index)->numButtons; ++i)
                emit buttonChanged (index, i, false);

            for (int i = 0; i < inputDevices().at (index)->numPOVs; ++i)
                emit povChanged (index, i, 0);
        }

        else if (!blacklisted)
            name.chop (_BLACKLISTED_STR.length());

        m_devices.at (index)->name = name;
        m_devices.at (index)->blacklisted = blacklisted;

        emit countChanged();
    }
}

//==================================================================================================
// QJoysticks::setVirtualJoystickRange
//==================================================================================================

void QJoysticks::setVirtualJoystickRange (float range) {
    virtualJoystick()->setAxisRange (range);
}

//==================================================================================================
// QJoysticks::setVirtualJoystickEnabled
//==================================================================================================

void QJoysticks::setVirtualJoystickEnabled (bool enabled) {
    virtualJoystick()->setJoystickEnabled (enabled);
}

//==================================================================================================
// QJoysticks::resetJoysticks
//==================================================================================================

void QJoysticks::resetJoysticks() {
    m_devices.clear();
    emit countChanged();
}

//==================================================================================================
// QJoysticks::updateInterfaces
//==================================================================================================

void QJoysticks::updateInterfaces() {
    resetJoysticks();

    foreach (QJoystickDevice* joystick, sdlJoysticks()->joysticks())
        addInputDevice (joystick);

    if (virtualJoystick()->joystickEnabled()) {
        addInputDevice (virtualJoystick()->joystick());
        virtualJoystick()->setJoystickID (inputDevices().count() - 1);
    }
}

//==================================================================================================
// QJoysticks::addJoystick
//==================================================================================================

void QJoysticks::addInputDevice (QJoystickDevice* device) {
    if (device != Q_NULLPTR) {
        m_devices.append (device);
        emit countChanged();
    }
}

//==================================================================================================
// QJoysticks::onPOVEvent
//==================================================================================================

void QJoysticks::onPOVEvent (QJoystickPOVEvent event) {
    if (isBlacklisted (event.joystick->id))
        return;

    emit povChanged (event.joystick->id, event.pov, event.angle);
}

//==================================================================================================
// QJoysticks::onAxisEvent
//==================================================================================================

void QJoysticks::onAxisEvent (QJoystickAxisEvent event) {
    if (isBlacklisted (event.joystick->id))
        return;

    emit axisChanged (event.joystick->id, event.axis, event.value);
}

//==================================================================================================
// QJoysticks::onButtonEvent
//==================================================================================================

void QJoysticks::onButtonEvent (QJoystickButtonEvent event) {
    if (isBlacklisted (event.joystick->id))
        return;

    emit buttonChanged (event.joystick->id, event.button, event.pressed);
}
