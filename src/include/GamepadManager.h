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

#ifndef _QDRIVER_STATION_GAMEPAD_MANAGER_H
#define _QDRIVER_STATION_GAMEPAD_MANAGER_H

#include <SDL.h>
#include <QObject>
#include <QStringList>

/**
 * Represents a joystick and provides some information about it
 */
struct GM_Joystick {
    int id;              /**< The raw ID of the joystick */
    int numAxes;         /**< The number of axes that the joystick has */
    int numButtons;      /**< The number of buttons that the joystick has */
    QString displayName; /**< The user-friendly name of the joystick*/
};


/**
 * Represents the state of a joystick hat and provides some data about it
 */
struct GM_Hat {
    int rawId;            /**< The raw ID of the hat */
    short angle;          /**< The pressed angle of the hat */
    GM_Joystick joystick; /**< The joystick that the hat belongs to */
};

/**
 * Represents the state of a joystick axis and provides some data about it
 */
struct GM_Axis {
    int rawId;            /**< The raw ID of the axis */
    int processedId;      /**< The calculated ID based on the axis type */
    double value;         /**< The value (between -1 and 1) of the axis */
    QString identifier;   /**< The user-friendly name of the axis */
    GM_Joystick joystick; /**< The joystick that the axis belongs to */
};

/**
 * Represents the state of a joystick button and provides some data about it
 */
struct GM_Button {
    int rawId;            /**< The raw ID of the button */
    bool pressed;         /**< The current state of the button */
    int processedId;      /**< The calculated ID based on the button type */
    QString identifier;   /**< The user-friendly name of the button */
    GM_Joystick joystick; /**< The joystick that the button belongs to */
};

class DriverStation;

/**
 * @class GamepadManager
 * @brief Implements an abstraction layer between SDL and Qt
 *
 * The \c GamepadManager class was created in order to simplify the reading
 * and management of joysticks by using the SDL library. The class automatically
 * parses SDL events into Qt signals to easily have access to joystick data and
 * implement methods to react to joystick input.
 *
 * Finally, the class uses a 'generic' mapping when it detects that an
 * unsupported controller was attached to the computer.
 * The generated mapping changes its UUID and displayname for the controller
 * in question and is saved in a configuration folder in the home path
 * of the user.
 */
class GamepadManager : public QObject
{
    Q_OBJECT

public:
    /**
     * Returns the only instance of the class
     */
    static GamepadManager* getInstance();

public slots:
    /**
     * Returns the number of hats that the selected \a joystick has
     */
    int getNumHats (int js);

    /**
     * Returns the number of axes that the selected \a joystick has
     */
    int getNumAxes (int js);

    /**
     * Returns the number of buttons that the selected \a joystick has
     */
    int getNumButtons (int js);

    /**
     * Returns the display name of the axis in the joystick
     */
    QString getAxisName (int axis);

    /**
     * Returns the display name of the button in the joystick
     */
    QString getButtonName (int button);

    /**
     * Returns the display name of the joystick
     */
    QString getJoystickName (int js);

    /**
     * Returns an ordered \c QStringList with the names and IDs of the
     * attached joysticks
     */
    QStringList joystickList();

    /**
     * Initializes the event loop system.
     *
     * This function is not called in the constructor to give the application
     * time to initialize its user interface before sending signals.
     */
    void init();

    /**
     * Changes the time interval between the SDL Event reader calls
     */
    void setUpdateInterval (int time);

    /**
     * Instructs the haptic device in \a joystick to rumble for the specified
     * \a time in milliseconds
     *
     * @param joystick The joystick to rumble
     * @param time The time in milliseconds to enable the haptic device
     */
    void rumble (int js, int time);

signals:
    /**
     * Emitted when a joystick is attached or removed
     */
    void countChanged (int count);

    /**
     * Emitted when a joystick is attached or removed
     */
    void countChanged (QStringList joysticks);

    /**
     * Emitted when the system detects a change in the state of the hats of
     * one of the connected joysticks
     */
    void hatEvent (GM_Hat hat);

    /**
     * Emitted when the system detects a change in the state of the axes of
     * one of the connected joysticks
     */
    void axisEvent (GM_Axis axis);

    /**
     * Emitted when the system detects a change in the state of the buttons of
     * one of the connected joysticks
     */
    void buttonEvent (GM_Button button);

protected:
    /**
     * Initializes SDL, loads controller mappings and starts the event loop
     */
    explicit GamepadManager();

    /**
     * Closes all SDL controllers and quits the SDL system
     */
    ~GamepadManager();

private:
    int m_time;
    int m_tracker;

    DriverStation* m_ds;
    QString m_genericMapping;
    static GamepadManager* m_instance;

    QList<int> idList;

    /**
     * Returns a \c GM_Hat structure filled with the information
     * provided by the \a event
     */
    GM_Hat getHat (const SDL_Event* event);

    /**
     * @internal
     * Returns a \c GM_Axis structure filled with the information
     * provided by the \a event
     */
    GM_Axis getAxis (const SDL_Event* event);

    /**
     * @internal
     * Returns a \c GM_Button structure filled with the information
     * provided by the \a event
     */
    GM_Button getButton (const SDL_Event* event);

    /**
     * @internal
     * Returns a \c GM_Joystick structure filled with the information
     * provided by the \a event
     */
    GM_Joystick getJoystick (const SDL_Event* event);

    /**
     * @internal
     * Calculates a dynamic ID that changes if a joystick was added or removed
     * This is used to be able to use the joystick ID in the \c GM_Joystick
     * struct with normal \c QList objects and to send a FRC-like joystick ID
     * to the robot
     */
    int getDynamicId (int js);

private slots:
    /**
     * @internal
     * Polls the SDL events and calls the appropriate handler functions.
     * When the function finishes reading SDL events, it waits some time and
     * calls itself again to continue the game loop.
     */
    void readSdlEvents();

    /**
     * @internal
     * Gets joystick and hat information and emits the appropiate signal
     */
    void onHatEvent (const SDL_Event* event);

    /**
     * @internal
     * Gets joystick and axis information and emits the appropriate signal
     */
    void onAxisEvent (const SDL_Event* event);

    /**
     * @internal
     * Gets joystick and button information and emits the appropriate signal
     */
    void onButtonEvent (const SDL_Event* event);

    /**
     * @internal
     * Opens the game controller that was added in order to be able to use it
     * in the event loop.
     *
     * In the case that there are no mappings available for the controller, the
     * function will assign a 'generic' Xbox mapping to the controller and will
     * save such mapping in the filesystem for future use.
     */
    void onControllerAdded (const SDL_Event* event);

    /**
     * @internal
     * Closes the game controller that was removed
     */
    void onControllerRemoved (const SDL_Event* event);

    /**
     * @internal
     * Registers and configures all the attached joysticks to the Driver Staiton
     */
    void registerJoysticksToDriverStation (int joystickCount);
};

#endif /* _QDRIVER_STATION_GAMEPAD_MANAGER_H */
