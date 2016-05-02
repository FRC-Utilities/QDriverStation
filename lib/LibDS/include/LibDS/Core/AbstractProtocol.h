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
#include "LibDS/DriverStation.h"
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
/// Please note that this class has a lot of inlined functions, which may increase code size,
/// but can make the operation of the library a little faster (e.g. by reducing function call
/// times while generating robot packets)
///
class LIB_DS_DECL AbstractProtocol : public QObject {
    Q_OBJECT

  public:
    ///
    /// Returns the current team number assigned to the protocol.
    /// Please note that the team number is assigned and updated automatically
    /// by the \c ProtocolManager class.
    ///
    inline int team() const {
        return m_team;
    }

    ///
    /// Returns \c true if the robot is running the user/team code.
    ///
    inline bool hasCode() const {
        return m_robotCode;
    }

    ///
    /// Returns \c true if the protocol is operating
    ///
    inline bool isOperating() const {
        return m_operating;
    }

    ///
    /// Returns the expiration time of the watchdog
    ///
    inline int expirationTime() const {
        return m_watchdog.expirationTime();
    }

    ///
    /// Returns the number of packets that the DS sent to the FMS.
    ///
    inline int sentFmsPackets() const {
        return m_sentFMSPackets;
    }

    ///
    /// Returns the number of packets that the DS sent to the robot.
    ///
    inline int sentRobotPackets() const {
        return m_sentRobotPackets;
    }

    ///
    /// Returns \c true if the robot is enabled.
    ///
    inline bool isEnabled() const {
        return m_enabled;
    }

    ///
    /// Returns \c true if the DS <--> Robot communications are working.
    ///
    inline bool isConnectedToRobot() const {
        return communicationStatus() == DS::kFull;
    }

    ///
    /// Returns \c true if the DS is able to ping the robot radio/router.
    ///
    inline bool isConnectedToRadio() const {
        return m_radioConnected;
    }

    ///
    /// Returns \c true if the robot requests the DS to send the current
    /// timestamp in a DS packet.
    ///
    inline bool sendDateTime() const {
        return m_sendDateTime;
    }

    ///
    /// Returns \c true if the robot reports a voltage brownout.
    ///
    inline bool hasVoltageBrownout() const {
        return m_voltageBrownout;
    }

    ///
    /// Returns \c true if the robot has been emergency stopped.
    /// Please note that the e-stop can be induced by the user or the
    /// robot controller itself.
    ///
    inline bool isEmergencyStopped() const {
        return m_emergencyStop;
    }

    ///
    /// Returns \c true if the client is attached to the FMS
    ///
    inline bool isFmsAttached() const {
        return m_fmsAttached;
    }

    ///
    /// Returns the current battery voltage of the robot.
    ///
    inline float robotVoltage() const {
        return m_voltage;
    }

    ///
    /// Returns the current alliance and position of the robot.
    ///
    inline DS::Alliance alliance() const {
        return m_alliance;
    }

    ///
    /// Returns the current control mode of the robot.
    ///
    inline DS::ControlMode controlMode() const {
        return m_controlMode;
    }

    ///
    /// Returns the communication status between the robot and the DS.
    ///
    inline DS::DS_CommStatus communicationStatus() const {
        return m_communicationStatus;
    }

    ///
    /// Returns a pointer to the joysticks managed by the \c DriverStation
    /// class.
    ///
    inline QList<DS::Joystick>* joysticks() const {
        return DriverStation::getInstance()->joysticks();
    }

    ///
    /// Returns the current radio address.
    /// Please note that this address can be one of the addresses
    /// specified by the subclassed protocol or an user-set address.
    ///
    inline QString radioAddress() {
        if (m_radioIPs.count() > m_radioIterator)
            return m_radioIPs.at (m_radioIterator);

        else
            return DS::getStaticIP (10, team(), 1);
    }

    ///
    /// Returns the current robot address.
    /// This address can be one of the addresses specified by the
    /// subclassed protocol or an user-set address.
    ///
    inline QString robotAddress() {
        return m_sockets.robotAddress();
    }

    ///
    /// Returns the default radio addresses of the current protocol and
    /// a generated list of DHCP IPs based on the local IP of the client.
    ///
    inline QStringList radioIPs() {
        return m_radioIPs;
    }

    ///
    /// Returns the default robot addresses of the current protocol and
    /// a generated list of DHCP IPs based on the local IP of the client.
    ///
    inline QStringList robotIPs() {
        return m_robotIPs;
    }

    ///
    /// Specifies the name/title of the protocol. This is used in the welcome
    /// message and to generate the protocol list in the DS.
    ///
    /// \note This function must be re-implemented by each protocol
    ///
    inline virtual QString name() {
        return "Abstract Protocol";
    }

    ///
    /// Specifies the frequency rate (in hertz) in which the subclassed
    /// protocol needs to generate a FMS packet.
    ///
    /// \note This function must be re-implemented by each protocol
    ///
    inline virtual int fmsFrequency() {
        return 0;
    }

    ///
    /// Specifies the frequency rate (in hertz) in which the subclassed
    /// protocol needs to generate a DS-to-robot packet.
    ///
    /// \note This function must be re-implemented by each protocol
    ///
    inline virtual int robotFrequency() {
        return 0;
    }

    ///
    /// Returns the port in which the protocol receives data from the FMS servers.
    ///
    /// \note This function must be re-implemented by each protocol
    ///
    inline virtual int fmsInputPort() {
        return DS::INVALID_PORT;
    }

    ///
    /// Returns the port in which the protocol sends data to the FMS servers.
    ///
    /// \note This function must be re-implemented by each protocol
    ///
    inline virtual int fmsOutputPort() {
        return DS::INVALID_PORT;
    }

    ///
    /// Returns the port in which the protocol receives data from the robot.
    ///
    /// \note This function must be re-implemented by each protocol
    ///
    inline virtual int robotInputPort() {
        return DS::INVALID_PORT;
    }

    ///
    /// Returns the port in which the protocol sends data to the robot.
    ///
    /// \note This function must be re-implemented by each protocol
    ///
    inline virtual int robotOutputPort() {
        return DS::INVALID_PORT;
    }

    ///
    /// Returns a TCP port that the robot uses for other functions (e.g. the
    /// webdashboard on the roboRIO).
    /// This port is used to check we we can ping the robot succesfully and
    /// obtain the 'partial' communication status.
    ///
    inline virtual int tcpProbesPort() {
        return DS::INVALID_PORT;
    }

    ///
    /// Returns the port in which we receive broadcasted messages from the robot.
    ///
    /// \note This function must be re-implemented by each protocol
    ///
    inline virtual int netConsoleInputPort() {
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
    inline virtual int netConsoleOutputPort() {
        return DS::INVALID_PORT;
    }

    ///
    /// Returns \c true if the protocol/robot accepts sending broadcasted
    /// commands from the DS to the robot.
    ///
    /// \note This function must be re-implemented by each protocol
    ///
    inline virtual bool acceptsConsoleCommands() {
        return false;
    }

    ///
    /// Returns the default radio address
    ///
    /// \note This function must be re-implemented by each protocol
    ///
    inline virtual QStringList additionalRadioIPs() {
        return QStringList (DS::getStaticIP (10, team(), 1));
    }

    ///
    /// Returns the default robot address
    ///
    /// \note This function must be re-implemented by each protocol
    ///
    inline virtual QStringList additionalRobotIPs() {
        return QStringList (DS::getStaticIP (10, team(), 2));
    }

    ///
    /// Specifies the type of IP protocol/socket that we must use to communicate with the FMS.
    /// Possible values are \a DS::kTcpSocket and \a DS::kUdpSocket
    ///
    /// \note This function must be re-implemented by each protocol
    ///
    inline virtual DS::SocketType fmsSocketType() {
        return DS::kUdpSocket;
    }

    ///
    /// Specifies the type of IP protocol/socket that we must use to communicate with the robot.
    /// Possible values are \a DS::kTcpSocket and \a DS::kUdpSocket
    ///
    /// \note This function must be re-implemented by each protocol
    ///
    inline virtual DS::SocketType robotSocketType() {
        return DS::kUdpSocket;
    }

  public slots:
    ///
    /// Inhibits the operation of the protocol
    ///
    inline void stop() {
        m_operating = false;
    }

    ///
    /// Allows the operation of the protocol
    ///
    inline void start() {
        m_operating = true;
    }

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

  public:
    explicit AbstractProtocol();

  protected:
    inline void updateRobotCode (bool available) {
        if (m_robotCode == available)
            return;

        if (m_robotCode && !available)
            setEnabled (false);

        m_robotCode = available;
        emit codeChanged (m_robotCode);
    }

    inline void updateSendDateTime (bool sendDT) {
        m_sendDateTime = sendDT;
    }

    inline void updateFmsAttached (bool attached) {
        m_fmsAttached = attached;
        emit fmsChanged (m_fmsAttached);
    }

    inline void updateRadioStatus (bool connected) {
        m_radioConnected = connected;
        emit radioCommChanged (m_radioConnected);
    }

    inline void updateVBrownout (bool brownout) {
        m_voltageBrownout = brownout;
        emit voltageBrownoutChanged (m_voltageBrownout);
    }

    inline void updateVoltage (float voltage) {
        m_voltage = voltage;
        int integer = (int) voltage;
        int decimal = (float) (voltage - integer) * 100;

        QString intString = QString::number (integer);
        QString decString = QString::number (decimal);
        intString.prepend (integer < 10 ? "0" : "");
        decString.prepend (decimal < 10 ? "0" : "");

        emit voltageChanged (intString + "." + decString);
    }

    inline void updateCommStatus (DS::DS_CommStatus status) {
        m_communicationStatus = status;
        emit communicationsChanged (m_communicationStatus);
    }

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
    int m_radioIterator;
    int m_sentFMSPackets;
    int m_sentRobotPackets;

    int m_fmsFrequency;
    int m_robotFrequency;

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
