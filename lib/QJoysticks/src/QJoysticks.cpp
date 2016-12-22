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
#include <QThread>
#include <QSettings>
#include <QJoysticks.h>
#include <QJoysticks/SDL_Joysticks.h>
#include <QJoysticks/VirtualJoystick.h>

QJoysticks::QJoysticks()
{
    /* Initialize input methods */
    m_sdlJoysticks = new SDL_Joysticks;
    m_virtualJoystick = new VirtualJoystick;

    /* Move SDL handler to another thread */
    m_thread = new QThread;
    m_thread->start (QThread::NormalPriority);
    m_sdlJoysticks->moveToThread (m_thread);

    /* Destroy the thread when it quits */
    connect (m_thread, SIGNAL (finished()), m_thread, SLOT (deleteLater()));

    /* Configure SDL joysticks */
    connect (sdlJoysticks(),    &SDL_Joysticks::POVEvent,
             this,              &QJoysticks::POVEvent);
    connect (sdlJoysticks(),    &SDL_Joysticks::axisEvent,
             this,              &QJoysticks::axisEvent);
    connect (sdlJoysticks(),    &SDL_Joysticks::buttonEvent,
             this,              &QJoysticks::buttonEvent);
    connect (sdlJoysticks(),    &SDL_Joysticks::countChanged,
             this,              &QJoysticks::updateInterfaces);

    /* Configure virtual joysticks */
    connect (virtualJoystick(), &VirtualJoystick::povEvent,
             this,              &QJoysticks::POVEvent);
    connect (virtualJoystick(), &VirtualJoystick::axisEvent,
             this,              &QJoysticks::axisEvent);
    connect (virtualJoystick(), &VirtualJoystick::buttonEvent,
             this,              &QJoysticks::buttonEvent);
    connect (virtualJoystick(), &VirtualJoystick::enabledChanged,
             this,              &QJoysticks::updateInterfaces);

    /* React to own signals to create QML signals */
    connect (this, &QJoysticks::POVEvent,
             this, &QJoysticks::onPOVEvent);
    connect (this, &QJoysticks::axisEvent,
             this, &QJoysticks::onAxisEvent);
    connect (this, &QJoysticks::buttonEvent,
             this, &QJoysticks::onButtonEvent);

    /* Configure the settings */
    m_sortJoyticks = 0;
    m_settings = new QSettings (qApp->organizationName(), qApp->applicationName());
    m_settings->beginGroup ("Blacklisted Joysticks");
}

QJoysticks::~QJoysticks()
{
    delete m_settings;
    delete m_sdlJoysticks;
    delete m_virtualJoystick;

    m_thread->exit();
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
 * Returns the number of axes that the joystick at the given \a index has.
 *
 * \note If the joystick does not exist, this function will return \c 0
 */
int QJoysticks::getNumAxes (int index)
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
int QJoysticks::getNumPOVs (int index)
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
int QJoysticks::getNumButtons (int index)
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
bool QJoysticks::isBlacklisted (int index)
{
    if (joystickExists (index))
        return inputDevices().at (index)->blacklisted;

    return true;
}

/**
 * Returns \c true if the joystick at the given \a index is valid, otherwise,
 * the function returns \c false and warns the user through the console.
 */
bool QJoysticks::joystickExists (int index)
{
    return (index >= 0) && (count() > index);
}

/**
 * Returns the name of the given joystick
 */
QString QJoysticks::getName (int index)
{
    if (joystickExists (index))
        return m_devices.at (index)->name;

    return "Invalid Joystick";
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
 */
QJoystickDevice* QJoysticks::getInputDevice (int index)
{
    if (joystickExists (index))
        return inputDevices().at (index);

    return Q_NULLPTR;
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
 * If \a sort is set to true, then the device list will put all blacklisted
 * joysticks at the end of the list
 */
void QJoysticks::setSortJoysticksByBlacklistState (bool sort)
{
    if (m_sortJoyticks != sort) {
        m_sortJoyticks = sort;
        updateInterfaces();
    }
}

/**
 * Blacklists or whitelists the joystick at the given \a index.
 *
 * \note This function does not have effect if the given joystick does not exist
 * \note Once the joystick is blacklisted, the joystick list will be updated
 */
void QJoysticks::setBlacklisted (int index, bool blacklisted)
{
    if (joystickExists (index)) {
        /* Netrualize the joystick */
        if (blacklisted) {
            for (int i = 0; i < inputDevices().at (index)->numAxes; ++i)
                emit axisChanged (index, i, 0);

            for (int i = 0; i < inputDevices().at (index)->numButtons; ++i)
                emit buttonChanged (index, i, false);

            for (int i = 0; i < inputDevices().at (index)->numPOVs; ++i)
                emit povChanged (index, i, 0);
        }

        /* See if blacklist value was actually changed */
        bool changed = m_devices.at (index)->blacklisted != blacklisted;

        /* Save settings */
        m_devices.at (index)->blacklisted = blacklisted;
        m_settings->setValue (getName (index), blacklisted);

        /* Re-scan joysticks if blacklist value has changed */
        if (changed)
            updateInterfaces();
    }
}

/**
 * 'Rescans' for new/removed joysticks and registers them again.
 */
void QJoysticks::updateInterfaces()
{
    m_devices.clear();

    /* Put blacklisted joysticks at the bottom of the list */
    if (m_sortJoyticks) {
        /* Register non-blacklisted SDL joysticks */
        foreach (QJoystickDevice* joystick, sdlJoysticks()->joysticks()) {
            joystick->blacklisted = m_settings->value (joystick->name, false).toBool();
            if (!joystick->blacklisted)
                addInputDevice (joystick);
        }

        /* Register the virtual joystick (if its not blacklisted) */
        if (virtualJoystick()->joystickEnabled()) {
            QJoystickDevice* joystick = virtualJoystick()->joystick();
            joystick->blacklisted = m_settings->value (joystick->name, false).toBool();

            if (!joystick->blacklisted) {
                addInputDevice (joystick);
                virtualJoystick()->setJoystickID (inputDevices().count() - 1);
            }
        }

        /* Register blacklisted SDL joysticks */
        foreach (QJoystickDevice* joystick, sdlJoysticks()->joysticks()) {
            joystick->blacklisted = m_settings->value (joystick->name, false).toBool();
            if (joystick->blacklisted)
                addInputDevice (joystick);
        }

        /* Register the virtual joystick (if its blacklisted) */
        if (virtualJoystick()->joystickEnabled()) {
            QJoystickDevice* joystick = virtualJoystick()->joystick();
            joystick->blacklisted = m_settings->value (joystick->name, false).toBool();

            if (joystick->blacklisted) {
                addInputDevice (joystick);
                virtualJoystick()->setJoystickID (inputDevices().count() - 1);
            }
        }
    }

    /* Sort normally */
    else {
        /* Register SDL joysticks */
        foreach (QJoystickDevice* joystick, sdlJoysticks()->joysticks()) {
            addInputDevice (joystick);
            joystick->blacklisted = m_settings->value (joystick->name, false).toBool();
        }

        /* Register virtual joystick */
        if (virtualJoystick()->joystickEnabled()) {
            QJoystickDevice* joystick = virtualJoystick()->joystick();
            joystick->blacklisted = m_settings->value (joystick->name, false).toBool();

            addInputDevice (joystick);
            virtualJoystick()->setJoystickID (inputDevices().count() - 1);
        }
    }

    emit countChanged();
}

/**
 * Changes the axis value range of the virtual joystick.
 *
 * Take into account that maximum axis values supported by the \c QJoysticks
 * system is from \c -1 to \c 1.
 */
void QJoysticks::setVirtualJoystickRange (qreal range)
{
    virtualJoystick()->setAxisRange (range);
}

/**
 * Enables or disables the virtual joystick
 */
void QJoysticks::setVirtualJoystickEnabled (bool enabled)
{
    virtualJoystick()->setJoystickEnabled (enabled);
}

/**
 * Removes all the registered joysticks and emits appropriate signals.
 */
void QJoysticks::resetJoysticks()
{
    m_devices.clear();
    emit countChanged();
}

/**
 * Registers the given \a device to the \c QJoysticks system
 */
void QJoysticks::addInputDevice (QJoystickDevice* device)
{
    if (device)
        m_devices.append (device);
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
