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

#include "LibDS/DriverStation.h"
#include "LibDS/Core/ProtocolBase.h"

using namespace DS_Core;

//=============================================================================
// NetConsole warnings/information texts
//=============================================================================

const QString CHECK_FIREWALL = "<p>"
                               "<font color=#FE755C><b>ERROR:</b></font> "
                               "<font color=#FFFFFF>Cannot connect to robot</font><br/>"
                               "<font color=#20C9FF>The robot is slow to respond. "
                               "Check firewall settings.</font>"
                               "</p>";
const QString PARTIAL_COMM   = "<p>"
                               "<font color=#FFF959><b>WARNING:</b></font> "
                               "<font color=#FFFFFF>%1 responds to ping requests, "
                               "but does not respond to DS packets.</font>"
                               "</p>";
const QString INFO_NOTE      = "<p>"
                               "<font color=#20C9FF><b>NOTE:</b></font> "
                               "<font color=#FFFFFF>"
                               "The DS may take up to 20 seconds to detect "
                               "your robot, please be patient.</font>"
                               "</p>";
const QString CODE_CRASH     = "<p>"
                               "<font color=#FFF959><b>WARNING:</b></font> "
                               "<font color=#FFFFFF>"
                               "The robot code seems to have crashed.</font>"
                               "</p>";
const QString COMM_ESTABLISH = "<p>"
                               "<font color=#59FF59><b>INFO:</b></font> "
                               "<font color=#FFFFFF>"
                               "Robot communications established</font></p>";

//=============================================================================
//ProtocolBase:DS_ProtocolBase
//=============================================================================

ProtocolBase::ProtocolBase() {
    m_team                = 0;
    m_radioIterator       = 0;
    m_robotIterator       = 0;
    m_sentFMSPackets      = 0;
    m_sentRobotPackets    = 0;

    m_robotAddress        = "";

    m_alliance            = DS::kAllianceRed1;
    m_controlMode         = DS::kControlInvalid;
    m_communicationStatus = DS::kFailing;

    m_enabled             = false;
    m_robotCode           = false;
    m_emergencyStop       = false;
    m_radioConnected      = false;

    m_radioPing.setObjectName ("Radio Ping");
    m_robotPing.setObjectName ("Robot Ping");

    connect (&m_watchdog,  &Watchdog::timeout,
             this,         &ProtocolBase::reset);
    connect (this,         &ProtocolBase::packetReceived,
             &m_watchdog,  &Watchdog::restart);
    connect (&m_robotPing, &QTcpSocket::stateChanged,
             this,         &ProtocolBase::onPingResponse);
    connect (&m_radioPing, &QTcpSocket::stateChanged,
             this,         &ProtocolBase::onPingResponse);

    reset();
    QTimer::singleShot (500, Qt::CoarseTimer, this, SLOT (showPatienceMsg()));
}

//=============================================================================
//ProtocolBase::team
//=============================================================================

int ProtocolBase::team() const {
    return m_team;
}

//=============================================================================
//ProtocolBase::hasCode
//=============================================================================

bool ProtocolBase::hasCode() const {
    return m_robotCode;
}

//=============================================================================
//ProtocolBase::sentFMSPackets
//=============================================================================

int ProtocolBase::sentFmsPackets() const {
    return m_sentFMSPackets;
}

//=============================================================================
//ProtocolBase::sentRobotPackets
//=============================================================================

int ProtocolBase::sentRobotPackets() const {
    return m_sentRobotPackets;
}

//=============================================================================
//ProtocolBase::isEnabled
//=============================================================================

bool ProtocolBase::isEnabled() const {
    return m_enabled;
}

//=============================================================================
//ProtocolBase::isConnectedToRobot
//=============================================================================

bool ProtocolBase::isConnectedToRobot() const {
    return communicationStatus() == DS::kFull;
}

//=============================================================================
//ProtocolBase::isConnectedToRadio
//=============================================================================

bool ProtocolBase::isConnectedToRadio() const {
    return m_radioConnected;
}

//=============================================================================
//ProtocolBase::sendDateTime
//=============================================================================

bool ProtocolBase::sendDateTime() const {
    return m_sendDateTime;
}

//=============================================================================
//ProtocolBase::hasVoltageBrownout
//=============================================================================

bool ProtocolBase::hasVoltageBrownout() const {
    return m_voltageBrownout;
}

//=============================================================================
//ProtocolBase::isEmergencyStopped
//=============================================================================

bool ProtocolBase::isEmergencyStopped() const {
    return m_emergencyStop;
}

//=============================================================================
//ProtocolBase::batteryVoltage
//=============================================================================

float ProtocolBase::batteryVoltage() const {
    return m_voltage;
}

//=============================================================================
//ProtocolBase::alliance
//=============================================================================

DS::Alliance ProtocolBase::alliance() const {
    return m_alliance;
}

//=============================================================================
//ProtocolBase::controlMode
//=============================================================================

DS::ControlMode ProtocolBase::controlMode() const {
    return m_controlMode;
}

//=============================================================================
//ProtocolBase::communicationStatus
//=============================================================================

DS::DS_CommStatus ProtocolBase::communicationStatus() const {
    return m_communicationStatus;
}

//=============================================================================
//ProtocolBase::joysticks
//=============================================================================

QList<DS::Joystick>* ProtocolBase::joysticks() const {
    return DriverStation::getInstance()->joysticks();
}

//=============================================================================
//ProtocolBase::radioAddress
//=============================================================================

QString ProtocolBase::radioAddress() {
    if (m_radioAddress.isEmpty())
        return defaultRadioAddress().at (m_radioIterator);

    return m_radioAddress;
}

//=============================================================================
//ProtocolBase::robotAddress
//=============================================================================

QString ProtocolBase::robotAddress() {
    if (m_robotAddress.isEmpty())
        return defaultRobotAddress().at (m_robotIterator);

    return m_robotAddress;
}

//=============================================================================
//ProtocolBase::createFMSPacket
//=============================================================================

QByteArray ProtocolBase::createFmsPacket() {
    m_sentFMSPackets += 1;
    return _getFmsPacket();
}

//=============================================================================
//ProtocolBase::createPacket
//=============================================================================

QByteArray ProtocolBase::createRobotPacket() {
    m_sentRobotPackets += 1;
    return _getClientPacket();
}

//=============================================================================
//ProtocolBase::reset
//=============================================================================

void ProtocolBase::reset() {
    /* Try another robot address */
    if (m_robotIterator >= defaultRobotAddress().count() - 1)
        m_robotIterator = 0;
    else
        m_robotIterator += 1;

    /* Try another radio address */
    if (m_radioIterator >= defaultRadioAddress().count() - 1)
        m_radioIterator = 0;
    else
        m_radioIterator += 1;

    /* Custom reset procedures for each protocol */
    _resetProtocol();

    /* Emit appropiate signals */
    updateVoltage      (0, 0);
    updateRobotCode    (false);
    updateRadioStatus  (false);
    updateSendDateTime (false);
    updateCommStatus   (DS::kFailing);

    /* Figure out the robot address and ping the robot */
    QHostInfo::lookupHost (robotAddress(),
                           this, SLOT (lookupFinished (QHostInfo)));

    /* Ping robot & radio */
    pingRadio();
    pingRobot();

    /* Lower the watchdog tolerance time to try more robot addresses faster */
    emit robotAddressChanged (robotAddress());
    m_watchdog.setTimeout (125);
}

//=============================================================================
//ProtocolBase::setTeam
//=============================================================================

void ProtocolBase::setTeam (int team) {
    if (team != m_team) {
        m_team = team;
        emit robotAddressChanged (robotAddress());

        DS::log (DS::kLibLevel,
                 "Team number set to: " + QString::number (m_team));
    }
}

//=============================================================================
//ProtocolBase::setEnabled
//=============================================================================

void ProtocolBase::setEnabled (bool enabled) {
    m_enabled = enabled;
    emit enabledChanged (isEnabled());
}

//=============================================================================
//ProtocolBase::setEmergencyStop
//=============================================================================

void ProtocolBase::setEmergencyStop (bool emergency_stop) {
    m_emergencyStop = emergency_stop;
    emit emergencyStoppedChanged (isEmergencyStopped());

    /* Stop sending e-stop packets after 1 second */
    if (m_emergencyStop)
        QTimer::singleShot (1000, this, SLOT (disableEmergencyStop()));

    emit emergencyStopped();
}

//=============================================================================
//ProtocolBase::setRobotAddress
//=============================================================================

void ProtocolBase::setRobotAddress (QString address) {
    m_robotAddress = address;
    emit robotAddressChanged (robotAddress());
}

//=============================================================================
//ProtocolBase::setAlliance
//=============================================================================

void ProtocolBase::setAlliance (DS::Alliance alliance) {
    m_alliance = alliance;
}

//=============================================================================
//ProtocolBase::setControlMode
//=============================================================================

void ProtocolBase::setControlMode (DS::ControlMode mode) {
    if (!isEmergencyStopped()) {
        m_controlMode = mode;
        emit controlModeChanged (controlMode());
    }
}

//=============================================================================
//ProtocolBase::readFMSPacket
//=============================================================================

void ProtocolBase::readFmsPacket (QByteArray data) {
    if (_readFMSPacket (data))
        emit fmsChanged (true);
}

//=============================================================================
//ProtocolBase::readRobotPacket
//=============================================================================

void ProtocolBase::readRobotPacket (QByteArray data) {
    if (!data.isEmpty()) {
        /* We just have connected to the robot, update internal values */
        if (!isConnectedToRobot()) {
            _getRobotInformation();

            setEnabled              (false);
            updateCommStatus        (DS::kFull);
            setControlMode          (DS::kControlTeleoperated);
            DS::sendMessage   (COMM_ESTABLISH);
            DS::log    (DS::kLibLevel,
                        "Robot/DS connection established!");

            /* Make the watchdog tolerance higher to avoid temporary resets */
            m_watchdog.setTimeout (2000);
        }

        /* Let the protocol implementation read the rest of the data */
        if (_readRobotPacket (data))
            emit packetReceived();
    }
}

//=============================================================================
//ProtocolBase::updateRobotCode
//=============================================================================

void ProtocolBase::updateRobotCode (bool available) {
    /* Robot code just crashed/failed */
    if (m_robotCode && !available) {
        setEnabled    (false);
        DS::sendMessage (CODE_CRASH);
        DS::log  (DS::kWarnLevel,
                  "It seems that the robot code crashed");
    }

    /* Update DS information */
    m_robotCode = available;
    emit codeChanged (m_robotCode);
}

//=============================================================================
//ProtocolBase::updateSendDateTime
//=============================================================================

void ProtocolBase::updateSendDateTime (bool sendDT) {
    m_sendDateTime = sendDT;
}

//=============================================================================
//ProtocolBase::updateRadioStatus
//=============================================================================

void ProtocolBase::updateRadioStatus (bool connected) {
    m_radioConnected = connected;
    emit radioCommChanged (m_radioConnected);
}

//=============================================================================
//ProtocolBase::updateCommStatus
//=============================================================================

void ProtocolBase::updateCommStatus (DS::DS_CommStatus status) {
    m_communicationStatus = status;
    emit communicationsChanged (m_communicationStatus);
}

//=============================================================================
//ProtocolBase::updateVoltageBrownout
//=============================================================================

void ProtocolBase::updateVoltageBrownout (bool brownout) {
    m_voltageBrownout = brownout;
    emit voltageBrownoutChanged (m_voltageBrownout);
}

//=============================================================================
//ProtocolBase::updateVoltage
//=============================================================================

void ProtocolBase::updateVoltage (QString digit, QString decimal) {
    /* Voltage is smaller than 10v, add 0 before the digit (e.g. to 09) */
    if (digit.length() < 2)
        digit.prepend ("0");

    /* Decimal voltage is less than 0.1v, add 0 to decimal (e.g 0.01) */
    if (decimal.length() < 2)
        decimal.prepend ("0");

    /* Decimal is too detailed, obtain only first two digits */
    else if (decimal.length() > 2)
        decimal = QString (decimal.at (0)) + QString (decimal.at (1));

    /* Construct voltage string */
    QString voltage = QString ("%1.%2").arg (digit, decimal);

    /* Update values & emit signals */
    m_voltage = voltage.toFloat();
    emit voltageChanged (voltage);
}

//=============================================================================
//ProtocolBase::pingRobot
//=============================================================================

void ProtocolBase::pingRobot() {
    m_robotPing.abort();
    m_robotPing.connectToHost (robotAddress(), tcpProbesPort(),
                               QTcpSocket::ReadOnly);
}

//=============================================================================
//ProtocolBase::pingRadio
//=============================================================================

void ProtocolBase::pingRadio() {
    m_radioPing.abort();
    m_radioPing.connectToHost (radioAddress(), 80, QTcpSocket::ReadOnly);
}

//=============================================================================
//ProtocolBase::showPatienceMsg
//=============================================================================

void ProtocolBase::showPatienceMsg() {
    DS::sendMessage (INFO_NOTE);
}

//=============================================================================
//ProtocolBase::disableEmergencyStop
//=============================================================================

void ProtocolBase::disableEmergencyStop() {
    setEmergencyStop (false);
}

//=============================================================================
//ProtocolBase::updateRobotIP
//=============================================================================

void ProtocolBase::lookupFinished (QHostInfo info) {
    if (!info.addresses().isEmpty())
        emit robotAddressChanged (info.addresses().first().toString());

    pingRobot();
}

//=============================================================================
//ProtocolBase::onPingResponse
//=============================================================================

void ProtocolBase::onPingResponse (QAbstractSocket::SocketState state) {
    bool isAlive = (state == QAbstractSocket::ConnectedState);

    /* Emitter is robot socket */
    if (sender()->objectName() == m_robotPing.objectName()) {
        if (communicationStatus() == DS::kFailing && isAlive)
            updateCommStatus (DS::kPartial);
    }

    /* Emitter is radio socket */
    else if (sender()->objectName() == m_radioPing.objectName())
        updateRadioStatus (isAlive);
}
