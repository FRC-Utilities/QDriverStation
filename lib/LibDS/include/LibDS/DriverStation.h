/*
 * Copyright (c) 2015-2016 WinT 3794 <http://wint3794.org>
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

#ifndef _LDS_DRIVER_STATION_H
#define _LDS_DRIVER_STATION_H

#include "LibDS/Core/Common.h"

namespace DS_Core {
class AbstractProtocol;
class NetConsole;
class ElapsedTime;
}

///
/// This class implements all the functions and external features of the LibDS
/// library into a single, compact and easy to use class.
///
/// Aside from presenting the LibDS features to the user, this class is also
/// in charge of regulating and syncronizing each component of the LibDS so
/// that everything works together.
///
/// Please note that this class contains some reduntant functions to ease its
/// usage for both C++ and QML clients.
///
class LIB_DS_DECL DriverStation : public QObject {
    Q_OBJECT
    Q_ENUMS (ProtocolType)

  public:
    ///
    /// Returns the only instance of the \c DriverStation.
    /// Initiating more than one instance of the Driver Station at
    /// a given time will result in loss of communication with the robot
    /// and (possibly) slow network performance.
    ///
    static DriverStation* getInstance();

    ///
    /// Represents the protocols that have been already implemented by
    /// the library. Note that you can implement your own protocol by
    /// subclassing the \c ProtocolBase library.
    ///
    /// We need to assign a value to each \c enum in order to be able
    /// to use the library with QML projects.
    ///
    enum ProtocolType {
        kFRCProtocol2016 = 0,
        kFRCProtocol2015 = 1,
        kFRCProtocol2014 = 2
    };

    ///
    /// Returns \c true if the current protocol is valid and does not
    /// point to a NULL pointer
    ///
    Q_INVOKABLE bool protocolLoaded();

    ///
    /// Returns \c true if the DS has full communications with the robot and
    /// the user code is already loaded on the robot.
    ///
    /// This function is useful for client implementations to know if its safe
    /// (and possible) to enable the robot.
    ///
    Q_INVOKABLE bool canBeEnabled();

    ///
    /// Returns a list with the available alliances that the robot can have.
    ///
    /// \todo Let each protocol define the available alliances and positions,
    ///       as this architecture could change in future seasons.
    ///
    Q_INVOKABLE QStringList alliances();

    ///
    /// Returns a list with the protocols that have been already implemented
    /// in the LibDS library. Please note that the order of the protocols
    /// MUST be the same as the values presented in the \c ProtocolType enum.
    ///
    Q_INVOKABLE QStringList protocols();

    ///
    /// Returns the current IP address of the robot radio/modem.
    ///
    Q_INVOKABLE QString radioAddress();

    ///
    /// Returns the current IP address of the robot controller.
    ///
    Q_INVOKABLE QString robotAddress();

    ///
    /// Returns the default network address of the robot
    ///
    Q_INVOKABLE QString defaultRobotAddress();

    ///
    /// Returns the list of the default robot addresses
    ///
    Q_INVOKABLE QStringList defaultRobotAddresses();

    ///
    /// Returns the current control mode of the robot
    ///
    Q_INVOKABLE DS::ControlMode controlMode();

    ///
    /// Returns a list with the current joysticks
    ///
    Q_INVOKABLE QList<DS::Joystick>* joysticks();

    ///
    /// Returns a pointer to the current protocol in use
    ///
    Q_INVOKABLE DS_Core::AbstractProtocol* currentProtocol();

    ///
    /// Returns \c true if the robot reports that the user code is loaded
    ///
    Q_INVOKABLE bool robotHasCode();

    ///
    /// Returns \c true if the robot is enabled
    ///
    Q_INVOKABLE bool isEnabled();

    ///
    /// Returns \c true if the client is using a practice match
    ///
    Q_INVOKABLE bool isPractice();

    ///
    /// Returns \c true if the robot is in test mode
    ///
    Q_INVOKABLE bool isTest();

    ///
    /// Returns \c true if the robot is in autonomous
    ///
    Q_INVOKABLE bool isAutonomous();

    ///
    /// Returns \c true if the robot is in teleoperated
    ///
    Q_INVOKABLE bool isTeleoperated();

    ///
    /// Returns \c true if the robot mode is \c kControlEmergencyStop
    ///
    Q_INVOKABLE bool isEmergencyStopped();

    ///
    /// Returns \c true if the DS is connected to the robot
    ///
    Q_INVOKABLE bool isConnected();

    ///
    /// Returns the number of joystick registered with the DS
    ///
    Q_INVOKABLE int joystickCount();

    ///
    /// Returns the current team number
    ///
    Q_INVOKABLE int team();

    ///
    /// Returns \c true if the current protocol allows us to send
    /// commands through the NetConsole
    ///
    Q_INVOKABLE bool acceptsConsoleCommands();

    ///
    /// Changes the status string when:
    ///     - The communications status has changed
    ///     - The robot code status has changed
    ///     - The control mode of the robot has changed
    ///
    Q_INVOKABLE QString getRobotStatus();

  public slots:
    ///
    /// Initializes the class and the interlal loop/refresh system
    ///
    Q_INVOKABLE void init();

    ///
    /// Reboots the robot using the specified protocol
    ///
    Q_INVOKABLE void rebootRobot();

    ///
    /// Restarts the robot code using the specified protocol
    ///
    Q_INVOKABLE void restartCode();

    ///
    /// Enables the robot directly
    ///
    Q_INVOKABLE void enableRobot();

    ///
    /// Disables the robot directly
    ///
    Q_INVOKABLE void disableRobot();

    ///
    /// Changes the \a enabled state of the robot
    ///
    Q_INVOKABLE void setEnabled (bool enabled = false);

    ///
    /// Sends a command through the net console
    ///
    Q_INVOKABLE void sendCommand (QString command = "");

    ///
    /// If \a emergency_stop is set to \c true, the robot will stop moving
    /// almost immediatly after this value is changed
    ///
    Q_INVOKABLE void setEmergencyStop (bool stop = false);

    ///
    /// Enables/Disables test mode directly
    ///
    Q_INVOKABLE void startTest (bool enabled = false);

    ///
    /// Enables/Disables autonomous directly
    ///
    Q_INVOKABLE void startAutonomous (bool enabled = false);

    ///
    /// Enables/Disables teleoperated directly
    ///
    Q_INVOKABLE void startTeleoperated (bool enabled = false);

    ///
    /// Simulates a timed match with the input time values (in seconds)
    ///
    Q_INVOKABLE void startPractice (int countdown = 0,
                                    int autonomous = 0,
                                    int delay = 0,
                                    int teleop = 0,
                                    int endgame = 0);

    ///
    /// Changes the team number used by the protocol and network diagnostics
    ///
    Q_INVOKABLE void setTeamNumber (int team = 0);

    ///
    /// Changes the alliance of the robot using the specified protocol
    ///
    Q_INVOKABLE void setAlliance (int alliance = DS::kAllianceRed1);

    ///
    /// Changes the protocol that we use to control the robot
    ///
    Q_INVOKABLE void setProtocol (DS_Core::AbstractProtocol* protocol);

    ///
    /// Changes the protocol that we use to control the robot
    ///
    Q_INVOKABLE void setProtocol (int protocol = DriverStation::kFRCProtocol2016);

    ///
    /// Changes the control mode of the robot, available options are:
    ///     - \c kTeleoperated
    ///     - \c kAutonomous
    ///     - \c kTest
    ///
    Q_INVOKABLE void setControlMode (int mode = DS::kControlTeleoperated);

    ///
    /// Changes the network address of the robot
    ///
    Q_INVOKABLE void setRobotAddress (QString address = "");

    ///
    /// Un-registers all the joysticks from the Driver Station
    ///
    Q_INVOKABLE void resetJoysticks();

    ///
    /// Updates the \a angle of the selected \a hat in the specified \a josytick
    ///
    Q_INVOKABLE void updateJoystickPOV (int js = -1, int hat = -1, int angle = 0);

    ///
    /// Registers a new joystick to the Driver Station with the specified number
    /// of \a axes, \a buttons and \a POVs.
    ///
    /// This function returns \c true if the joystick is valid and respects
    /// the axis and button ranges.
    ///
    Q_INVOKABLE bool addJoystick (int axes = -1, int buttons = -1, int POVs = -1);

    ///
    /// Updates the \a value of the selected \a axis in the specified \a josytick
    ///
    Q_INVOKABLE void updateJoystickAxis (int js = -1, int axis = -1, float value = 0);

    ///
    /// Updates the \a state of the selected \a button in the specified \a josytick
    ///
    Q_INVOKABLE void updateJoystickButton (int js = -1, int button = -1, bool pressed = false);

  protected:
    explicit DriverStation();

  signals:
    ///
    /// Emitted when \a Init() was called and the DS engine is started
    ///
    void initialized();

    ///
    /// Emitted when a joystick is added or removed from the DS system
    ///
    void joystickCountChanged();

    ///
    /// Emitted when the robot is e-stopped
    ///
    void emergencyStopped();

    ///
    /// Emitted when a protocol was set and initialized
    ///
    void protocolChanged();

    ///
    /// Emitted when the \a SetTeam() function is called
    ///
    void teamChanged (int team);

    ///
    /// Emitted when the robot is enabled or disabled
    ///
    void enabledChanged (bool enabled);

    ///
    /// Emitted when the client detects that the availability of the robot
    /// software/code has changed
    ///
    void codeChanged (bool available);

    ///
    /// Emitted when the robot detects a possible voltage brownout
    ///
    void voltageBrownoutChanged (bool hasBrownout);

    ///
    /// Emitted when the DS receives and decodes a CAN data structure
    ///
    void CANInfoReceived (DS::CAN_Information information);

    ///
    /// Emitted when the state of the FMS connection is changed
    ///
    void fmsChanged (bool attached);

    ///
    /// Emitted when the state of the network communications with the robot
    /// has been changed. Unlike the other signals with a \c bool value, this
    /// signal contains more information about the communication status, such
    /// as:
    ///     - The robot responds ping requests, but does not respond to DS
    ///     - The robot responds to ping requests and DS
    ///     - The robot does not respond to ping requests nor the DS
    ///
    void communicationsChanged (DS::CommStatus status);

    ///
    /// Emitted when the client detects that the availability of the robot radio
    /// has changed (eg. when the robot is powered off with the breaker)
    ///
    void radioChanged (bool available);

    ///
    /// Emitted when the client analyzes a packet from the robot and extracts
    /// the battery voltage of the robot.
    ///
    void voltageChanged (QString voltage);

    ///
    /// Emitted when the NetConsole receives and decodes a message from the
    /// robot
    ///
    void newMessage (QString message);

    ///
    /// Emitted when the client has just connected to the robot and downloaded
    /// its library INI files and analyzed them
    ///
    void libVersionChanged (QString version);

    ///
    /// Emitted when the client has just connected to the robot and downloaded
    /// its PCM INI files and analyzed them
    ///
    void rioVersionChanged (QString version);

    ///
    /// Emitted when the client has just connected to the robot and downloaded
    /// its PDP information and analyzed them
    ///
    void pdpVersionChanged (QString version);

    ///
    /// Emitted when the client has just connected to the robot and downloaded
    /// the PCM information files and analyzed them
    ///
    void pcmVersionChanged (QString version);

    ///
    /// Emitted when one of the following happens:
    ///     - The code status changes
    ///     - The communication status changes
    ///     - The control mode of the robot is changed
    ///
    void robotStatusChanged (QString status);

    ///
    /// Emitted when the control mode is changed
    ///
    void controlModeChanged (DS::ControlMode mode);

    ///
    /// Emitted when the libary detects that the RAM usage of the robot has
    /// changed since the last update.
    ///
    void ramUsageChanged (int percent);

    ///
    /// Emitted when the library detects that the CPU usage of the robot has
    /// changed
    ///
    void cpuUsageChanged (int percent);

    ///
    /// Emitted when the libary detects that the disk usage of the robot has
    /// changed since the last update.
    ///
    void diskUsageChanged (int percent);

    ///
    /// Emitted when the robot is enabled and the elapsed time
    /// has been updated by the library.
    ///
    void elapsedTimeChanged (QString time);

  private:
    ///
    /// This variable allows (or disallows) us to receive joystick
    /// data and perform most of the operations of the Driver Station.
    /// This variable is set to \c true when the \c init() function is called
    ///
    bool m_init;

    ///
    /// If set to \c true the client will asume that we are running a practice match
    ///
    bool m_practiceRunning;

    ///
    /// If set to true, the practice timings will be canceled
    ///
    bool m_practiceInterrupted;

    ///
    /// Used for receiving messages broadcasted by the
    /// robot over the network
    ///
    DS_Core::NetConsole* m_netConsole;

    ///
    /// Represents the current protocol in operation
    ///
    DS_Core::AbstractProtocol* m_protocol;

    ///
    /// Counts the elapsed time since the robot was
    /// enabled. When the robot is disabled, the elapsed
    /// time is stopped (just as in the official Driver Station)
    ///
    DS_Core::ElapsedTime* m_elapsedTime;

    ///
    /// Registers and manages the joystick inputs
    ///
    QList<DS::Joystick> m_joysticks;

  private slots:
    ///
    /// Resets the internal values of the library when we disconnect from the
    /// robot.
    ///
    void resetEverything();

    ///
    /// Updates the elapsed time when the control mode is changed
    ///
    void resetElapsedTimer (DS::ControlMode mode);

    ///
    /// Plays a sound that indicates the upcomming end of a match
    ///
    void playEndGameApproaching();

    ///
    /// Plays a sound that indicates the end of a match
    ///
    void playEndGame();

    ///
    /// Plays a sound that indicates the start of the operator control
    ///
    void playTeleopStart();

    ///
    /// Plays a sound that indicates the start of the autonomous period
    ///
    void playAutonomousStart();
};

#endif
