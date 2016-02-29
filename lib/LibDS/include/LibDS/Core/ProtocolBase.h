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

#ifndef _LDS_PROTOCOL_H
#define _LDS_PROTOCOL_H

#include <QBitArray>
#include <QHostInfo>
#include <QTcpSocket>
#include <QNetworkReply>

#include "LibDS/Core/Common.h"
#include "LibDS/Core/Watchdog.h"

namespace DS_Core {

///
/// Implements an generic class to be used as a base for any communication protocol.
///
/// The \c ProtocolBase class already implements all the functions needed to
/// communicate efficiently with the LibDS and implement the safety mechanisms
/// used by the Driver Station, such as detecting and updating emergency stop(s),
/// a dynamic watchdog (for establishing robot-to-DS communications and reseting
/// the protocol when the robot does not respond).
///
/// Additionally, the \c ProtocolBase class implements the robot and radio lookup
/// functions in such a way that a subclassed protocol only needs to define a list
/// with possible robot IPs for the DS to find the robot.
///
/// Finally, joystick input is handled by automatically by this class and the
/// \c ProtocolManager class.
///
/// If you wish to implement a protocol, you just need to redefine the necessary
/// virtual functions to generate DS-to-robot packets, read robot-to-DS packets,
/// encode joystick outputs and specify the ports used by the DS, robot and NetConsole.
///
class LIB_DS_DECL ProtocolBase : public QObject {
    Q_OBJECT

  public:
    ///
    /// Returns the current team number assigned to the protocol.
    /// Please note that the team number is assigned and updated automatically
    /// by the \c ProtocolManager class.
    ///
    int team() const;

    ///
    /// Returns \c true if the robot is running the user/team code.
    ///
    bool hasCode() const;

    ///
    /// Returns the number of packets that the DS sent to the FMS.
    ///
    int sentFmsPackets() const;

    ///
    /// Returns the number of packets that the DS sent to the robot.
    ///
    int sentRobotPackets() const;

    ///
    /// Returns \c true if the robot is enabled.
    ///
    bool isEnabled() const;

    ///
    /// Returns \c true if the DS <--> Robot communications are working.
    ///
    bool isConnectedToRobot() const;

    ///
    /// Returns \c true if the DS is able to ping the robot radio/router.
    ///
    bool isConnectedToRadio() const;

    ///
    /// Returns \c true if the robot requests the DS to send the current
    /// timestamp in a DS packet.
    ///
    bool sendDateTime() const;

    ///
    /// Returns \c true if the robot reports a voltage brownout.
    ///
    bool hasVoltageBrownout() const;

    ///
    /// Returns \c true if the robot has been emergency stopped.
    /// Please note that the e-stop can be induced by the user or the
    /// robot controller itself.
    ///
    bool isEmergencyStopped() const;

    ///
    /// Returns the current battery voltage of the robot.
    ///
    float batteryVoltage() const;

    ///
    /// Returns the current alliance and position of the robot.
    ///
    DS::Alliance alliance() const;

    ///
    /// Returns the current control mode of the robot.
    ///
    DS::ControlMode controlMode() const;

    ///
    /// Returns the communication status between the robot and the DS.
    ///
    DS::DS_CommStatus communicationStatus() const;

    ///
    /// Returns a pointer to the joysticks managed by the \c DriverStation
    /// class.
    ///
    QList<DS::Joystick>* joysticks() const;

    ///
    /// Returns the current radio address.
    /// Please note that this address can be one of the addresses
    /// specified by the subclassed protocol or an user-set address.
    ///
    QString radioAddress();

    ///
    /// Returns the current robot address.
    /// This address can be one of the addresses specified by the
    /// subclassed protocol or an user-set address.
    ///
    QString robotAddress();

    ///
    /// Constructs an FMS packet and updates the internal values
    /// of this class.
    ///
    /// Please note that each subclassed protocol defines how a
    /// FMS packet is generated with the \c _getFmsPacket function.
    ///
    QByteArray createFmsPacket();

    ///
    /// Constructs a DS-to-robot packet and updates the internal values
    /// of this class.
    ///
    /// Please note that each subclassed protocol defines how a robot
    /// packet is generated with the \c _getRobotPacket function.
    ///
    QByteArray createRobotPacket();

    ///
    /// Specifies the frequency rate (in hertz) in which the subclassed
    /// protocol needs to generate a FMS packet.
    ///
    /// \note This function must be re-implemented by each protocol
    ///
    virtual int fmsFrequency() {
        return 0;
    }

    ///
    /// Specifies the frequency rate (in hertz) in which the subclassed
    /// protocol needs to generate a DS-to-robot packet.
    ///
    /// \note This function must be re-implemented by each protocol
    ///
    virtual int robotFrequency() {
        return 0;
    }

    ///
    /// Returns the port in which the protocol receives data from the FMS servers.
    ///
    /// \note This function must be re-implemented by each protocol
    ///
    virtual int fmsInputPort() {
        return DS::INVALID_PORT;
    }

    ///
    /// Returns the port in which the protocol sends data to the FMS servers.
    ///
    /// \note This function must be re-implemented by each protocol
    ///
    virtual int fmsOutputPort() {
        return DS::INVALID_PORT;
    }

    ///
    /// Returns the port in which the protocol receives data from the robot.
    ///
    /// \note This function must be re-implemented by each protocol
    ///
    virtual int robotInputPort() {
        return DS::INVALID_PORT;
    }

    ///
    /// Returns the port in which the protocol sends data to the robot.
    ///
    /// \note This function must be re-implemented by each protocol
    ///
    virtual int robotOutputPort() {
        return DS::INVALID_PORT;
    }

    ///
    /// Returns a TCP port that the robot uses for other functions (e.g. the
    /// webdashboard on the roboRIO).
    /// This port is used to check we we can ping the robot succesfully and
    /// obtain the 'partial' communication status.
    ///
    virtual int tcpProbesPort() {
        return DS::INVALID_PORT;
    }

    ///
    /// Returns the port in which we receive broadcasted messages from the robot.
    ///
    /// \note This function must be re-implemented by each protocol
    ///
    virtual int netConsoleInputPort() {
        return DS::INVALID_PORT;
    }

    ///
    /// Returns the port in which we send broadcasted commands to the robot.
    /// Please note that not all protocols support this feature, for example,
    /// the 2015 protocol does not support this, while the 2014 protocol does
    /// accept sending broadcasted commands to the robot.
    ///
    /// \note This function must be re-implemented by each protocol
    ///
    virtual int netConsoleOutputPort() {
        return DS::INVALID_PORT;
    }

    ///
    /// Returns \c true if the protocol/robot accepts sending broadcasted
    /// commands from the DS to the robot.
    ///
    /// \note This function must be re-implemented by each protocol
    ///
    virtual bool acceptsConsoleCommands() {
        return false;
    }

    ///
    /// Returns the default radio address
    ///
    /// \note This function must be re-implemented by each protocol
    ///
    virtual QStringList defaultRadioAddress() {
        return QStringList ("");
    }

    ///
    /// Returns the default robot address
    ///
    /// \note This function must be re-implemented by each protocol
    ///
    virtual QStringList defaultRobotAddress() {
        return QStringList ("");
    }

  public slots:
    ///
    /// Resets the internal values of the protocol and emits the appropiate
    /// signals when the robot communication is lost.
    ///
    /// After doing that, the function will try to ping the robot again
    ///
    void reset();

    ///
    /// Changes the team number of the robot, this can be used to generate
    /// the robot and radio address.
    ///
    void setTeam (int team);

    ///
    /// Attempts to change the enabled state of the robot
    ///
    void setEnabled (bool enabled);

    ///
    /// Attempts to change the e-stop state of the robot
    ///
    void setEmergencyStop (bool emergency_stop);

    ///
    /// Changes the robot address to \a address
    ///
    void setRobotAddress (QString address);

    ///
    /// Updates the \a alliance of the robot
    ///
    void setAlliance (DS::Alliance alliance);

    ///
    /// Updates the control \a mode of the robot
    ///
    void setControlMode (DS::ControlMode mode);

    ///
    /// Reads the FMS packet and calls the appropiate functions to interpret it
    ///
    void readFmsPacket (QByteArray data);

    ///
    /// Reads the robot data and calls the appropiate functions to interpret it
    ///
    void readRobotPacket (QByteArray data);

    ///
    /// Reboots the robot
    ///
    /// \note This function must be re-implemented by each protocol
    ///
    virtual void reboot() {}

    ///
    /// Restarts the robot code or the user program
    /// \note This function must be re-implemented by each protocol
    ///
    virtual void restartCode() {}

    ///
    /// Called when the state of the joysticks is changed
    ///
    virtual void _onJoysticksChanged() {}

  protected slots:
    ///
    /// Implements the necessary steps to reset the internal values of a protocol
    ///
    /// \note This function must be re-implemented by each protocol
    ///
    virtual void _resetProtocol()  {}

    ///
    /// Implements a method to get more information about the robot components
    ///
    /// \note This function must be re-implemented by each protocol
    ///
    virtual void _getRobotInformation()  {}

  protected:
    ///
    /// Interprets the received FMS \a data
    ///
    /// \note This function must be re-implemented by each protocol
    ///
    virtual bool _readFMSPacket (QByteArray data) {
        Q_UNUSED (data);
        return false;
    }

    ///
    /// Interprets the received robot \a data and emits the appropiate signals
    ///
    /// \note This function must be re-implemented by each protocol
    ///
    virtual bool _readRobotPacket (QByteArray data) {
        Q_UNUSED (data);
        return false;
    }

    ///
    /// Uses the joystick input information to generate a data array to be
    /// sent along the client packet
    ///
    /// \note This function must be re-implemented by each protocol
    ///
    virtual QByteArray _getJoystickData() {
        return QByteArray ("");
    }

    ///
    /// Generates the neccesary data to send the robot the current timezone
    /// of the client DS
    ///
    /// \note This function must be re-implemented by each protocol
    ///
    virtual QByteArray _getTimezoneData() {
        return QByteArray ("");
    }

    ///
    /// Generates a control packet to be sent to the FMS
    ///
    /// \note This function must be re-implemented by each protocol
    ///
    virtual QByteArray _getFmsPacket() {
        return QByteArray ("");
    }

    ///
    /// Generates a control packet to be sent to the robot
    ///
    /// \note This function must be re-implemented by each protocol
    ///
    virtual QByteArray _getClientPacket() {
        return QByteArray ("");
    }

  public:
    explicit ProtocolBase();

  protected:
    void updateRobotCode (bool available);
    void updateSendDateTime (bool sendDT);
    void updateRadioStatus (bool connected);
    void updateCommStatus (DS::DS_CommStatus statusCode);
    void updateVoltageBrownout (bool brownout);
    void updateVoltage (int major, int minor);

  signals:
    void codeChanged (bool);
    void emergencyStopped();
    void fmsChanged (bool attached);
    void radioCommChanged (bool);
    void rumbleRequest (DS::RumbleRequest request);
    void communicationsChanged (DS::DS_CommStatus);
    void voltageBrownoutChanged (bool);
    void voltageChanged (QString);
    void cpuUsageChanged (int);
    void ramUsageChanged (int);
    void diskUsageChanged (int);
    void CANInfoReceived (DS::CAN);
    void enabledChanged (bool);
    void emergencyStoppedChanged (bool);
    void controlModeChanged (DS::ControlMode);
    void libVersionChanged (QString);
    void rioVersionChanged (QString);
    void pdpVersionChanged (QString);
    void pcmVersionChanged (QString);
    void robotAddressChanged (QString);
    void packetReceived();

  private:
    int m_team;
    int m_radioIterator;
    int m_robotIterator;
    int m_sentFMSPackets;
    int m_sentRobotPackets;

    float m_voltage;

    bool m_enabled;
    bool m_robotCode;
    bool m_sendDateTime;
    bool m_emergencyStop;
    bool m_radioConnected;
    bool m_voltageBrownout;

    DS::Alliance m_alliance;
    DS::ControlMode m_controlMode;
    DS::DS_CommStatus m_communicationStatus;

    QString m_robotIp;
    QString m_robotAddress;
    QString m_radioAddress;

    Watchdog m_watchdog;
    QTcpSocket m_robotPing;
    QTcpSocket m_radioPing;

  private slots:
    void pingRobot();
    void pingRadio();
    void showPatienceMsg();
    void disableEmergencyStop();
    void lookupFinished (QHostInfo info);
    void onPingResponse (QAbstractSocket::SocketState state);
};

}

#endif
