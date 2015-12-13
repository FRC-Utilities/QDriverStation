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

#include "LibDS/Core/ProtocolBase.h"

//=============================================================================
// DS_ProtocolBase::DS_ProtocolBase
//=============================================================================

DS_ProtocolBase::DS_ProtocolBase() {
    m_team = 0;
    m_resetCount = 0;
    m_sentPackets = 0;
    m_robotCode = false;
    m_useFallbackAddress = false;
    m_communicationStatus = kFailing;
    m_alliance = kAllianceRed1;
    m_robotAddress = QString ("");
    m_controlMode = kControlDisabled;

    m_joysticks = new QList<DS_Joystick*>;

    connect (&m_watchdog, SIGNAL (Timeout()), this, SLOT (Reset()));
    connect (this, SIGNAL (PacketReceived()), &m_watchdog, SLOT (Restart()));
    connect (&m_pingSocket, SIGNAL (stateChanged   (QAbstractSocket::SocketState)),
             this,          SLOT   (OnStateChanged (QAbstractSocket::SocketState)));
}

//=============================================================================
// DS_ProtocolBase::~DS_ProtocolBase
//=============================================================================

DS_ProtocolBase::~DS_ProtocolBase() {
    m_pingSocket.abort();
    delete m_joysticks;
}

//=============================================================================
// DS_ProtocolBase::Team
//=============================================================================

int DS_ProtocolBase::Team() const {
    return m_team;
}

//=============================================================================
// DS_ProtocolBase::Status
//=============================================================================

int DS_ProtocolBase::Status() const {
    return m_status;
}

//=============================================================================
// DS_ProtocolBase::RobotCode
//=============================================================================

bool DS_ProtocolBase::RobotCode() const {
    return m_robotCode;
}

//=============================================================================
// DS_ProtocolBase::SentPackets
//=============================================================================

int DS_ProtocolBase::SentPackets() const {
    return m_sentPackets;
}

//=============================================================================
// DS_ProtocolBase::IsConnected
//=============================================================================

bool DS_ProtocolBase::IsConnected() const {
    return CommunicationStatus() == kFull;
}

//=============================================================================
// DS_ProtocolBase::SendDateTime
//=============================================================================

bool DS_ProtocolBase::SendDateTime() const {
    return m_sendDateTime;
}

//=============================================================================
// DS_ProtocolBase::Alliance
//=============================================================================

DS_Alliance DS_ProtocolBase::Alliance() const {
    return m_alliance;
}

//=============================================================================
// DS_ProtocolBase::ControlMode
//=============================================================================

DS_ControlMode DS_ProtocolBase::ControlMode() const {
    return m_controlMode;
}

//=============================================================================
// DS_ProtocolBase::CommunicationStatus
//=============================================================================

DS_CommStatus DS_ProtocolBase::CommunicationStatus() const {
    return m_communicationStatus;
}

//=============================================================================
// DS_ProtocolBase::Joysticks
//=============================================================================

QList<DS_Joystick*>* DS_ProtocolBase::Joysticks() const {
    return m_joysticks;
}

//=============================================================================
// DS_ProtocolBase::RadioAddress
//=============================================================================

QString DS_ProtocolBase::RadioAddress() {
    return m_radioAddress.isEmpty() ? DefaultRadioAddress() : m_radioAddress;
}

//=============================================================================
// DS_ProtocolBase::RobotAddress
//=============================================================================

QString DS_ProtocolBase::RobotAddress() {
    if (m_robotAddress.isEmpty()) {
        if (m_useFallbackAddress)
            return DS_GetStaticIp (Team(), 2);
        else
            return DefaultRobotAddress();
    }

    return m_robotAddress;
}

//=============================================================================
// DS_ProtocolBase::CreateClientPacket
//=============================================================================

QByteArray DS_ProtocolBase::CreateClientPacket() {
    m_sentPackets += 1;
    return GetClientPacket();
}

//=============================================================================
// DS_ProtocolBase::Reset
//=============================================================================

void DS_ProtocolBase::Reset() {
    /* Custom reset procedures for each protocol */
    ResetProtocol();

    /* Emit appropiate signals */
    UpdateVoltage (0, 0);
    UpdateRobotCode (false);
    UpdateSendDateTime (false);
    UpdateCommStatus (kFailing);

    /* Toggle the usage of the fallback robot address every 10 resets */
    m_resetCount += 1;
    if (m_resetCount >= 10) {
        m_resetCount = 0;
        m_useFallbackAddress = !m_useFallbackAddress;
    }

    /* Figure out the robot address and ping the robot */
    emit RobotAddressChanged (RobotAddress());
    m_discovery.GetIP (RobotAddress(), this, SLOT (OnIpFound (QString, QString)));
}

//=============================================================================
// DS_ProtocolBase::SetTeamNumber
//=============================================================================

void DS_ProtocolBase::SetTeamNumber (int team) {
    m_team = team;
    emit RobotAddressChanged (RobotAddress());
}

//=============================================================================
// DS_ProtocolBase::SetRobotAddress
//=============================================================================

void DS_ProtocolBase::SetRobotAddress (QString address) {
    m_robotAddress = address;
    emit RobotAddressChanged (RobotAddress());
}

//=============================================================================
// DS_ProtocolBase::SetAlliance
//=============================================================================

void DS_ProtocolBase::SetAlliance (DS_Alliance alliance) {
    m_alliance = alliance;
}

//=============================================================================
// DS_ProtocolBase::SetControlMode
//=============================================================================

void DS_ProtocolBase::SetControlMode (DS_ControlMode mode) {
    if (m_controlMode != kControlEmergencyStop) {
        m_controlMode = IsConnected() ? mode : kControlDisabled;
        emit ControlModeChanged (ControlMode());
    }
}

//=============================================================================
// DS_ProtocolBase::SetJoysticks
//=============================================================================

void DS_ProtocolBase::SetJoysticks (QList<DS_Joystick*>* joysticks) {
    m_joysticks = joysticks;
}

//=============================================================================
// DS_ProtocolBase::ReadRobotPacket
//=============================================================================

void DS_ProtocolBase::ReadRobotPacket (QByteArray data) {
    if (!data.isEmpty()) {
        /* We just have connected to the robot, update internal values */
        if (!IsConnected()) {
            GetRobotInformation();
            UpdateCommStatus (kFull);
            SetControlMode (kControlDisabled);
        }

        /* Let the protocol implementation read the rest of the data */
        if (ReadRobotPacket (data))
            emit PacketReceived();
    }
}

//=============================================================================
// DS_ProtocolBase::UpdateStatus
//=============================================================================

void DS_ProtocolBase::UpdateStatus (int status) {
    m_status = status;
}

//=============================================================================
// DS_ProtocolBase::UpdateRobotCode
//=============================================================================

void DS_ProtocolBase::UpdateRobotCode (bool available) {
    /* Robot code just crashed/failed */
    if (m_robotCode && !available)
        SetControlMode (kControlDisabled);

    /* Update DS information */
    m_robotCode = available;
    emit CodeChanged (m_robotCode);
}

//=============================================================================
// DS_ProtocolBase::UpdateSendDateTime
//=============================================================================

void DS_ProtocolBase::UpdateSendDateTime (bool sendDT) {
    m_sendDateTime = sendDT;
}

//=============================================================================
// DS_ProtocolBase::UpdateCommStatus
//=============================================================================

void DS_ProtocolBase::UpdateCommStatus (DS_CommStatus status) {
    m_communicationStatus = status;
    emit CommunicationsChanged (m_communicationStatus);
}

//=============================================================================
// DS_ProtocolBase::UpdateVoltage
//=============================================================================

void DS_ProtocolBase::UpdateVoltage (int major, int minor) {
    QString maj = QString::number (major);
    QString min = QString::number (minor);

    maj = maj.replace ("-", "");
    min = min.replace ("-", "");

    if (min.length() == 1)
        min = QString ("0%1").arg (min);

    else if (min.length() > 2)
        min = QString (min.at (0)) + QString (min.at (1));

    emit VoltageChanged (QString ("%1.%2").arg (maj, min));
}

//=============================================================================
// DS_ProtocolBase::PingRobot
//=============================================================================

void DS_ProtocolBase::PingRobot() {
    m_pingSocket.abort();
    m_pingSocket.connectToHost (RobotAddress(), 80, QTcpSocket::ReadOnly);
}

//=============================================================================
// DS_ProtocolBase::OnIpFound
//=============================================================================

void DS_ProtocolBase::OnIpFound (QString address, QString ip) {
    if (address.toLower() == RobotAddress().toLower() && address != ip)
        emit RobotAddressChanged (ip);

    PingRobot();
}

//=============================================================================
// DS_ProtocolBase::OnStateChanged
//=============================================================================

void DS_ProtocolBase::OnStateChanged (QAbstractSocket::SocketState state) {
    if (CommunicationStatus() == kFailing
            && state == QAbstractSocket::ConnectedState)
        UpdateCommStatus (kPartial);
}
