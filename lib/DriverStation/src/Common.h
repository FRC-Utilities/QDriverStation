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

#ifndef _DRIVER_STATION_COMMON_H
#define _DRIVER_STATION_COMMON_H

class QString;

/**
 * Represents the available operation modes of the robot.
 * If you have ever participated in a FRC team, you will understand the
 * importance of defining how the robot will behave.
 */
enum DS_ControlMode {
    DS_Test = 0x05,          /**< Driver moves each component manually */
    DS_TeleOp = 0x04,        /**< The robot moves based on driver input */
    DS_Disabled = 0x00,      /**< The robot is idle */
    DS_Autonomous = 0x06,    /**< The robot uses pre-programmed instructions */
    DS_EmergencyStop = 0x80  /**< The robot stops and needs to reboot*/
};

/**
 * Represents the possible control bytes that we can send to the robot
 * to reboot the RoboRIO or to restart the robot code
 */
enum DS_Status {
    DS_Ok = 0x10,       /**< Do not reboot or do anything, let it be... */
    DS_NoCode = 0x14,   /**< Restart the robot code */
    DS_Rebooting = 0x18 /**< Reboot the RoboRIO */
};

/**
 * Represents the available alliances that the robot can have.
 * Its important to specify which alliance we use in order to tell
 * the robot program 'where it is' and communicate with the FMS correctly
 */
enum DS_Alliance {
    DS_Red1 = 0,  /**< Red 1, use control bit 0x0 */
    DS_Red2 = 1,  /**< Red 2, use control bit 0x1 */
    DS_Red3 = 2,  /**< Red 3, use control bit 0x2 */
    DS_Blue1 = 3, /**< Blue 1, use control bit 0x3 */
    DS_Blue2 = 4, /**< Blue 2, use control bit 0x4 */
    DS_Blue3 = 5  /**< Blue 3, use control bit 0x5 */
};

/**
 * Represents a joystick and its current state.
 * The idea behind this is that the program will record the current state
 * of a joystick and fill this structure with its data, which then
 * will be processed by the library and sent to the robot
 */
struct DS_JoystickData {
    int id;              /**< The joysticks that are attached */
    double axis_value;   /**< The values of each joystick axis */
    bool button_pressed; /**< The states of each joystick button */
};

/**
 * Returns a calculated IP address based on the team address.
 *
 * For example:
 *     - \c DS_GetStaticIp(3794, 1) would return \c 10.37.94.1
 *     - \c DS_GetStaticIp(3794, 2) would return \c 10.37.94.2
 *     - \c DS_GetStaticIp( 118, 3) would return \c 10.01.18.3
 *     - And so on...
 */
QString DS_GetStaticIp (int team, int host);

/**
 * Returns an user-friendly string given the inputed robot control mode
 */
QString DS_GetControlModeString (DS_ControlMode mode);

#endif /* _DRIVER_STATION_COMMON_H */
