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
#include <QNetworkInterface>

#include "LibDS/Core/Common.h"
#include "LibDS/Core/Watchdog.h"
#include "LibDS/Core/SocketManager.h"

namespace DS_Core {

///
/// Implements an generic class to be used as a base for any communication protocol.
///
/// The \c AbstractProtocol class already implements all the functions needed to
/// communicate efficiently with the LibDS and implement the safety mechanisms
/// used by the Driver Station, such as detecting and updating emergency stop(s),
/// a dynamic watchdog (for establishing robot-to-DS communications and reseting
/// the protocol when the robot does not respond).
///
/// Additionally, the \c AbstractProtocol class implements the robot and radio lookup
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
class LIB_DS_DECL AbstractProtocol : public QObject {
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
    /// Returns \c true if the protocol is operating
    ///
    bool isOperating() const;

    ///
    /// Returns the expiration time of the watchdog
    ///
    int expirationTime() const;

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
    /// Returns \c true if the client is attached to the FMS
    ///
    bool isFmsAttached() const;

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
    /// Returns the default radio addresses of the current protocol and
    /// a generated list of DHCP IPs based on the local IP of the client.
    ///
    QStringList radioIPs();

    ///
    /// Returns the default robot addresses of the current protocol and
    /// a generated list of DHCP IPs based on the local IP of the client.
    ///
    QStringList robotIPs();

    ///
    /// Specifies the name/title of the protocol. This is used in the welcome
    /// message and to generate the protocol list in the DS.
    ///
    /// \note This function must be re-implemented by each protocol
    ///
    virtual QString name() {
        return "Abstract Protocol";
    }

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

  public slots:
    ///
    /// Inhibits the operation of the protocol
    ///
    void stop();

    ///
    /// Allows the operation of the protocol
    ///
    void start();

    ///
    /// Resets the internal values of the protocol and emits the appropiate
    /// signals when the robot communication is lost.
    ///
    /// After doing that, the function will try to ping the robot again
    ///
    void reset();

    ///
    /// Changes the team number and regenerates the IP lists
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
    virtual void onJoysticksChanged() {}

  protected slots:
    ///
    /// Implements the necessary steps to reset the internal values of a protocol
    ///
    /// \note This function must be re-implemented by each protocol
    ///
    virtual void resetProtocol()  {}

    ///
    /// Implements a method to get more information about the robot components
    ///
    /// \note This function must be re-implemented by each protocol
    ///
    virtual void getRobotInformation()  {}

  protected:
    ///
    /// Interprets the received FMS \a data
    ///
    /// \note This function must be re-implemented by each protocol
    ///
    virtual bool interpretFmsPacket (QByteArray data) {
        Q_UNUSED (data);
        return false;
    }

    ///
    /// Interprets the received robot \a data and emits the appropiate signals
    ///
    /// \note This function must be re-implemented by each protocol
    ///
    virtual bool interpretRobotPacket (QByteArray data) {
        Q_UNUSED (data);
        return false;
    }

    ///
    /// Generates a control packet to be sent to the FMS
    ///
    /// \note This function must be re-implemented by each protocol
    ///
    virtual QByteArray generateFmsPacket() {
        return QByteArray ("");
    }

    ///
    /// Generates a control packet to be sent to the robot
    ///
    /// \note This function must be re-implemented by each protocol
    ///
    virtual QByteArray generateRobotPacket() {
        return QByteArray ("");
    }

    ///
    /// Returns the default radio address
    ///
    /// \note This function must be re-implemented by each protocol
    ///
    virtual QStringList additionalRadioIPs() {
        return QStringList ("");
    }

    ///
    /// Returns the default robot address
    ///
    /// \note This function must be re-implemented by each protocol
    ///
    virtual QStringList additionalRobotIPs() {
        return QStringList ("");
    }

  public:
    explicit AbstractProtocol();

  protected:
    void updateRobotCode (bool available);
    void updateSendDateTime (bool sendDT);
    void updateFmsAttached (bool attached);
    void updateRadioStatus (bool connected);
    void updateVoltageBrownout (bool brownout);
    void updateVoltage (QString integer, QString decimal);
    void updateCommStatus (DS::DS_CommStatus statusCode);

  signals:
    void codeChanged (bool);
    void emergencyStopped();
    void cpuUsageChanged (int);
    void ramUsageChanged (int);
    void enabledChanged (bool);
    void diskUsageChanged (int);
    void radioCommChanged (bool);
    void voltageChanged (QString);
    void fmsChanged (bool attached);
    void libVersionChanged (QString);
    void rioVersionChanged (QString);
    void pdpVersionChanged (QString);
    void pcmVersionChanged (QString);
    void voltageBrownoutChanged (bool);
    void emergencyStoppedChanged (bool);
    void controlModeChanged (DS::ControlMode);
    void CANInfoReceived (DS::CAN_Information);
    void rumbleRequest (DS::RumbleRequest request);
    void communicationsChanged (DS::DS_CommStatus);

  private:
    int m_team;
    int m_interfaces;
    int m_sentFMSPackets;
    int m_sentRobotPackets;

    float m_voltage;

    bool m_enabled;
    bool m_operating;
    bool m_robotCode;
    bool m_fmsAttached;
    bool m_sendDateTime;
    bool m_patienceShown;
    bool m_emergencyStop;
    bool m_radioConnected;
    bool m_voltageBrownout;

    DS::Alliance m_alliance;
    DS::ControlMode m_controlMode;
    DS::DS_CommStatus m_communicationStatus;

    QStringList m_robotIPs;
    QStringList m_radioIPs;

    Watchdog m_watchdog;
    QTcpSocket m_robotPing;
    QTcpSocket m_radioPing;
    SocketManager m_sockets;

  private slots:
    void pingRobot();
    void pingRadio();
    void initialize();
    void sendFmsPacket();
    void sendRobotPacket();
    void generateIpLists();
    void disableEmergencyStop();
    void readFmsPacket (QByteArray data);
    void readRobotPacket (QByteArray data);
    void onPingResponse (QAbstractSocket::SocketState state);
};

}

#endif
