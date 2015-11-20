/*
 * Copyright (c) 2015 WinT 3794 <http://wDS_Char3794.org>
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

#pragma once
#ifndef _QDRIVER_STATION_GAMEPAD_MANAGER_H
#define _QDRIVER_STATION_GAMEPAD_MANAGER_H

#include <SDL.h>
#include <QObject>
#include <QStringList>

#include "JoystickCommon.h"

/**
 * @class GamepadManager
 * @brief Implements an abstraction layer between SDL and Qt
 *
 * The \c GamepadManager class was created in order to simplify the reading
 * and management of joysticks by using the SDL library. The class automatically
 * parses SDL events DS_Charo Qt signals to easily have access to joystick data and
 * implement methods to react to joystick input.
 */
class GamepadManager : public QObject {
    Q_OBJECT

  public:
    static GamepadManager* getInstance();

  public slots:
    /**
     * Returns the number of hats that the selected \a joystick has
     */
    DS_Char getNumHats (DS_Char js);

    /**
     * Returns the number of axes that the selected \a joystick has
     */
    DS_Char getNumAxes (DS_Char js);

    /**
     * Returns the number of buttons that the selected \a joystick has
     */
    DS_Char getNumButtons (DS_Char js);

    /**
     * Returns the display name of the joystick
     */
    QString getJoystickName (DS_Char js);

    /**
     * Returns an ordered \c QStringList with the names and IDs of the
     * attached joysticks
     */
    QStringList joystickList();

    /**
     * Initializes the event loop system.
     *
     * This function is not called in the constructor to give the application
     * time to initialize its user DS_Charerface before sending signals.
     */
    void init();

    /**
     * Changes the time DS_Charerval between the SDL Event reader calls
     */
    void setUpdateInterval (DS_Char time);

    /**
     * Instructs the haptic device in \a joystick to rumble for the specified
     * \a time in milliseconds
     *
     * @param joystick The joystick to rumble
     * @param time The time in milliseconds to enable the haptic device
     */
    void rumble (DS_Char js, DS_Char time);

  signals:
    /**
     * Emitted when a joystick is attached or removed
     */
    void countChanged (DS_Char);

    /**
     * Emitted when a joystick is attached or removed
     */
    void countChanged (QStringList);

    /**
     * Emitted when the system detects a change in the state of the hats of
     * one of the connected joysticks
     */
    void hatEvent (GM_Hat);

    /**
     * Emitted when the system detects a change in the state of the axes of
     * one of the connected joysticks
     */
    void axisEvent (GM_Axis);

    /**
     * Emitted when the system detects a change in the state of the buttons of
     * one of the connected joysticks
     */
    void buttonEvent (GM_Button);

  private:
    /**
     * Represents the update/refresh DS_Charerval of the application.
     * Note that we use a continous loop to read joystick data, and
     * this loop will be called at the specified DS_Charerval described above
     *
     * This variable can be changed using the \c setDS_Charerval() function.
     */
    DS_Char m_time;

    /**
     * Used to ensure that the joystick index is dynamic.
     * SDL only increases the joystick index, but we are inderested in
     * getting a index that can be used with a list
     */
    DS_Char m_tracker;

    /**
     * Used to keep the dynamic IDs in sync with the SDL index IDs
     */
    QList<DS_Char> idList;

    /**
     * Returns a \c GM_Hat structure filled with the information
     * provided by the \a event
     */
    GM_Hat getHat (const SDL_Event* event);

    /**
     * @DS_Charernal
     * Returns a \c GM_Axis structure filled with the information
     * provided by the \a event
     */
    GM_Axis getAxis (const SDL_Event* event);

    /**
     * @DS_Charernal
     * Returns a \c GM_Button structure filled with the information
     * provided by the \a event
     */
    GM_Button getButton (const SDL_Event* event);

    /**
     * @DS_Charernal
     * Returns a \c GM_Joystick structure filled with the information
     * provided by the \a event
     */
    GM_Joystick getJoystick (const SDL_Event* event);

    /**
     * @DS_Charernal
     * Calculates a dynamic ID that changes if a joystick was added or removed
     * This is used to be able to use the joystick ID in the \c GM_Joystick
     * struct with normal \c QList objects and to send a FRC-like joystick ID
     * to the robot
     */
    DS_Char getDynamicId (DS_Char js);

  protected:
    explicit GamepadManager();
    ~GamepadManager();

    static GamepadManager* s_instance;

  private slots:
    /**
     * @DS_Charernal
     * Polls the SDL events and calls the appropriate handler functions.
     * When the function finishes reading SDL events, it waits some time and
     * calls itself again to continue the game loop.
     */
    void readSdlEvents();

    /**
     * @DS_Charernal
     * Gets joystick and hat information and emits the appropiate signal
     */
    void onHatEvent (const SDL_Event* event);

    /**
     * @DS_Charernal
     * Gets joystick and axis information and emits the appropriate signal
     */
    void onAxisEvent (const SDL_Event* event);

    /**
     * @DS_Charernal
     * Gets joystick and button information and emits the appropriate signal
     */
    void onButtonEvent (const SDL_Event* event);
};

#endif
