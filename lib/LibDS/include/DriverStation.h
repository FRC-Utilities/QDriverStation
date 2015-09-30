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

#ifndef _LIB_DS_DRIVER_STATION_H
#define _LIB_DS_DRIVER_STATION_H

#include <QTimer>
#include <QObject>

#include "../src/headers/DS_Debug.h"
#include "../src/headers/DS_Client.h"
#include "../src/headers/DS_LogWindow.h"
#include "../src/headers/DS_NetConsole.h"
#include "../src/headers/DS_ElapsedTime.h"
#include "../src/headers/DS_RobotManager.h"
#include "../src/headers/DS_NetworkDiagnostics.h"

#include "../src/headers/DS_Protocol2015.h"

class DriverStation : public QObject {
    Q_OBJECT

public:
    explicit DriverStation();
    ~DriverStation();

    static DriverStation* getInstance();

    /**
     * Returns \c true if the robot has communication and the code is loaded.
     * This function is useful in the case that you want to warn the user
     * when he/she clicks the 'Enable' button and something is not working
     */
    bool canBeEnabled();

    /**
     * Returns a list with the available alliances and positions that the robot
     * can have.
     *
     * This is used by the robot program to change its behaviour depending on
     * the alliance (Blue or Red) and position (1, 2 & 3) of the robot.
     */
    QStringList alliances();

    /**
     * Returns the IP address of the robot radio
     */
    QString radioAddress();

    /**
     * Returns the network address of the robot
     */
    QString robotAddress();

    /**
     * Returns the current control mode of the robot
     */
    DS_ControlMode controlMode();

    /**
     * Returns \c true if the robot reports that the user code is loaded
     */
    bool robotHasCode();

    /**
     * Returns \c true if the DS is connected to the robot
     */
    bool networkAvailable();

public slots:
    /**
     * Initializes the class and the interlal loop/refresh system
     */
    void init();

    /**
     * Reboots the robot using the specified protocol
     */
    void reboot();

    /**
     * Restarts the robot code using the specified protocol
     */
    void restartCode();

    /**
     * What can I say, this function just shows the DS Log Window
     */
    void showLogWindow();


    /**
     * Simulates a timed match with the input time values (in seconds)
     */
    void startPractice (int countdown,
                        int autonomous,
                        int delay,
                        int teleop,
                        int endgame);

    /**
     * Changes the protocol that we use to control the robot
     */
    void setProtocol (DS_Protocol* protocol);

    /**
     * Changes the team number used by the protocol and network diagnostics
     */
    void setTeamNumber (int team);

    /**
     * Changes the alliance of the robot using the specified protocol
     */
    void setAlliance (DS_Alliance alliance);

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
    void setCustomAddress (QString address);

    /**
     * Un-registers all the joysticks from the Driver Station
     */
    void clearJoysticks();

    /**
     * Updates the \a angle of the selected \a hat in the specified \a josytick
     */
    void updateJoystickPovHat (int js, int hat, int angle);

    /**
     * Registers a new joystick to the Driver Station with the specified number
     * of \a axes, \a buttons and \a povHats
     */
    void addJoystick (int axes, int buttons, int povHats);

    /**
     * Updates the \a value of the selected \a axis in the specified \a josytick
     */
    void updateJoystickAxis (int js, int axis, double value);

    /**
     * Updates the \a state of the selected \a button in the specified \a josytick
     */
    void updateJoystickButton (int js, int button, bool state);

signals:
    /**
     * Emitted when the client detects that the availability of the robot
     * software/code has changed
     */
    void codeChanged (bool available);

    /**
     * Emitted when the library detects that the CPU usage of the robot has
     * changed
     */
    void cpuUsageChanged (int percent);

    /**
     * Emitted when the client detects that the availability of the robot
     * has changed (eg. when the robot is powered off with the breaker)
     */
    void networkChanged (bool available);

    /**
     * Emitted when the client detects that the availability of the robot radio
     * has changed (eg. when the robot is powered off with the breaker)
     */
    void radioChanged (bool available);

    /**
     * Emitted when the client analyzes a packet from the robot and extracts
     * the battery voltage of the robot.
     */
    void voltageChanged (QString voltage);

    /**
     * Emitted when the client analyzes a packet from the robot and extracts
     * the battery voltage of the robot.
     */
    void voltageChanged (double voltage);

    /**
     * Emitted when the NetConsole receives and decodes a message from the
     * robot
     */
    void newNetConsoleMessage (QString message);

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
     * Emitted when one of the following happens:
     *     - The code status changes
     *     - The communication status changes
     *     - The control mode of the robot is changed
     */
    void robotStatusChanged (QString status);

    /**
     * Emitted when the control mode is changed
     */
    void controlModeChanged (DS_ControlMode mode);

    /**
     * Emitted when the libary detects that the RAM usage of the robot has
     * changed since the last update.
     */
    void ramUsageChanged (int total, int used);

    /**
     * Emitted when the libary detects that the disk usage of the robot has
     * changed since the last update.
     */
    void diskUsageChanged (int total, int used);

    /**
     * Emitted when the robot is enabled and the elapsed time
     * has been updated by the library.
     */
    void elapsedTimeChanged (QString elapsedTime);

protected:
    static DriverStation* m_instance;

private:
    bool m_init;

    DS_Client m_client;
    DS_LogWindow m_logWindow;
    DS_RobotManager m_manager;
    DS_NetConsole m_netConsole;
    DS_ElapsedTime m_elapsedTime;
    DS_NetworkDiagnostics m_networkDiagnostics;

private slots:
    /**
     * @internal
     * Resets the internal values of the library when we disconnect from the
     * robot.
     */
    void resetInternalValues();

    /**
     * @internal
     * Checks if the connection between the robot and the client is
     * working correctly by "pinging" the robot with a TCP/IP connection.
     *
     * If the function detects that the client just connected to the robot,
     * it downloads robot information using FTP and analyzes the downloaded
     * information to emit singals to connected objects.
     *
     * If the function detects that the client just disconnected from the
     * robot, it updates all the internal values to prevent sending data
     * and packets to the void and notifies the connected objects.
     */
    void checkConnection();

    /**
     * @internal
     * Sends a 6-byte packet to the robot that contains:
     *     - Ping diagnostic data
     *     - Robot state command
     *     - Control bit (TeleOp, Autonomous, etc)
     *     - Alliance and position of the robot
     *
     * Additionaly, this function may send the robot a 27-byte packet that
     * contains the system ASCII timezone if the client has just established
     * a connection with the robot.
     *
     * This function is called once every 20 milliseconds.
     */
    void sendRobotPackets();

    /**
     * @internal
     * Changes the status string when:
     *     - The communications status has changed
     *     - The robot code status has changed
     *     - The control mode of the robot has changed
     */
    QString getStatus();

    /**
     * Updates the elapsed time when the control mode is changed
     */
    void onControlModeChanged (DS_ControlMode mode);
};

#endif
