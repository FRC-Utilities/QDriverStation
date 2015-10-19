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
#ifndef _LIB_DS_PROTOCOL_H
#define _LIB_DS_PROTOCOL_H

#include <QBitArray>
#include <QHostInfo>
#include <QNetworkReply>

#include "DS_Common.h"
#include "DS_Watchdog.h"

/**
 * \class DS_Protocol
 *
 * Implements an abstract class to be used as a base for any protocol that
 * will be used to drive an FRC robot.
 */
class LIB_DS_DECL DS_Protocol : public QObject
{
    Q_OBJECT

public:
    explicit DS_Protocol();
    ~DS_Protocol();

    /**
     * Returns \c true if the user code is loaded on the robot
     */
    bool robotCode() const;

    /**
     * Returns \c true if the robot communications are working
     */
    bool robotCommunication() const;

    /**
     * Returns the current alliance of the robot
     */
    DS_Alliance alliance() const;

    /**
     * Returns the current control mode of the robot
     */
    DS_ControlMode controlMode() const;

    /**
     * Returns the default radio address or the custom radio address
     */
    QString radioAddress();

    /**
     * Returns the default robot address or the custom robot address
     */
    QString robotAddress();

public slots:
    /**
     * @brief reset
     */
    void reset();

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

    /**
     * Reads the robot data and calls the appropiate functions to interpret it
     */
    void readRobotPacket (QByteArray& data);

    /* Functions that need to be implemented for each protocol */
    virtual void reboot() = 0;
    virtual int robotPort() = 0;
    virtual int clientPort() = 0;
    virtual void restartCode() = 0;
    virtual QByteArray getClientPacket() = 0;
    virtual DS_ControlMode getControlMode (char mode) = 0;

private slots:
    /**
     * Implements the necessary steps to reset the internal values of a protocol
     */
    virtual void resetProtocol() = 0;

    /**
     * Returns the default radio address
     */
    virtual QString defaultRadioAddress() = 0;

    /**
     * Returns the default robot address
     */
    virtual QString defaultRobotAddress() = 0;

    /**
     * Implements a method to get more information about the robot components
     */
    virtual void downloadRobotInformation() = 0;

    /**
     * Uses the joystick input information to generate a data array to be
     * sent along the client packet
     */
    virtual QByteArray generateJoystickData() = 0;

    /**
     * Generates the neccesary data to send the robot the current timezone
     * of the client DS
     */
    virtual QByteArray generateTimezoneData() = 0;

    /**
     * Interprets the received robot \a data
     */
    virtual void readRobotData (QByteArray data) = 0;

    /**
     * Returns the control code used by the protocol to represent the selected
     * control \a mode
     */
    virtual char getControlCode (DS_ControlMode mode) = 0;

    /**
     * Returns the code used by the protocol to represent the selected
     * \a alliance
     */
    virtual char getAllianceCode (DS_Alliance alliance) = 0;

signals:
    /**
     * Emitted when the protocol detects that the status of the
     * user code has changed
     */
    void codeChanged (bool available);

    /**
     * Emitted when the state of the network communications with the robot
     * has been changed
     */
    void communicationsChanged (bool available);

    /**
     * Emitted when the protocol detects that the robot voltage has changed
     */
    void voltageChanged (QString voltage);

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

    /**
     * Emitted when the Driver Station changes the robot address or the team number
     * Can be used internally or externally to update network addresses.
     */
    void robotAddressChanged (QString address);

    /**
     * Emitted when we receive a packet from the robot. Used by the library
     * to generate and send another control packet to the robot.
     */
    void packetReceived();

protected:
    /**
     * Converts the input \a bits to bytes
     */
    QByteArray bitsToBytes (QBitArray bits);

protected:
    /**
     * This is the team number, you may use it for a variety of purposes
     * This variable is changed with the \c setTeamNumber() function.
     */
    int p_team;

    /**
     * Represents the number of packets sent to the robot
     */
    short p_sentPackets;

    /**
     * Represents the number of received robot packets
     */
    short p_receivedPackets;

    /**
     * Represents the minimum length of a robot packet of a protocol
     */
    int p_minPacketLength;

    /**
     * This variable should be set to \c true when the user code is loaded
     */
    bool p_robotCode;

    /**
     * This variable should be set to \c true when the robot responds to
     * the packets that we send to the robot
     */
    bool p_robotCommunication;

    /**
     * The IP address of the robot, calculate it with the \c QHostInfo class
     */
    QString p_robotIp;

    /**
     * The network address of the robot component (e.g. cRIO or roboRIO)
     * This variable is overwritten with the \c setRobotAddress() function.
     */
    QString p_robotAddress;

    /**
     * The radio address of the the robot (generally 10.XX.YY.1)
     */
    QString p_radioAddress;

    /**
     * The current alliance of the robot.
     * This variable is changed with the \c setAlliance() function.
     */
    DS_Alliance p_alliance;

    /**
     * The watchdog, used to reset internal values and refresh data when
     * robot is not present or does not respond
     */
    DS_Watchdog* p_watchdog;

    /**
     * The current control mode of the robot.
     * This variable is changed with the \c setControlMode() function.
     */
    DS_ControlMode p_controlMode;

    /**
     * A list with the input data of the joysticks. This is just a reference
     * to the joystick list in the protocol manager.
     */
    QList<DS_Joystick*>* p_joysticks;
};

#endif
