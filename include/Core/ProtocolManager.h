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
#ifndef _LIB_DS_PROTOCOL_MANAGER_H
#define _LIB_DS_PROTOCOL_MANAGER_H

#include "Core/Common.h"

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
class LIB_DS_DECL DS_ProtocolManager : public QObject {
    Q_OBJECT

  public:
    explicit DS_ProtocolManager();
    ~DS_ProtocolManager();

    /**
     * Returns the current protocol in use
     */
    DS_ProtocolBase* protocol() const;

    /**
     * Returns \c true if the current protocol is initialized
     */
    bool protocolIsValid() const;

  public slots:
    /**
     * Changes the protocol that we use to communicate with the robot
     */
    void setProtocol (DS_ProtocolBase* protocol);

    /**
     * Un-registeres all the joysticks from the Driver Station
     */
    void clearJoysticks();

    /**
     * Registers a new joystick and its characteristics to the Driver Station
     */
    void addJoystick (DS_Char axes, DS_Char buttons, DS_Char povHats);

    /**
     * Updates the state of the POV hats in the selected joystick
     */
    void updateJoystickPovHat (DS_Char js, DS_Char hat, DS_Char angle);

    /**
     * Updates the state of the axes in the selected joystick
     */
    void updateJoystickAxis (DS_Char js, DS_Char axis, DS_Decimal value);

    /**
     * Updates the state of the buttons in the selected joystick
     */
    void updateJoystickButton (DS_Char js, DS_Char button, bool status);

    /**
     * Sends the input \a data to the current protocol to decode
     */
    void readRobotData (QByteArray data);

  signals:
    /**
     * Emitted when the protocol detects that the status of the
     * user code has changed
     */
    void codeChanged (bool);

    /**
     * Emitted when the protocol changes its address.
     * This can be triggered by the user (or programmer) or the protocol
     * itself, when it figures out the IP address of the robot.
     */
    void robotAddressChanged (QString);

    /**
     * Emitted when the state of the network communications with the robot
     * has been changed
     */
    void communicationsChanged (bool);

    /**
     * Emitted when the protocol detects that the robot voltage has changed
     */
    void voltageChanged (QString);

    /**
     * Emitted when the protocol detects that the RAM usage of the robot
     * has changed
     */
    void ramUsageChanged (int);

    /**
     * Emitted when the protocol detects that the disk usage of the robot
     * has changed
     */
    void diskUsageChanged (int);

    /**
     * Emitted when the protocol detects that the control mode has changed.
     * Note: this signal can be emitted when the user changes the control mode
     * or when the robot itself changes its mode (e.g. e-stop)
     */
    void controlModeChanged (DS_ControlMode);

    /**
     * Emitted when the client has just connected to the robot and downloaded
     * its library INI files and analyzed them
     */
    void libVersionChanged (QString);

    /**
     * Emitted when the client has just connected to the robot and downloaded
     * its PCM INI files and analyzed them
     */
    void rioVersionChanged (QString);

    /**
     * Emitted when the client has just connected to the robot and downloaded
     * its PDP information and analyzed them
     */
    void pdpVersionChanged (QString);

    /**
     * Emitted when the client has just connected to the robot and downloaded
     * the PCM information files and analyzed them
     */
    void pcmVersionChanged (QString);

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
    bool joystickIsValid (DS_Char js) const;
};

#endif
