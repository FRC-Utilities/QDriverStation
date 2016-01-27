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

#include <QSound>

//=============================================================================
// Core components
//=============================================================================

#include "LibDS/Core/Timers.h"
#include "LibDS/Core/Client.h"
#include "LibDS/Core/NetConsole.h"
#include "LibDS/Core/ElapsedTimer.h"
#include "LibDS/Core/ProtocolManager.h"

//=============================================================================
// Protocols
//=============================================================================

#include "LibDS/Protocols/Protocol2015.h"
#include "LibDS/Protocols/Protocol2016.h"

//=============================================================================
// Library class
//=============================================================================

#include "LibDS/DriverStation.h"

//=============================================================================
// Library instance
//=============================================================================

DriverStation* DriverStation::s_instance = Q_NULLPTR;

//=============================================================================
// DriverStation::DriverStation
//=============================================================================

DriverStation::DriverStation()
{
    m_init = false;

    /* Initialize private members */
    m_client      = new DS_Client;
    m_netConsole  = new DS_NetConsole;
    m_elapsedTime = new DS_ElapsedTime;
    m_manager     = new DS_ProtocolManager;
    m_protocol    = Q_NULLPTR;

    /* Update internal values and notify object on robot status events */
    connect (m_manager, SIGNAL (codeChanged            (bool)),
             this,        SLOT (updateRobotStatus      (bool)));
    connect (m_manager, SIGNAL (communicationsChanged  (DS_CommStatus)),
             this,        SLOT (updateRobotStatus      (DS_CommStatus)));
    connect (m_manager, SIGNAL (controlModeChanged     (DS_ControlMode)),
             this,        SLOT (resetElapsedTimer      (DS_ControlMode)));
    connect (m_manager, SIGNAL (codeChanged            (bool)),
             this,      SIGNAL (codeChanged            (bool)));
    connect (m_manager, SIGNAL (communicationsChanged  (DS_CommStatus)),
             this,      SIGNAL (communicationsChanged  (DS_CommStatus)));
    connect (m_manager, SIGNAL (controlModeChanged     (DS_ControlMode)),
             this,      SIGNAL (controlModeChanged     (DS_ControlMode)));
    connect (m_manager, SIGNAL (diskUsageChanged       (int)),
             this,      SIGNAL (diskUsageChanged       (int)));
    connect (m_manager, SIGNAL (ramUsageChanged        (int)),
             this,      SIGNAL (ramUsageChanged        (int)));
    connect (m_manager, SIGNAL (cpuUsageChanged        (int)),
             this,      SIGNAL (cpuUsageChanged        (int)));
    connect (m_manager, SIGNAL (voltageChanged         (QString)),
             this,      SIGNAL (voltageChanged         (QString)));
    connect (m_manager, SIGNAL (voltageBrownoutChanged (bool)),
             this,      SIGNAL (voltageBrownoutChanged (bool)));
    connect (m_manager, SIGNAL (CANInfoReceived        (DS_CAN)),
             this,      SIGNAL (CANInfoReceived        (DS_CAN)));
    connect (m_manager, SIGNAL (emergencyStopped       (void)),
             this,      SIGNAL (emergencyStopped       (void)));
    connect (m_manager, SIGNAL (fmsChanged             (bool)),
             this,      SIGNAL (fmsChanged             (bool)));

    /* Stop timer when the communications status changes */
    connect (m_manager,     SIGNAL (communicationsChanged (DS_CommStatus)),
             m_elapsedTime,   SLOT (stopTimer()));

    /* Robot information has changed */
    connect (m_manager, SIGNAL (libVersionChanged (QString)),
             this,      SIGNAL (libVersionChanged (QString)));
    connect (m_manager, SIGNAL (rioVersionChanged (QString)),
             this,      SIGNAL (rioVersionChanged (QString)));
    connect (m_manager, SIGNAL (pcmVersionChanged (QString)),
             this,      SIGNAL (pcmVersionChanged (QString)));
    connect (m_manager, SIGNAL (pdpVersionChanged (QString)),
             this,      SIGNAL (pdpVersionChanged (QString)));

    /* Sync robot address and calculated IPs automatically */
    connect (m_manager, SIGNAL (robotAddressChanged (QString)),
             m_client,  SLOT   (setRobotAddress     (QString)));

    /* Update the elapsed time text automatically */
    connect (m_elapsedTime, SIGNAL (elapsedTimeChanged (QString)),
             this,          SIGNAL (elapsedTimeChanged (QString)));

    /* New NetConsole message received */
    connect (m_netConsole,  SIGNAL (newMessage (QString)),
             this,          SIGNAL (newMessage (QString)));

    /* Send and read robot packets */
    connect (m_client,  SIGNAL (dataReceived     (QByteArray)),
             this,        SLOT (readRobotPacket  (QByteArray)));
    connect (DS_Timers::getInstance(), SIGNAL    (timeout20()),
             this,                       SLOT    (sendClientPacket()));
}

//=============================================================================
// DriverStation::~DriverStation
//=============================================================================

DriverStation::~DriverStation()
{
    delete m_client;
    delete m_manager;
    delete m_protocol;
    delete m_netConsole;
    delete m_elapsedTime;

    delete s_instance;
}

//=============================================================================
// DriverStation::getInstance
//=============================================================================

DriverStation* DriverStation::getInstance()
{
    if (s_instance == Q_NULLPTR)
        s_instance = new DriverStation();

    return s_instance;
}

//=============================================================================
// DriverStation::canBeEnabled
//=============================================================================

bool DriverStation::canBeEnabled()
{
    return robotHasCode() && isConnected() && !isEmergencyStopped();
}

//=============================================================================
// DriverStation::alliances
//=============================================================================

QStringList DriverStation::alliances()
{
    QStringList list;

    list.append ("Red 1");
    list.append ("Red 2");
    list.append ("Red 3");
    list.append ("Blue 1");
    list.append ("Blue 2");
    list.append ("Blue 3");

    return list;
}

//=============================================================================
// DriverStation::protocols
//=============================================================================

QStringList DriverStation::protocols()
{
    QStringList list;
    list.append ("2016 Protocol");
    list.append ("2015 Protocol");
    return list;
}

//=============================================================================
// DriverStation::radioAddress
//=============================================================================

QString DriverStation::radioAddress()
{
    if (m_manager->isValid())
        return m_manager->currentProtocol()->defaultRadioAddress().at (0);

    return "";
}

//=============================================================================
// DriverStation::robotAddress
//=============================================================================

QString DriverStation::robotAddress()
{
    if (m_manager->isValid())
        return m_manager->currentProtocol()->robotAddress();

    return "";
}

//=============================================================================
// DriverStation::defaultRobotAddress
//=============================================================================

QString DriverStation::defaultRobotAddress()
{
    if (m_manager->isValid())
        return m_manager->currentProtocol()->defaultRobotAddress().at (0);

    return "";
}

//=============================================================================
// DriverStation::defaultRobotAddresses
//=============================================================================

QStringList DriverStation::defaultRobotAddresses()
{
    if (m_manager->isValid())
        return m_manager->currentProtocol()->defaultRobotAddress();

    return QStringList ("");
}

//=============================================================================
// DriverStation::controlMode
//=============================================================================

DS_ControlMode DriverStation::controlMode()
{
    if (m_manager->isValid())
        return m_manager->currentProtocol()->controlMode();

    return kControlInvalid;
}

//=============================================================================
// DriverStation::robotHasCode
//=============================================================================

bool DriverStation::robotHasCode()
{
    if (m_manager->isValid())
        return m_manager->currentProtocol()->hasCode();

    return false;
}

//=============================================================================
// DriverStation::isEnabled
//=============================================================================

bool DriverStation::isEnabled()
{
    if (m_manager->isValid())
        return m_manager->currentProtocol()->isEnabled();

    return false;
}

//=============================================================================
// DriverStation::isEmergencyStopped
//=============================================================================

bool DriverStation::isEmergencyStopped()
{
    if (m_manager->isValid())
        return m_manager->currentProtocol()->isEmergencyStopped();

    return false;
}

//=============================================================================
// DriverStation::isConnected
//=============================================================================

bool DriverStation::isConnected()
{
    if (m_manager->isValid())
        return m_manager->currentProtocol()->isConnectedToRobot();

    return false;
}

//=============================================================================
// DriverStation::joystickCount
//=============================================================================

int DriverStation::joystickCount()
{
    return m_manager->joystickCount();
}

//=============================================================================
// DriverStation::init
//=============================================================================

void DriverStation::init()
{
    if (!m_init)
        {
            m_init = true;

            QTimer::singleShot (500, this, SIGNAL (initialized()));
            QTimer::singleShot (500, this, SLOT   (resetEverything()));
            QTimer::singleShot (500, DS_Timers::getInstance(), SLOT (start()));
        }
}

//=============================================================================
// DriverStation::rebootRobot
//=============================================================================

void DriverStation::rebootRobot()
{
    if (m_manager->isValid())
        m_manager->currentProtocol()->reboot();
}

//=============================================================================
// DriverStation::restartRobotCode
//=============================================================================

void DriverStation::restartCode()
{
    if (m_manager->isValid())
        m_manager->currentProtocol()->restartCode();
}

//=============================================================================
// DriverStation::setEnabled
//=============================================================================

void DriverStation::setEnabled (bool enabled)
{
    if (m_manager->isValid())
        {
            m_manager->currentProtocol()->setEnabled (enabled);

            if (enabled)
                m_elapsedTime->resetTimer();

            else
                m_elapsedTime->stopTimer();
        }
}

//=============================================================================
// DriverStation::setEmergencyStop
//=============================================================================

void DriverStation::setEmergencyStop (bool emergency_stop)
{
    if (m_manager->isValid())
        m_manager->currentProtocol()->setEmergencyStop (emergency_stop);
}

//=============================================================================
// DriverStation::startPractice
//=============================================================================

void DriverStation::startPractice (int countdown,
                                   int autonomous,
                                   int delay,
                                   int teleop,
                                   int endgame)
{
    qDebug() << Q_FUNC_INFO << "TODO";

    /* Transform the times from seconds to milliseconds */
    delay *= 1000;
    teleop *= 1000;
    endgame *= 1000;
    countdown *= 1000;
    autonomous *= 1000;

    /* Calculate the time frames */
    quint32 _startAutonomous = countdown;
    quint32 _startTeleoperated = _startAutonomous + delay + autonomous;
    quint32 _startEndGame = _startTeleoperated + delay + teleop - endgame;
    quint32 _stop = _startEndGame + endgame;

    /* Configure the sound/media timers */
    QTimer::singleShot (_startAutonomous, Qt::PreciseTimer,
                        this, SLOT (playAutonomousStart()));
    QTimer::singleShot (_startTeleoperated, Qt::PreciseTimer,
                        this, SLOT (playTeleopStart()));
    QTimer::singleShot (_stop, Qt::PreciseTimer,
                        this, SLOT (playEndGame()));
    QTimer::singleShot (_startEndGame, Qt::PreciseTimer,
                        this, SLOT (playEndGame()));
}

//=============================================================================
// DriverStation::setProtocol
//=============================================================================

void DriverStation::setProtocol (DS_ProtocolBase* protocol)
{
    if (protocol != Q_NULLPTR)
        {
            /* Pass current protocol settings to new protocol */
            if (m_manager->isValid())
                {
                    int team = m_manager->currentProtocol()->team();
                    QString address = m_manager->currentProtocol()->robotAddress();

                    protocol->setTeam (team);
                    protocol->setRobotAddress (address);
                }

            /* Configure the DS modules with the new protocol */
            m_manager->setProtocol  (protocol);
            m_client->setRobotPort  (protocol->robotPort());
            m_client->setClientPort (protocol->clientPort());
            m_netConsole->setPort   (protocol->netConsolePort());

            emit protocolChanged();
        }
}

//=============================================================================
// DriverStation::setProtocol
//=============================================================================

void DriverStation::setProtocol (ProtocolType protocol)
{
    if (protocol == kProtocol2015)
        {
            m_protocol = new DS_Protocol2015;
            setProtocol (m_protocol);
        }

    if (protocol == kProtocol2016)
        {
            m_protocol = new DS_Protocol2016;
            setProtocol (m_protocol);
        }
}

//=============================================================================
// DriverStation::setTeamNumber
//=============================================================================

void DriverStation::setTeamNumber (int team)
{
    if (m_manager->isValid())
        m_manager->currentProtocol()->setTeam (team);

    emit teamChanged (team);
}

//=============================================================================
// DriverStation::setAlliance
//=============================================================================

void DriverStation::setAlliance (int alliance)
{
    setAlliance ((DS_Alliance) alliance);
}

//=============================================================================
// DriverStation::setAlliance
//=============================================================================

void DriverStation::setAlliance (DS_Alliance alliance)
{
    if (m_manager->isValid())
        m_manager->currentProtocol()->setAlliance (alliance);
}

//=============================================================================
// DriverStation::setControlMode
//=============================================================================

void DriverStation::setControlMode (DS_ControlMode mode)
{
    if (m_manager->isValid())
        m_manager->currentProtocol()->setControlMode (mode);
}

//=============================================================================
// DriverStation::setRobotAddress
//=============================================================================

void DriverStation::setRobotAddress (QString address)
{
    if (m_manager->isValid())
        m_manager->currentProtocol()->setRobotAddress (address);
}

//=============================================================================
// DriverStation::resetJoysticks
//=============================================================================

void DriverStation::resetJoysticks()
{
    m_manager->resetJoysticks();
    emit joystickCountChanged();
}

//=============================================================================
// DriverStation::updateJoystickPOV
//=============================================================================

void DriverStation::updateJoystickPOV (int js, int hat, int angle)
{
    if (m_manager->isValid())
        m_manager->updateJoystickPOV (js, hat, angle);
}

//=============================================================================
// DriverStation::addJoystick
//=============================================================================

void DriverStation::addJoystick (int axes, int buttons, int povHats)
{
    if (m_manager->isValid())
        {
            m_manager->addJoystick (axes, buttons, povHats);
            emit joystickCountChanged();
        }
}

//=============================================================================
// DriverStation::updateJoystickAxis
//=============================================================================

void DriverStation::updateJoystickAxis (int js, int axis, double value)
{
    if (m_manager->isValid())
        m_manager->updateJoystickAxis (js, axis, value);
}

//=============================================================================
// DriverStation::updateJoystickButton
//=============================================================================

void DriverStation::updateJoystickButton (int js, int button, bool pressed)
{
    if (m_manager->isValid())
        m_manager->updateJoystickButton (js, button, pressed);
}

//=============================================================================
// DriverStation::getRobotStatus
//=============================================================================

QString DriverStation::getRobotStatus()
{
    if (m_manager->isValid()
            && m_manager->currentProtocol()->isConnectedToRobot())
        {
            if (m_manager->currentProtocol()->isEmergencyStopped())
                return "Emergency Stopped";

            if (!m_manager->currentProtocol()->hasCode())
                return "No Robot Code";

            return QString ("%1 %2")
                   .arg (DS_GetControlModeString (controlMode()))
                   .arg (isEnabled() ? "Enabled" : "Disabled");
        }

    return QString ("No Robot Communication");
}

//=============================================================================
// DriverStation::sendClientPacket
//=============================================================================

void DriverStation::sendClientPacket()
{
    if (m_manager->isValid())
        m_client->sendPacket (m_manager->currentProtocol()->createPacket());
}

//=============================================================================
// DriverStation::resetEverything
//=============================================================================

void DriverStation::resetEverything()
{
    m_elapsedTime->resetTimer();
    m_elapsedTime->stopTimer();

    emit codeChanged (false);
    emit voltageChanged (QString (""));
    emit elapsedTimeChanged ("00:00.0");
    emit communicationsChanged (kFailing);
}

//=============================================================================
// DriverStation::readRobotPacket
//=============================================================================

void DriverStation::readRobotPacket (QByteArray robotResponse)
{
    if (m_manager->isValid())
        m_manager->currentProtocol()->readRobotPacket (robotResponse);
}

//=============================================================================
// DriverStation::updateRobotStatus
//=============================================================================

void DriverStation::updateRobotStatus (bool ignored)
{
    Q_UNUSED (ignored);
    emit robotStatusChanged (getRobotStatus());
}

//=============================================================================
// DriverStation::updateRobotStatus
//=============================================================================

void DriverStation::updateRobotStatus (DS_CommStatus status)
{
    emit robotStatusChanged (getRobotStatus());
    emit communicationsChanged (status == kFull);
}

//=============================================================================
// DriverStation::resetElapsedTimer
//=============================================================================

void DriverStation::resetElapsedTimer (DS_ControlMode mode)
{
    Q_UNUSED (mode);
    emit robotStatusChanged (getRobotStatus());
}

//=============================================================================
// DriverStation::playEndGameApproaching
//=============================================================================

void DriverStation::playEndGameApproaching()
{
    QSound::play (":/LibDS/Start of End Game_normalized.wav");
}

//=============================================================================
// DriverStation::playEndGame
//=============================================================================

void DriverStation::playEndGame()
{
    QSound::play (":/LibDS/Match End_normalized.wav");
}

//=============================================================================
// DriverStation::playTeleopStart
//=============================================================================

void DriverStation::playTeleopStart()
{
    QSound::play (":/LibDS/Start Teleop_normalized.wav");
}

//=============================================================================
// DriverStation::playAutonomousStart
//=============================================================================

void DriverStation::playAutonomousStart()
{
    QSound::play (":/LibDS/Start Auto_normalized.wav");
}
