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
// NetConsole warnings/information texts
//=============================================================================

const QString CHECK_FIREWALL = "<p>"
                               "<font color=#FE755C>ERROR: </font>"
                               "<font color=#FFFFFF>Cannot connect to robot</font><br/>"
                               "<font color=#20C9FF>FRC: The robot is slow to respond. "
                               "Check firewall settings.</font>"
                               "</p>";
const QString PARTIAL_COMM   = "<p>"
                               "<font color=#FFF959>WARNING: </font>"
                               "<font color=#FFFFFF>%1 responds to ping requests, "
                               "but does not respond to DS packets.</font>"
                               "</p>";

//=============================================================================
// DS_ProtocolBase::DS_ProtocolBase
//=============================================================================

DS_ProtocolBase::DS_ProtocolBase()
{
    m_team = 0;
    m_resetCount = 0;
    m_sentPackets = 0;
    m_radioIterator = 0;
    m_robotIterator = 0;

    m_robotAddress = QString ("");

    m_alliance = kAllianceRed1;
    m_controlMode = kControlInvalid;
    m_communicationStatus = kFailing;

    m_enabled = false;
    m_robotCode = false;
    m_emergencyStop = false;
    m_radioConnected = false;

    m_joysticks = new QList<DS_Joystick*>;

    m_radioPing.setObjectName ("Radio Ping");
    m_robotPing.setObjectName ("Robot Ping");

    connect (&m_watchdog, SIGNAL (timeout()), this, SLOT (reset()));
    connect (this, SIGNAL (packetReceived()), &m_watchdog, SLOT (restart()));

    connect (&m_robotPing,  SIGNAL (stateChanged   (QAbstractSocket::SocketState)),
             this,            SLOT (onPingResponse (QAbstractSocket::SocketState)));
    connect (&m_radioPing,  SIGNAL (stateChanged   (QAbstractSocket::SocketState)),
             this,            SLOT (onPingResponse (QAbstractSocket::SocketState)));
    connect (&m_discovery,  SIGNAL (ipFound        (QString, QString)),
             this,            SLOT (updateRobotIP  (QString, QString)));

    reset();
}

//=============================================================================
// DS_ProtocolBase::~DS_ProtocolBase
//=============================================================================

DS_ProtocolBase::~DS_ProtocolBase()
{
    m_robotPing.abort();
    delete m_joysticks;
}

//=============================================================================
// DS_ProtocolBase::team
//=============================================================================

int DS_ProtocolBase::team() const
{
    return m_team;
}

//=============================================================================
// DS_ProtocolBase::statusCode
//=============================================================================

int DS_ProtocolBase::statusCode() const
{
    return m_status;
}

//=============================================================================
// DS_ProtocolBase::hasCode
//=============================================================================

bool DS_ProtocolBase::hasCode() const
{
    return m_robotCode;
}

//=============================================================================
// DS_ProtocolBase::sentPackets
//=============================================================================

int DS_ProtocolBase::sentPackets() const
{
    return m_sentPackets;
}

//=============================================================================
// DS_ProtocolBase::isEnabled
//=============================================================================

bool DS_ProtocolBase::isEnabled() const
{
    return m_enabled;
}

//=============================================================================
// DS_ProtocolBase::isConnectedToRobot
//=============================================================================

bool DS_ProtocolBase::isConnectedToRobot() const
{
    return communicationStatus() == kFull;
}

//=============================================================================
// DS_ProtocolBase::isConnectedToRadio
//=============================================================================

bool DS_ProtocolBase::isConnectedToRadio() const
{
    return m_radioConnected;
}

//=============================================================================
// DS_ProtocolBase::sendDateTime
//=============================================================================

bool DS_ProtocolBase::sendDateTime() const
{
    return m_sendDateTime;
}

//=============================================================================
// DS_ProtocolBase::hasVoltageBrownout
//=============================================================================

bool DS_ProtocolBase::hasVoltageBrownout() const
{
    return m_voltageBrownout;
}

//=============================================================================
// DS_ProtocolBase::isEmergencyStopped
//=============================================================================

bool DS_ProtocolBase::isEmergencyStopped() const
{
    return m_emergencyStop;
}

//=============================================================================
// DS_ProtocolBase::alliance
//=============================================================================

DS_Alliance DS_ProtocolBase::alliance() const
{
    return m_alliance;
}

//=============================================================================
// DS_ProtocolBase::controlMode
//=============================================================================

DS_ControlMode DS_ProtocolBase::controlMode() const
{
    return m_controlMode;
}

//=============================================================================
// DS_ProtocolBase::communicationStatus
//=============================================================================

DS_CommStatus DS_ProtocolBase::communicationStatus() const
{
    return m_communicationStatus;
}

//=============================================================================
// DS_ProtocolBase::joysticks
//=============================================================================

QList<DS_Joystick*>* DS_ProtocolBase::joysticks() const
{
    return m_joysticks;
}

//=============================================================================
// DS_ProtocolBase::radioAddress
//=============================================================================

QString DS_ProtocolBase::radioAddress()
{
    if (m_radioAddress.isEmpty())
        return defaultRadioAddress().at (m_radioIterator);

    return m_radioAddress;
}

//=============================================================================
// DS_ProtocolBase::robotAddress
//=============================================================================

QString DS_ProtocolBase::robotAddress()
{
    if (m_robotAddress.isEmpty())
        return defaultRobotAddress().at (m_robotIterator);

    return m_robotAddress;
}

//=============================================================================
// DS_ProtocolBase::createPacket
//=============================================================================

QByteArray DS_ProtocolBase::createPacket()
{
    m_sentPackets += 1;
    return getClientPacket();
}

//=============================================================================
// DS_ProtocolBase::reset
//=============================================================================

void DS_ProtocolBase::reset()
{
    m_resetCount += 1;

    /* Notify user that communication is partial */
    if (m_resetCount == 3 && communicationStatus() == kPartial)
        DS_SendMessage (PARTIAL_COMM.arg (robotAddress()));

    /* Warn user that we cannot communicate with robot */
    if (m_resetCount >= 3)
        {
            m_resetCount = 0;

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
        }

    /* Custom reset procedures for each protocol */
    resetProtocol();

    /* Emit appropiate signals */
    updateVoltage (0, 0);
    updateRobotCode (false);
    updateRadioStatus (false);
    updateSendDateTime (false);
    updateCommStatus (kFailing);

    /* Figure out the robot address and ping the robot */
    m_discovery.getIP (robotAddress());
    emit robotAddressChanged (robotAddress());

    /* Ping robot & radio */
    pingRadio();
    pingRobot();
}

//=============================================================================
// DS_ProtocolBase::setTeam
//=============================================================================

void DS_ProtocolBase::setTeam (int team)
{
    m_team = team;
    emit robotAddressChanged (robotAddress());
}

//=============================================================================
// DS_ProtocolBase::setEnabled
//=============================================================================

void DS_ProtocolBase::setEnabled (bool enabled)
{
    m_enabled = enabled;
    emit enabledChanged (isEnabled());
}

//=============================================================================
// DS_ProtocolBase::setEmergencyStop
//=============================================================================

void DS_ProtocolBase::setEmergencyStop (bool emergency_stop)
{
    m_emergencyStop = emergency_stop;
    emit emergencyStoppedChanged (isEmergencyStopped());

    /* Stop sending e-stop packets after 1 second */
    if (m_emergencyStop)
        QTimer::singleShot (1000, this, SLOT (disableEmergencyStop()));

    emit emergencyStopped();
}

//=============================================================================
// DS_ProtocolBase::setRobotAddress
//=============================================================================

void DS_ProtocolBase::setRobotAddress (QString address)
{
    m_robotAddress = address;
    emit robotAddressChanged (robotAddress());
}

//=============================================================================
// DS_ProtocolBase::setAlliance
//=============================================================================

void DS_ProtocolBase::setAlliance (DS_Alliance alliance)
{
    m_alliance = alliance;
}

//=============================================================================
// DS_ProtocolBase::setControlMode
//=============================================================================

void DS_ProtocolBase::setControlMode (DS_ControlMode mode)
{
    if (!isEmergencyStopped())
        {
            m_controlMode = mode;
            emit controlModeChanged (controlMode());
        }
}

//=============================================================================
// DS_ProtocolBase::setJoysticks
//=============================================================================

void DS_ProtocolBase::setJoysticks (QList<DS_Joystick*>* joysticks)
{
    m_joysticks = joysticks;
}

//=============================================================================
// DS_ProtocolBase::readRobotPacket
//=============================================================================

void DS_ProtocolBase::readRobotPacket (QByteArray data)
{
    if (!data.isEmpty())
        {
            /* We just have connected to the robot, update internal values */
            if (!isConnectedToRobot())
                {
                    getRobotInformation();

                    setEnabled (false);
                    updateCommStatus (kFull);
                    setControlMode (kControlTeleoperated);
                }

            /* Let the protocol implementation read the rest of the data */
            if (readPacket (data))
                emit packetReceived();
        }
}

//=============================================================================
// DS_ProtocolBase::updateStatus
//=============================================================================

void DS_ProtocolBase::updateStatus (int status)
{
    m_status = status;
}

//=============================================================================
// DS_ProtocolBase::updateRobotCode
//=============================================================================

void DS_ProtocolBase::updateRobotCode (bool available)
{
    /* Robot code just crashed/failed */
    if (m_robotCode && !available)
        setEnabled (false);

    /* Update DS information */
    m_robotCode = available;
    emit codeChanged (m_robotCode);
}

//=============================================================================
// DS_ProtocolBase::updateSendDateTime
//=============================================================================

void DS_ProtocolBase::updateSendDateTime (bool sendDT)
{
    m_sendDateTime = sendDT;
}

//=============================================================================
// DS_ProtocolBase::updateRadioStatus
//=============================================================================

void DS_ProtocolBase::updateRadioStatus (bool connected)
{
    m_radioConnected = connected;
    emit radioCommChanged (m_radioConnected);
}

//=============================================================================
// DS_ProtocolBase::updateCommStatus
//=============================================================================

void DS_ProtocolBase::updateCommStatus (DS_CommStatus status)
{
    m_communicationStatus = status;
    emit communicationsChanged (m_communicationStatus);
}

//=============================================================================
// DS_ProtocolBase::updateVoltageBrownout
//=============================================================================

void DS_ProtocolBase::updateVoltageBrownout (bool brownout)
{
    m_voltageBrownout = brownout;
    emit voltageBrownoutChanged (m_voltageBrownout);
}

//=============================================================================
// DS_ProtocolBase::updateVoltage
//=============================================================================

void DS_ProtocolBase::updateVoltage (int major, int minor)
{
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

//=============================================================================
// DS_ProtocolBase::pingRobot
//=============================================================================

void DS_ProtocolBase::pingRobot()
{
    m_robotPing.abort();
    m_robotPing.connectToHost (robotAddress(), tcpProbePort(),
                               QTcpSocket::ReadOnly);
}

//=============================================================================
// DS_ProtocolBase::pingRadio
//=============================================================================

void DS_ProtocolBase::pingRadio()
{
    m_radioPing.abort();
    m_radioPing.connectToHost (radioAddress(), 80, QTcpSocket::ReadOnly);
}

//=============================================================================
// DS_ProtocolBase::disableEmergencyStop
//=============================================================================

void DS_ProtocolBase::disableEmergencyStop()
{
    setEmergencyStop (false);
}

//=============================================================================
// DS_ProtocolBase::updateRobotIP
//=============================================================================

void DS_ProtocolBase::updateRobotIP (QString address, QString ip)
{
    if (address.toLower() == robotAddress().toLower() && address != ip)
        emit robotAddressChanged (ip);

    pingRobot();
}

//=============================================================================
// DS_ProtocolBase::onPingResponse
//=============================================================================

void DS_ProtocolBase::onPingResponse (QAbstractSocket::SocketState state)
{
    bool isAlive = (state == QAbstractSocket::ConnectedState);

    /* Emitter is robot socket */
    if (sender()->objectName() == m_robotPing.objectName())
        {
            if (communicationStatus() == kFailing && isAlive)
                updateCommStatus (kPartial);
        }

    /* Emitter is radio socket */
    else if (sender()->objectName() == m_radioPing.objectName())
        updateRadioStatus (isAlive);
}
