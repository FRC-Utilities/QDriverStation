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

#ifndef _DRIVER_STATION_JOYSTICK_MANAGER_H
#define _DRIVER_STATION_JOYSTICK_MANAGER_H

#include <QList>
#include <QObject>

/**
 * \class DS_JoystickManager
 *
 * The DS_JoystickManager class allows us to generate the data packets with
 * information about the joystick input, which is sent and processed by the
 * robot software.
 *
 * The DS_JoystickManager has built-in functions to register a joystick and its
 * components and update the joystick values safely.
 */
class DS_JoystickManager : public QObject
{
    Q_OBJECT

private:
    /**
     * @internal
     * Represents a joystick axis
     */
    struct Axis {
        double value;
    };

    /**
     * @internal
     * Represents a joystick button
     */
    struct Button {
        bool pressed;
    };

    /**
     * @internal
     * Represents a joystick POV
     */
    struct Pov {
        short angle;
    };

    /**
     * @internal
     * Represents a joystick, its used to access the values of the registered
     * joystick axes and buttons.
     */
    struct Joystick {
        int id;
        QList <Pov*> povs;
        QList <Axis*> axes;
        QList <Button*> buttons;
    };

    QList <Joystick*> m_joysticks;

public slots:
    /**
     * Returns a generated data packet that contains input information.
     * The generated data is then sent to the robot by the Driver Station.
     */
    QByteArray getData();

    /**
     * Removes all the registered joysticks from the Driver Station
     */
    void removeAllJoysticks();

    /**
     * Unregisters the selected \a joystick from the Driver Station
     */
    void removeJoystick (int joystick);

    /**
     * Registers a new joystick  to the Driver Station with the selected number
     * of \a axes and \a buttons
     */
    void addJoystick (int axes, int buttons);

    /**
     * Updates the \a value of the \a axis in the selected \a joystick
     */
    void updateAxis (int joystick, int axis, double value);

    /**
     * Updates the \a pressed state of the \a button in the selected \a joystick
     */
    void updateButton (int joystick, int button, bool pressed);

    /**
     * Updates the \a angle of the seleccted \a pov in the \a joystick
     */
    void updatePov (int joystick, int pov, short angle);

    /**
     * Returns the size of a joystick structure
     */
    int getJoystickSize (const Joystick* joystick);
};

#endif /* _DRIVER_STATION_JOYSTICK_MANAGER_H */
