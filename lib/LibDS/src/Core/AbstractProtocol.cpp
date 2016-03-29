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
#include "LibDS/Core/AbstractProtocol.h"

using namespace DS_Core;

//==================================================================================================
// NetConsole warnings/information texts
//==================================================================================================

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
                               "The DS may take up to %1 seconds to detect "
                               "your robot, please be patient.</font>"
                               "</p>";
const QString COMM_ESTABLISH = "<p>"
                               "<font color=#59FF59><b>INFO:</b></font> "
                               "<font color=#FFFFFF>"
                               "Robot communications established with %1</font></p>";

//==================================================================================================
// AbstractProtocol::AbstractProtocol
//==================================================================================================

AbstractProtocol::AbstractProtocol() {
    m_team                = 0;
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
             this,         &AbstractProtocol::reset);
    connect (this,         &AbstractProtocol::packetReceived,
             &m_watchdog,  &Watchdog::restart);
    connect (&m_robotPing, &QTcpSocket::stateChanged,
             this,         &AbstractProtocol::onPingResponse);
    connect (&m_radioPing, &QTcpSocket::stateChanged,
             this,         &AbstractProtocol::onPingResponse);
    connect (&m_scanner,   &NetworkScanner::dataReceived,
             this,         &AbstractProtocol::onScannerResponse);

    generateIpLists();
    m_watchdog.setTimeout (200);
    QTimer::singleShot (100, Qt::CoarseTimer, this, SLOT (reset()));
    QTimer::singleShot (500, Qt::CoarseTimer, this, SLOT (showPatienceMsg()));
}

//==================================================================================================
// AbstractProtocol::team
//==================================================================================================

int AbstractProtocol::team() const {
    return m_team;
}

//==================================================================================================
// AbstractProtocol::hasCode
//==================================================================================================

bool AbstractProtocol::hasCode() const {
    return m_robotCode;
}

//==================================================================================================
// AbstractProtocol::expirationTime
//==================================================================================================

int AbstractProtocol::expirationTime() const {
    return m_watchdog.expirationTime();
}

//==================================================================================================
// AbstractProtocol::sentFMSPackets
//==================================================================================================

int AbstractProtocol::sentFmsPackets() const {
    return m_sentFMSPackets;
}

//==================================================================================================
// AbstractProtocol::sentRobotPackets
//==================================================================================================

int AbstractProtocol::sentRobotPackets() const {
    return m_sentRobotPackets;
}

//==================================================================================================
// AbstractProtocol::isEnabled
//==================================================================================================

bool AbstractProtocol::isEnabled() const {
    return m_enabled;
}

//==================================================================================================
// AbstractProtocol::isConnectedToRobot
//==================================================================================================

bool AbstractProtocol::isConnectedToRobot() const {
    return communicationStatus() == DS::kFull;
}

//==================================================================================================
// AbstractProtocol::isConnectedToRadio
//==================================================================================================

bool AbstractProtocol::isConnectedToRadio() const {
    return m_radioConnected;
}

//==================================================================================================
// AbstractProtocol::sendDateTime
//==================================================================================================

bool AbstractProtocol::sendDateTime() const {
    return m_sendDateTime;
}

//==================================================================================================
// AbstractProtocol::hasVoltageBrownout
//==================================================================================================

bool AbstractProtocol::hasVoltageBrownout() const {
    return m_voltageBrownout;
}

//==================================================================================================
// AbstractProtocol::isEmergencyStopped
//==================================================================================================

bool AbstractProtocol::isEmergencyStopped() const {
    return m_emergencyStop;
}

//==================================================================================================
// AbstractProtocol::batteryVoltage
//==================================================================================================

float AbstractProtocol::batteryVoltage() const {
    return m_voltage;
}

//==================================================================================================
// AbstractProtocol::alliance
//==================================================================================================

DS::Alliance AbstractProtocol::alliance() const {
    return m_alliance;
}

//==================================================================================================
// AbstractProtocol::controlMode
//==================================================================================================

DS::ControlMode AbstractProtocol::controlMode() const {
    return m_controlMode;
}

//==================================================================================================
// AbstractProtocol::communicationStatus
//==================================================================================================

DS::DS_CommStatus AbstractProtocol::communicationStatus() const {
    return m_communicationStatus;
}

//==================================================================================================
// AbstractProtocol::joysticks
//==================================================================================================

QList<DS::Joystick>* AbstractProtocol::joysticks() const {
    return DriverStation::getInstance()->joysticks();
}

//==================================================================================================
// AbstractProtocol::radioAddress
//==================================================================================================

QString AbstractProtocol::radioAddress() {
    return m_radioAddress;
}

//==================================================================================================
// AbstractProtocol::robotAddress
//==================================================================================================

QString AbstractProtocol::robotAddress() {
    return m_robotAddress;
}

//==================================================================================================
// AbstractProtocol::createFMSPacket
//==================================================================================================

QByteArray AbstractProtocol::createFmsPacket() {
    m_sentFMSPackets += 1;
    return _getFmsPacket();
}

//==================================================================================================
// AbstractProtocol::createPacket
//==================================================================================================

QByteArray AbstractProtocol::createRobotPacket() {
    m_sentRobotPackets += 1;
    QByteArray packetData = _getClientPacket();

    if (!isConnectedToRobot() && robotAddress().isEmpty())
        m_scanner.sendData (packetData);

    return packetData;
}

//==================================================================================================
// AbstractProtocol::radioIPs
//==================================================================================================

QStringList AbstractProtocol::radioIPs() {
    return m_radioIPs;
}

//==================================================================================================
// AbstractProtocol::robotIPs
//==================================================================================================

QStringList AbstractProtocol::robotIPs() {
    return m_robotIPs;
}

//==================================================================================================
// AbstractProtocol::reset
//==================================================================================================

void AbstractProtocol::reset() {
    /* Custom reset procedures for each protocol */
    _resetProtocol();

    /* Emit appropiate signals */
    updateVoltage      (0, 0);
    updateRobotCode    (false);
    updateRadioStatus  (false);
    updateSendDateTime (false);
    updateCommStatus   (DS::kFailing);

    /* Ping robot & radio */
    pingRadio();
    pingRobot();

    /* Lower the watchdog timeout for faster scanning */
    m_watchdog.setTimeout (200);

    /* Scan the next round of IP addresses */
    if (robotAddress().isEmpty()) {
        m_scanner.setEnabled (true);
        m_scanner.update();
    }
}

//==================================================================================================
// AbstractProtocol::setTeam
//==================================================================================================

void AbstractProtocol::setTeam (int team) {
    if (team != m_team) {
        m_team = team;
        generateIpLists();
        DS::log (DS::kLibLevel, "Team number set to: " + QString::number (m_team));
    }
}

//==================================================================================================
// AbstractProtocol::setEnabled
//==================================================================================================

void AbstractProtocol::setEnabled (bool enabled) {
    m_enabled = enabled;
    emit enabledChanged (isEnabled());
}

//==================================================================================================
// AbstractProtocol::setEmergencyStop
//==================================================================================================

void AbstractProtocol::setEmergencyStop (bool emergency_stop) {
    m_emergencyStop = emergency_stop;
    emit emergencyStoppedChanged (isEmergencyStopped());

    if (isEmergencyStopped())
        QTimer::singleShot (500, this, SLOT (disableEmergencyStop()));

    emit emergencyStopped();
}

//==================================================================================================
// AbstractProtocol::setRobotAddress
//==================================================================================================

void AbstractProtocol::setRobotAddress (QString address) {
    m_robotAddress = address;

    if (!isConnectedToRobot())
        emit robotAddressChanged (robotAddress());
}

//==================================================================================================
// AbstractProtocol::setAlliance
//==================================================================================================

void AbstractProtocol::setAlliance (DS::Alliance alliance) {
    m_alliance = alliance;
}

//==================================================================================================
// AbstractProtocol::setControlMode
//==================================================================================================

void AbstractProtocol::setControlMode (DS::ControlMode mode) {
    if (!isEmergencyStopped()) {
        m_controlMode = mode;
        emit controlModeChanged (controlMode());
    }
}

//==================================================================================================
// AbstractProtocol::readFMSPacket
//==================================================================================================

void AbstractProtocol::readFmsPacket (QByteArray data) {
    if (_readFMSPacket (data))
        emit fmsChanged (true);
}

//==================================================================================================
// AbstractProtocol::readRobotPacket
//==================================================================================================

void AbstractProtocol::readRobotPacket (QByteArray data) {
    if (data.isEmpty())
        return;

    if (!isConnectedToRobot()) {
        m_scanner.setEnabled (false);
        m_watchdog.setTimeout (1000);

        updateCommStatus (DS::kFull);
        setEnabled       (isEnabled());
        setRobotAddress  (robotAddress());
        setControlMode   (DS::kControlTeleoperated);
        DS::log          (DS::kLibLevel, "Robot/DS connection established!");

        _getRobotInformation();
    }

    if (_readRobotPacket (data))
        emit packetReceived();
}

//==================================================================================================
// AbstractProtocol::updateRobotCode
//==================================================================================================

void AbstractProtocol::updateRobotCode (bool available) {
    /* Robot code just crashed/failed */
    if (m_robotCode && !available)
        setEnabled (false);

    /* Update DS information */
    m_robotCode = available;
    emit codeChanged (m_robotCode);
}

//==================================================================================================
// AbstractProtocol::updateSendDateTime
//==================================================================================================

void AbstractProtocol::updateSendDateTime (bool sendDT) {
    m_sendDateTime = sendDT;
}

//==================================================================================================
// AbstractProtocol::updateRadioStatus
//==================================================================================================

void AbstractProtocol::updateRadioStatus (bool connected) {
    m_radioConnected = connected;
    emit radioCommChanged (m_radioConnected);
}

//==================================================================================================
// AbstractProtocol::updateCommStatus
//==================================================================================================

void AbstractProtocol::updateCommStatus (DS::DS_CommStatus status) {
    m_communicationStatus = status;
    emit communicationsChanged (m_communicationStatus);
}

//==================================================================================================
// AbstractProtocol::updateVoltageBrownout
//==================================================================================================

void AbstractProtocol::updateVoltageBrownout (bool brownout) {
    m_voltageBrownout = brownout;
    emit voltageBrownoutChanged (m_voltageBrownout);
}

//==================================================================================================
// AbstractProtocol::updateVoltage
//==================================================================================================

void AbstractProtocol::updateVoltage (QString digit, QString decimal) {
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

//==================================================================================================
// AbstractProtocol::pingRobot
//==================================================================================================

void AbstractProtocol::pingRobot() {
    m_robotPing.abort();
    m_robotPing.connectToHost (robotAddress(), tcpProbesPort(), QTcpSocket::ReadOnly);
}

//==================================================================================================
// AbstractProtocol::pingRadio
//==================================================================================================

void AbstractProtocol::pingRadio() {
    m_radioPing.abort();
    m_radioPing.connectToHost (radioAddress(), 80, QTcpSocket::ReadOnly);
}

//==================================================================================================
// AbstractProtocol::generateIpLists
//==================================================================================================

void AbstractProtocol::generateIpLists() {
    /* Clear the IPs lists */
    m_robotIPs.clear();
    m_radioIPs.clear();

    /* Generate radio IPs */
    m_radioIPs.append (_extraRadioIPs());
    m_radioIPs.append (DS::getStaticIP (10, team(), 1));

    /* Generate robot IPs */
    m_robotIPs.append (_extraRobotIPs());
    m_robotIPs.append ("127.0.0.1");

    /* Generate robot IPs based on the current IP(s) of the computer */
    foreach (const QNetworkInterface& interface, QNetworkInterface::allInterfaces()) {
        bool isUp       = (bool) (interface.flags() & QNetworkInterface::IsUp);
        bool isRunning  = (bool) (interface.flags() & QNetworkInterface::IsRunning);

        /* Only take into account useful network interfaces */
        if (isUp && isRunning) {
            foreach (const QNetworkAddressEntry& address, interface.addressEntries()) {
                QStringList numbers = address.ip().toString().split (".");
                bool valid = (address.ip() != QHostAddress ("127.0.0.1")) &&
                             (address.ip().protocol() == QAbstractSocket::IPv4Protocol);

                /* Ensure that the current IP is IPv4 and is valid */
                if (numbers.count() == 4 && valid) {
                    QString base = QString ("%1.%2.%3.")
                                   .arg (numbers.at (0))
                                   .arg (numbers.at (1))
                                   .arg (numbers.at (2));

                    /* Add all other IPs in the selected network */
                    for (int i = 1; i < 255; ++i)
                        m_robotIPs.append (base + QString::number (i));
                }
            }
        }
    }

    /* Re-configure the network scanner */
    m_scanner.setScanningList (m_robotIPs);
    m_scanner.setInputPort (robotInputPort());
    m_scanner.setOutputPort (robotOutputPort());

    /* Log information */
    DS::log (DS::kLibLevel, QString ("Generated %1 radio IPs").arg (m_radioIPs.count()));
    DS::log (DS::kLibLevel, QString ("Generated %1 robot IPs").arg (m_robotIPs.count()));
}

//==================================================================================================
// AbstractProtocol::showPatienceMsg
//==================================================================================================

void AbstractProtocol::showPatienceMsg() {
    /* Get total scanning time, convert to seconds and round to nearest 10 */
    double msec = (robotIPs().count() * expirationTime()) / m_scanner.scannerCount();
    double time = ceil ((msec / 1000) / 10) * 10;

    /* Display the message */
    DS::sendMessage (INFO_NOTE.arg (time));
}

//==================================================================================================
// AbstractProtocol::disableEmergencyStop
//==================================================================================================

void AbstractProtocol::disableEmergencyStop() {
    setEmergencyStop (false);
}

//==================================================================================================
// AbstractProtocol::onScannerResponse
//==================================================================================================

void AbstractProtocol::onScannerResponse (QString ip, QByteArray data) {
    setRobotAddress (ip);
    readRobotPacket (data);
}

//==================================================================================================
// AbstractProtocol::onPingResponse
//==================================================================================================

void AbstractProtocol::onPingResponse (QAbstractSocket::SocketState state) {
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
