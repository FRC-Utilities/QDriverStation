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
#include <QBitArray>
#include <QByteArray>

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

public:
    DS_JoystickManager();
    ~DS_JoystickManager();

private:
    /**
     * @internal
     * Represents a joystick, its used to access the values of the registered
     * joystick axes and buttons.
     */
    struct Joystick {
        int numHats;
        int numAxes;
        int numButtons;
        char* axes;
        char* hats;
        bool* buttons;
    };

    int m_currentJoystick;

    Joystick* m_tempJoystick;
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
    void removeJoystick (const short& js);

    /**
     * Registers a new joystick  to the Driver Station with the selected number
     * of \a axes and \a buttons
     */
    void addJoystick (const short& axes, const short& buttons, const short& hats);


    /**
     * Updates the \a angle of the \a hat in the selected \a joystick
     */
    void updateHat (const short& js,
                    const short& hat,
                    const int& angle);

    /**
     * Updates the \a value of the \a axis in the selected \a joystick
     */
    void updateAxis (const short& js,
                     const short& axis,
                     const double& value);

    /**
     * Updates the \a pressed state of the \a button in the selected \a joystick
     */
    void updateButton (const short& js,
                       const short& button,
                       const bool& pressed);

    /**
     * Returns the size of a joystick structure
     */
    int getJoystickSize (const Joystick* joystick);

    /**
     * Returns the number of bits in a button structure of the selected
     * \a joystick
     */
    int getButtonBytes (const Joystick* joystick);

    /**
     * Converts the input \a bits to bytes
     */
    QByteArray bitsToBytes (QBitArray bits);
};

#endif /* _DRIVER_STATION_JOYSTICK_MANAGER_H */
