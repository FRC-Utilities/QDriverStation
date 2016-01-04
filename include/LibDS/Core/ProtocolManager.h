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

#pragma once
#ifndef _LIB_DS_PROTOCOL_MANAGER_H
#define _LIB_DS_PROTOCOL_MANAGER_H

#include "LibDS/Core/Common.h"

class DS_ProtocolBase;

/**
 * \class DS_RobotManager
 *
 * Manages the avaiable protocols and configures them correctly, so that
 * the Driver Station can safely send commands and joystick input to the robot.
 *
 * The class also implements functions to register and condifure joystick input
 * automatically. Please note that you should implement a way to read joystick
 * input by yourself (be it a software-based or hardware based).
 */
class LIB_DS_DECL DS_ProtocolManager : public QObject
{
    Q_OBJECT

public:
    explicit DS_ProtocolManager();
    ~DS_ProtocolManager();

    /**
     * Returns the current protocol in use
     */
    DS_ProtocolBase* CurrentProtocol() const;

    /**
     * Returns \c true if the current protocol is initialized
     */
    bool ProtocolIsValid() const;

public slots:
    /**
     * Changes the protocol that we use to communicate with the robot
     */
    void SetProtocol (DS_ProtocolBase* CurrentProtocol);

    /**
     * Un-registeres all the joysticks from the Driver Station
     */
    void ClearJoysticks();

    /**
     * Registers a new joystick and its characteristics to the Driver Station
     */
    void AddJoystick (int axes, int buttons, int povHats);

    /**
     * Updates the state of the POV hats in the selected joystick
     */
    void UpdateJoystickPOV (int js, int hat, int angle);

    /**
     * Updates the state of the axes in the selected joystick
     */
    void UpdateJoystickAxis (int js, int axis, double value);

    /**
     * Updates the state of the buttons in the selected joystick
     */
    void UpdateJoystickButton (int js, int bt, bool status);

    /**
     * Sends the input \a data to the current protocol to decode
     */
    void ReadRobotPacket (QByteArray data);

signals:
    /**
     * Emitted when the protocol detects that the status of the
     * user code has changed
     */
    void CodeChanged (bool);

    /**
     * Emitted when the protocol changes its address.
     * This can be triggered by the user (or programmer) or the protocol
     * itself, when it figures out the IP address of the robot.
     */
    void RobotAddressChanged (QString);

    /**
     * Emitted when the connection state between the computer and the
     * robot radio is changed
     */
    void RadioCommChanged (bool);

    /**
     * Emitted when the state of the network communications with the robot
     * has been changed. Unlike the other signals with a \c bool value, this
     * signal contains more information about the communication status, such
     * as:
     *     - The robot responds ping requests, but does not respond to DS
     *     - The robot responds to ping requests and DS
     *     - The robot does not respond to ping requests nor the DS
     */
    void CommunicationsChanged (DS_CommStatus);

    /**
     * Emitted when the protocol detects that the robot voltage has changed
     */
    void VoltageChanged (QString);

    /**
     * Emitted when the protocol detects that the RAM usage of the robot
     * has changed
     */
    void CPUUsageChanged (int);

    /**
     * Emitted when the protocol detects that the RAM usage of the robot
     * has changed
     */
    void RAMUsageChanged (int);

    /**
     * Emitted when the protocol detects that the disk usage of the robot
     * has changed
     */
    void DiskUsageChanged (int);

    /**
     * Emitted when the protocol detects that the control mode has changed.
     * Note: this signal can be emitted when the user changes the control mode
     * or when the robot itself changes its mode (e.g. e-stop)
     */
    void ControlModeChanged (DS_ControlMode);

    /**
     * Emitted when the robot detects a possible voltage brownout
     */
    void VoltageBrownoutChanged (bool);

    /**
     * Emitted when the protocol receives and decodes a CAN data structure
     */
    void CANInfoReceived (DS_CAN);

    /**
     * Emitted when the client has just connected to the robot and downloaded
     * its library INI files and analyzed them
     */
    void LibVersionChanged (QString);

    /**
     * Emitted when the client has just connected to the robot and downloaded
     * its PCM INI files and analyzed them
     */
    void RIOVersionChanged (QString);

    /**
     * Emitted when the client has just connected to the robot and downloaded
     * its PDP information and analyzed them
     */
    void PDPVersionChanged (QString);

    /**
     * Emitted when the client has just connected to the robot and downloaded
     * the PCM information files and analyzed them
     */
    void PCMVersionChanged (QString);

private:
    /**
     * The current protocol being used
     */
    DS_ProtocolBase* m_protocol;

    /**
     * The joystick data, we only have one instance for the whole library
     */
    QList<DS_Joystick*>* m_joysticks;

    /**
     * Returns \c true if the \a js is registered with the joystick list
     */
    bool JoystickIsValid (int js) const;
};

#endif
