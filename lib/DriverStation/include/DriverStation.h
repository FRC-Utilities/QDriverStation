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

#ifndef _DRIVER_STATION_MAIN_H
#define _DRIVER_STATION_MAIN_H

#include <QTimer>
#include <QObject>
#include <QStringList>
#include <QElapsedTimer>

#include "../src/Times.h"
#include "../src/Common.h"
#include "../src/Sender.h"
#include "../src/Receiver.h"
#include "../src/NetConsole.h"
#include "../src/VersionAnalyzer.h"
#include "../src/NetworkDiagnostics.h"

/**
 * @class DriverStation
 * @brief Connects, manages and controls a FRC robot
 */
class DriverStation : public QObject
{
    Q_OBJECT

public:
    /**
     * Returns the only instance of the class
     */
    static DriverStation* getInstance();

    /**
     * Returns \c true if the robot has communication and the code is loaded.
     * This function is useful in the case that you want to warn the user
     * when he/she clicks the 'Enable' button and something is not working
     */
    Q_INVOKABLE bool canBeEnabled();

    /**
     * Returns a list with the available alliances and positions that the robot
     * can have.
     *
     * This is used by the robot program to change its behaviour depending on
     * the alliance (Blue or Red) and position (1, 2 & 3) of the robot.
     */
    Q_INVOKABLE QStringList alliances();

    /**
     * Returns the current operation mode of the robot.
     * Possible return values are:
     *     - DS_Test
     *     - DS_TeleOp
     *     - DS_Disabled
     *     - DS_Autonomous
     *     - DS_EmergencyStop
     */
    Q_INVOKABLE DS_ControlMode operationMode();

    /**
     * Returns the IP address of the robot radio
     */
    Q_INVOKABLE QString radioAddress();

    /**
     * Returns the IP/DNS address of the roboRIO
     */
    Q_INVOKABLE QString roboRioAddress();

public slots:
    /**
     * Initializes the loops that send and recieve network packets between
     * the client and the robot.
     *
     * This function is not called in the constructor to give the application
     * time to initialize its user interface.
     */
    Q_INVOKABLE void init();

    /**
     * Reboots the roboRIO and changes the way the client sends packets to
     * the robot until it reloads its operating system
     */
    Q_INVOKABLE void reboot();

    /**
     * Sends a special packet to the robot so that it re-starts the process that
     * runs the robot software
     */
    Q_INVOKABLE void restartCode();

    /**
     * Changes the team number. Its a simple way to change the roboRIO address
     * without asking the final user to specify the 'full' address of the robot
     *
     * \note Changes may take up to one second to take full effect
     */
    Q_INVOKABLE void setTeamNumber (int team);

    /**
     * Updates the alliance of the robot by changing the way packets are
     * generated and sent to the roboRIO
     */
    Q_INVOKABLE void setAlliance (DS_Alliance alliance);

    /**
     * Updates the address that we use to communicate to the roboRIO.
     * Useful for teams that have changed the default network configurations
     * of their roboRIO component.
     */
    Q_INVOKABLE void setCustomAddress (QString address);

    /**
     * Changes the operation mode of the robot.
     * Available options are:
     *     - DS_Test
     *     - DS_TeleOp
     *     - DS_Disabled
     *     - DS_Autonomous
     *     - DS_EmergencyStop
     *
     * @note The elapsed time will be reset when the mode is switched
     */
    Q_INVOKABLE void setControlMode (DS_ControlMode mode);

    /**
     * [TODO]
     */
    Q_INVOKABLE void putJoystickData (DS_JoystickData joystickData);

    /**
     * Simulates a timed match with the input time values (in seconds)
     */
    Q_INVOKABLE void startPractice (int countdown, int autonomous,
                                    int delay, int teleop, int endgame);

signals:
    /**
     * Emitted when the client detects that the availability of the robot
     * software/code has changed
     */
    void codeChanged (bool available);

    /**
     * Emitted when the client detects that the availability of the roboRIO
     * has changed (eg. when the robot is powered off with the breaker)
     */
    void networkChanged (bool available);

    /**
     * Emitted when the client detects that the availability of the robot radio
     * has changed (eg. when the robot is powered off with the breaker)
     */
    void radioChanged (bool available);

    /**
     * Emitted when the client analyzes a packet from the roboRIO and extracts
     * the battery voltage of the robot.
     */
    void voltageChanged (QString voltage);

    /**
     * Emitted when the NetConsole receives and decodes a message from the
     * roboRIO
     */
    void newMessage (QString message);

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
     * Emitted when the libary detects that the RAM usage of the roboRIO has
     * changed since the last update.
     */
    void ramUsageChanged (int total, int used);

    /**
     * Emitted when the libary detects that the disk usage of the roboRIO has
     * changed since the last update.
     */
    void diskUsageChanged (int total, int used);

    /**
     * Emitted when the robot is enabled and the elapsed time
     * has been updated by the library.
     */
    void elapsedTimeChanged (QString elapsedTime);

protected:
    /**
     * Initializes the private variables of the class.
     * Call \c DriverStation::init() to finish initialization process
     */
    explicit DriverStation();

private:
    static DriverStation* m_instance;

    int m_team;
    bool m_code;
    bool m_init;
    bool m_radioStatus;
    bool m_oldConnection;
    bool m_justConnected;
    bool m_justDisconnected;

    QString m_oldStatus;
    QString m_newStatus;
    QElapsedTimer m_elapsedTime;

    DS_Status m_status;
    DS_ControlMode m_mode;
    DS_Alliance m_alliance;
    DS_JoystickData m_joystickData;

    DS_Sender m_sender;
    DS_Receiver m_receiver;
    DS_NetConsole m_netConsole;
    DS_VersionAnalyzer m_versionAnalyzer;
    DS_NetworkDiagnostics m_netDiagnostics;

private slots:
    /**
     * Returns a string with the current status of the robot.
     * Possible return values can be:
     *     - No Robot Code
     *     - No Robot Communication
     *     - TeleOp
     *     - Test
     *     - Autonomous
     *     - Emergency Stop
     *     - Disabled
     */
    QString getStatus();

    /**
     * Checks if the connection between the roboRIO and the client is
     * working correctly by "pinging" the roboRIO with a TCP/IP connection.
     *
     * If the function detects that the client just connected to the roboRIO,
     * it downloads robot information using FTP and analyzes the downloaded
     * information to emit singals to connected objects.
     *
     * If the function detects that the client just disconnected from the
     * roboRIO, it updates all the internal values to prevent sending data
     * and packets to the void and notifies the connected objects.
     *
     * This function is called once every second.
     */
    void checkConnection();

    /**
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
    void sendPacketsToRobot();

    /**
     * @internal
     * Updates the elapsed time with the time that has passed since the user
     * enabled the robot.
     *
     * @note The timer is reset when the control mode is changed
     */
    void updateElapsedTime();
};

#endif /* _DRIVER_STATION_MAIN_H */
