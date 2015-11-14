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
#ifndef _LIB_DS_DRIVER_STATION_H
#define _LIB_DS_DRIVER_STATION_H

#include "LibDS/DS_Common.h"

class DS_Client;
class DS_Protocol;
class DS_NetConsole;
class DS_ElapsedTime;
class DS_ProtocolManager;

/**
 * \class DriverStation
 *
 * Implements all the functions and features of the LibDS library into a single,
 * compact and simple-to-use class.
 *
 * The \c DriverStation class also provides all the methods necessary to
 * manage the user input, protocol versions and operate the robot in a
 * straigthforward way.
 */
class LIB_DS_DECL DriverStation : public QObject
{
    Q_OBJECT
    Q_ENUMS (ControlMode)
    Q_ENUMS (ProtocolType)
    Q_ENUMS (AllianceType)

public:
    explicit DriverStation ();
    ~DriverStation ();

    static DriverStation* getInstance ();

    /**
     * The available protocols for the user
     *
     * \note We assign a value to the enums so that they can be used
     * with a list-based object
     */
    enum ProtocolType {
        Protocol2015 = 0
    };

    /**
     * The available alliances and positions for the user
     *
     * \note We assign a value to the enums so that they can be used
     * with a list-based object
     */
    enum AllianceType {
        Red1  = 0,
        Red2  = 1,
        Red3  = 2,
        Blue1 = 3,
        Blue2 = 4,
        Blue3 = 5
    };

    /**
     * The available control modes
     *
     * \note We assign a value to the enums so that they can be used
     * with a list-based object
     */
    enum ControlMode {
        Disabled      = 0,
        Teleoperated  = 1,
        Autonomous    = 2,
        Test          = 3,
        EmergencyStop = 4
    };

    /**
     * Returns \c true if the robot has communication and the code is loaded.
     * This function is useful in the case that you want to warn the user
     * when he/she clicks the 'Enable' button and something is not working
     */
    Q_INVOKABLE bool canBeEnabled ();

    /**
     * Returns a list with the available alliances and positions that the robot
     * can have.
     *
     * This is used by the robot program to change its behaviour depending on
     * the alliance (Blue or Red) and position (1, 2 & 3) of the robot.
     */
    Q_INVOKABLE QStringList alliances ();

    /**
     * Returns a list with the available protocols that we can use to drive
     * a FRC robot.
     */
    Q_INVOKABLE QStringList protocols ();

    /**
     * Returns the IP address of the robot radio
     */
    Q_INVOKABLE QString radioAddress ();

    /**
     * Returns the network address of the robot
     */
    Q_INVOKABLE QString robotAddress ();

    /**
     * Returns the current control mode of the robot
     */
    Q_INVOKABLE DS_ControlMode controlMode ();

    /**
     * Returns \c true if the robot reports that the user code is loaded
     */
    Q_INVOKABLE bool robotHasCode ();

    /**
     * Returns \c true if the DS is connected to the robot
     */
    Q_INVOKABLE bool networkAvailable ();

public slots:
    /**
     * Initializes the class and the interlal loop/refresh system
     */
    Q_INVOKABLE void init ();

    /**
     * Reboots the robot using the specified protocol
     */
    Q_INVOKABLE void reboot ();

    /**
     * Restarts the robot code using the specified protocol
     */
    Q_INVOKABLE void restartCode ();

    /**
     * Simulates a timed match with the input time values (in seconds)
     */
    Q_INVOKABLE void startPractice (int countdown,
                                    int autonomous,
                                    int delay,
                                    int teleop,
                                    int endgame);

    /**
     * Changes the protocol that we use to control the robot
     */
    Q_INVOKABLE void setProtocol (DS_Protocol* protocol);

    /**
     * Changes the protocol that we use to control the robot
     */
    Q_INVOKABLE void setProtocol (ProtocolType protocol);

    /**
     * Changes the team number used by the protocol and network diagnostics
     */
    Q_INVOKABLE void setTeamNumber (int team);

    /**
     * Changes the alliance of the robot using the specified protocol
     */
    Q_INVOKABLE void setAlliance (AllianceType allianceType);

    /**
     * Processes the input \a message and sends it to the other objects
     */
    Q_INVOKABLE void writeMessage (QString message);

    /**
     * Changes the control mode of the robot, available options are:
     *     - \c Disabled
     *     - \c Teleoperated
     *     - \c Autonomous
     *     - \c Test
     *     - \c EmergencyStop
     */
    Q_INVOKABLE void setControlMode (ControlMode mode);

    /**
     * Changes the control mode of the robot, available options are:
     *     - \c DS_Disabled
     *     - \c DS_Teleoperated
     *     - \c DS_Autonomous
     *     - \c DS_Test
     *     - \c DS_EmergencyStop
     */
    void setControlMode (DS_ControlMode mode);

    /**
     * Changes the network address of the robot
     */
    Q_INVOKABLE void setCustomAddress (QString address);

    /**
     * Un-registers all the joysticks from the Driver Station
     */
    Q_INVOKABLE void clearJoysticks ();

    /**
     * Updates the \a angle of the selected \a hat in the specified \a josytick
     */
    Q_INVOKABLE void updateJoystickPovHat (int js, int hat, int angle);

    /**
     * Registers a new joystick to the Driver Station with the specified number
     * of \a axes, \a buttons and \a povHats
     */
    Q_INVOKABLE void addJoystick (int axes, int buttons, int povHats);

    /**
     * Updates the \a value of the selected \a axis in the specified \a josytick
     */
    Q_INVOKABLE void updateJoystickAxis (int js, int axis, double value);

    /**
     * Updates the \a state of the selected \a button in the specified \a josytick
     */
    Q_INVOKABLE void updateJoystickButton (int js, int button, bool state);

signals:
    /**
     * Emitted when the client detects that the availability of the robot
     * software/code has changed
     */
    void codeChanged (bool);

    /**
     * Emitted when the library detects that the CPU usage of the robot has
     * changed
     */
    void cpuUsageChanged (int);

    /**
     * Emitted when the client detects that the availability of the robot
     * has changed (eg. when the robot is powered off with the breaker)
     */
    void communicationsChanged (bool);

    /**
     * Emitted when the client detects that the availability of the robot radio
     * has changed (eg. when the robot is powered off with the breaker)
     */
    void radioChanged (bool);

    /**
     * Emitted when the client analyzes a packet from the robot and extracts
     * the battery voltage of the robot.
     */
    void voltageChanged (QString);

    /**
     * Emitted when the NetConsole receives and decodes a message from the
     * robot
     */
    void newMessage (QString);

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

    /**
     * Emitted when one of the following happens:
     *     - The code status changes
     *     - The communication status changes
     *     - The control mode of the robot is changed
     */
    void robotStatusChanged (QString);

    /**
     * Emitted when the control mode is changed
     */
    void controlModeChanged (DS_ControlMode);

    /**
     * Emitted when the libary detects that the RAM usage of the robot has
     * changed since the last update.
     */
    void ramUsageChanged (int);

    /**
     * Emitted when the libary detects that the disk usage of the robot has
     * changed since the last update.
     */
    void diskUsageChanged (int);

    /**
     * Emitted when the robot is enabled and the elapsed time
     * has been updated by the library.
     */
    void elapsedTimeChanged (QString);

protected:
    static DriverStation* s_instance;

private:
    /**
     * This variable allows (or disallows) us to receive joystick
     * data and perform most of the operations of the Driver Station.
     *
     * This variable is set to \c true when the \c init() function is called
     */
    bool m_init;

    /**
     * The client, used for sending and receiving data
     * from a specified network address and port, go on
     * and crash the school's server if you wish
     */
    DS_Client* m_client;

    /**
     * Used for receiving messages broadcasted by the
     * robot over the network
     */
    DS_NetConsole* m_netConsole;

    /**
     * Allows us to select an operation protocol and
     * configure it automatically to fit the librarie's
     * standards
     */
    DS_ProtocolManager* m_manager;

    /**
     * Represents the current protocol in operation
     */
    DS_Protocol* m_protocol;

    /**
     * Counts the elapsed time since the robot was
     * enabled. When the robot is disabled, the elapsed
     * time is stopped (just as in the official Driver Station)
     */
    DS_ElapsedTime* m_elapsedTime;

private slots:
    /**
     * @brief sendRobotPackets
     */
    void sendRobotPackets ();

    /**
     * @internal
     * Resets the internal values of the library when we disconnect from the
     * robot.
     */
    void resetInternalValues ();

    /**
     * @internal
     * Sends the received data to the current protocol
     */
    void readRobotPackets (QByteArray robotResponse);

    /**
     * @internal
     * Changes the status string when:
     *     - The communications status has changed
     *     - The robot code status has changed
     *     - The control mode of the robot has changed
     */
    QString getStatus ();

    /**
     * @internal
     * Notifies other object when the status of the user code or the robot
     * communications changed. The \a ignore_me variable is ignored because
     * we use the \c getStatus() function to construct the status string
     * of the robot.
     */
    void updateStatus (bool ignored);

    /**
     * @internal
     * Updates the elapsed time when the control mode is changed
     */
    void onControlModeChanged (DS_ControlMode mode);
};

#endif
