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

DS_ProtocolBase::DS_ProtocolBase() {
    m_team = 0;
    m_sentPackets = 0;
    m_robotCode = false;
    m_communicationStatus = kFailing;
    m_alliance = kAllianceRed1;
    m_robotAddress = QString ("");
    m_controlMode = kControlNoCommunication;

    m_joysticks = new QList<DS_Joystick*>;

    connect (&m_watchdog,   SIGNAL (timeout()),        this,        SLOT (reset()));
    connect (this,          SIGNAL (packetReceived()), &m_watchdog,
             SLOT (restart()));
    connect (&m_pingSocket, SIGNAL (stateChanged   (QAbstractSocket::SocketState)),
             this,          SLOT   (onStateChanged (QAbstractSocket::SocketState)));
}

DS_ProtocolBase::~DS_ProtocolBase() {
    m_pingSocket.abort();
    delete m_joysticks;
}

int DS_ProtocolBase::team() const {
    return m_team;
}

int DS_ProtocolBase::status() const {
    return m_status;
}

bool DS_ProtocolBase::robotCode() const {
    return m_robotCode;
}

int DS_ProtocolBase::sentPackets() const {
    return m_sentPackets;
}

bool DS_ProtocolBase::isConnected() const {
    return communicationStatus() == kFull;
}

bool DS_ProtocolBase::sendDateTime() const {
    return m_sendDateTime;
}

DS_Alliance DS_ProtocolBase::alliance() const {
    return m_alliance;
}

DS_ControlMode DS_ProtocolBase::controlMode() const {
    return m_controlMode;
}

DS_CommunicationStatus DS_ProtocolBase::communicationStatus() const {
    return m_communicationStatus;
}

QList<DS_Joystick*>* DS_ProtocolBase::joysticks() const {
    return m_joysticks;
}

QString DS_ProtocolBase::radioAddress() {
    return m_radioAddress.isEmpty() ? defaultRadioAddress() : m_radioAddress;
}

QString DS_ProtocolBase::robotAddress() {
    return m_robotAddress.isEmpty() ? defaultRobotAddress() : m_robotAddress;
}

QByteArray DS_ProtocolBase::getClientPacket() {
    m_sentPackets += 1;
    return generateClientPacket();
}

void DS_ProtocolBase::reset() {
    updateRobotCode (false);
    updateSendDateTime (false);
    updateCommunications (kFailing);

    emit voltageChanged (QString (""));

    m_discovery.getIp (robotAddress(), this, SLOT (onIpFound (QString, QString)));

    resetProtocol();
}

void DS_ProtocolBase::setTeamNumber (int team) {
    m_team = team;
    emit robotAddressChanged (robotAddress());
}

void DS_ProtocolBase::setRobotAddress (QString address) {
    m_robotAddress = address;
    emit robotAddressChanged (robotAddress());
}

void DS_ProtocolBase::setAlliance (DS_Alliance alliance) {
    m_alliance = alliance;
}

void DS_ProtocolBase::setControlMode (DS_ControlMode mode) {
    if (m_controlMode != kControlEmergencyStop) {
        m_controlMode = isConnected() ? mode : kControlNoCommunication;
        emit controlModeChanged (controlMode());
    }
}

void DS_ProtocolBase::setJoysticks (QList<DS_Joystick*>* joysticks) {
    m_joysticks = joysticks;
}

void DS_ProtocolBase::readRobotPacket (QByteArray data) {
    if (!data.isEmpty())
        readRobotData (data);
}

QByteArray DS_ProtocolBase::bitsToBytes (QBitArray bits) {
    QByteArray bytes (bits.count() / 8, 0);

    for (int i = 0; i < bits.count(); ++i)
        bytes [i / 8] = (bytes.at (i / 8) | ((bits [i] ? 1 : 0) << (i % 8)));

    return bytes;
}

void DS_ProtocolBase::updateStatus (int status) {
    m_status = status;
}

void DS_ProtocolBase::updateRobotCode (bool available) {
    m_robotCode = available;
    emit codeChanged (m_robotCode);
}

void DS_ProtocolBase::updateSendDateTime (bool sendDT) {
    m_sendDateTime = sendDT;
}

void DS_ProtocolBase::updateCommunications (DS_CommunicationStatus status) {
    m_communicationStatus = status;
    emit communicationsChanged (m_communicationStatus);
}

void DS_ProtocolBase::updateVoltage (char major, char minor) {
    QString maj = QString::number (major);
    QString min = QString::number (minor);

    maj = maj.replace ("-", "");
    min = min.replace ("-", "");

    if (min.length() == 1)
        min = QString ("0%1").arg (min);

    else if (min.length() > 2)
        min = QString (min.at (0)) + QString (min.at (1));

    emit voltageChanged (QString ("%1.%2").arg (maj, min));
}

void DS_ProtocolBase::pingRobot() {
    m_pingSocket.abort();
    m_pingSocket.connectToHost (robotAddress(), 80, QTcpSocket::ReadOnly);
}

void DS_ProtocolBase::onIpFound (QString address, QString ip) {
    if (address.toLower() == robotAddress().toLower()) {
        setRobotAddress (ip);
        pingRobot();
    }
}

void DS_ProtocolBase::onStateChanged (QAbstractSocket::SocketState state) {
    if (communicationStatus() == kFailing
            && state == QAbstractSocket::ConnectedState)
        updateCommunications (kPartial);
}
