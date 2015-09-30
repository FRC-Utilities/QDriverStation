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

#ifndef _LIB_DS_PROTOCOL_H
#define _LIB_DS_PROTOCOL_H

#include <QList>
#include <QString>
#include <QObject>
#include <QBitArray>
#include <QByteArray>

#include "DS_Debug.h"
#include "DS_Common.h"

/**
 * \class DS_Protocol
 *
 * Implements an abstract class to be used as a base for any protocol that
 * will be used to drive an FRC robot.
 */
class DS_Protocol : public QObject
{
    Q_OBJECT

public:
    explicit DS_Protocol();

    /**
     * Returns \c true if the user code is loaded on the robot
     */
    bool robotCode();

    /**
     * Returns the current alliance of the robot
     */
    DS_Alliance alliance();

    /**
     * Returns the current control mode of the robot
     */
    DS_ControlMode controlMode();

public slots:
    /**
     * Changes the team number of the robot, this can be used to generate
     * the robot and radio address.
     */
    void setTeamNumber (int team);

    /**
     * Changes the robot address to \a address
     */
    void setRobotAddress (QString address);

    /**
     * Updates the \a alliance of the robot
     */
    void setAlliance (DS_Alliance alliance);

    /**
     * Updates the control \a mode of the robot
     */
    void setControlMode (DS_ControlMode mode);

    /**
     * Indicates the protocol from where to read input from the \a joysticks
     */
    void setJoysticks (QList<DS_Joystick*>* joysticks);

    /* Functions that need to be implemented for each protocol */
    virtual void reset() = 0;
    virtual void reboot() = 0;
    virtual int robotPort() = 0;
    virtual int clientPort() = 0;
    virtual void restartCode() = 0;
    virtual QString robotAddress() = 0;
    virtual QString radioAddress() = 0;
    virtual void downloadRobotInformation() = 0;
    virtual QByteArray generateClientPacket() = 0;
    virtual QByteArray generateJoystickData() = 0;
    virtual void readRobotData (QByteArray data) = 0;
    virtual char getControlCode (DS_ControlMode mode) = 0;
    virtual char getAllianceCode (DS_Alliance alliance) = 0;

signals:
    /**
     * Emitted when the protocol detects that the status of the
     * user code has changed
     */
    void codeChanged (bool available);

    /**
     * Emitted when the protocol detects that the robot voltage has changed
     */
    void voltageChanged (double voltage);

    /**
     * Emitted when the protocol detects that the RAM usage of the robot
     * has changed
     */
    void ramUsageChanged (int total, int used);

    /**
     * Emitted when the protocol detects that the disk usage of the robot
     * has changed
     */
    void diskUsageChanged (int total, int used);

    /**
     * Emitted when the protocol detects that the control mode has changed.
     * Note: this signal can be emitted when the user changes the control mode
     * or when the robot itself changes its mode (e.g. e-stop)
     */
    void controlModeChanged (DS_ControlMode mode);

    /**
     * Emitted when the client has just connected to the robot and downloaded
     * its library INI files and analyzed them
     */
    void libVersionChanged (QString version);

    /**
     * Emitted when the client has just connected to the robot and downloaded
     * its PCM INI files and analyzed them
     */
    void rioVersionChanged (QString version);

    /**
     * Emitted when the client has just connected to the robot and downloaded
     * its PDP information and analyzed them
     */
    void pdpVersionChanged (QString version);

    /**
     * Emitted when the client has just connected to the robot and downloaded
     * the PCM information files and analyzed them
     */
    void pcmVersionChanged (QString version);

protected slots:
    /**
     * Converts the input \a bits to bytes
     */
    QByteArray bitsToBytes (QBitArray bits);

protected:
    int p_team;
    bool p_robotCode;
    QString p_robotAddress;
    QString p_radioAddress;
    DS_Alliance p_alliance;
    DS_ControlMode p_controlMode;
    QList<DS_Joystick*>* p_joysticks;
};

#endif
