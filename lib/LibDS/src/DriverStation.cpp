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

//=============================================================================
// System includes
//=============================================================================

#include <QTimer>
#include <QSound>
#include <QThread>
#include <QApplication>

//=============================================================================
// Core components
//=============================================================================

#include "LibDS/Core/Client.h"
#include "LibDS/Core/NetConsole.h"
#include "LibDS/Core/ElapsedTimer.h"
#include "LibDS/Core/ProtocolManager.h"

//=============================================================================
// Protocols
//=============================================================================

#include "LibDS/Protocols/Protocol2014.h"
#include "LibDS/Protocols/Protocol2015.h"
#include "LibDS/Protocols/Protocol2016.h"

//=============================================================================
// Library class
//=============================================================================

#include "LibDS/DriverStation.h"

//=============================================================================
// DriverStation::DriverStation
//=============================================================================

DriverStation::DriverStation()
{
    /* Register the DS types so that Qt does not bitch about the connections */
    qRegisterMetaType <DS_CAN>           ("DS_CAN");
    qRegisterMetaType <DS_Joystick>      ("DS_Joystick");
    qRegisterMetaType <DS_Alliance>      ("DS_Alliance");
    qRegisterMetaType <DS_CommStatus>    ("DS_CommStatus");
    qRegisterMetaType <DS_ControlMode>   ("DS_ControlMode");
    qRegisterMetaType <DS_RumbleRequest> ("DS_RumbleRequest");

    /* This is changed with the init() function, which must be called by
     * the programmer when the UI is loaded */
    m_init = false;

    /* Initialize private members */
    m_client      = new DS_Client          (this);
    m_netConsole  = new DS_NetConsole      (this);
    m_elapsedTime = new DS_ElapsedTime     (this);
    m_manager     = new DS_ProtocolManager (this);
    m_protocol    = Q_NULLPTR;

    /* Update internal values and notify object on robot status events */
    connect (m_manager,     &DS_ProtocolManager::controlModeChanged,
             this,          &DriverStation::resetElapsedTimer);
    connect (m_manager,     &DS_ProtocolManager::codeChanged,
             this,          &DriverStation::codeChanged);
    connect (m_manager,     &DS_ProtocolManager::controlModeChanged,
             this,          &DriverStation::controlModeChanged);
    connect (m_manager,     &DS_ProtocolManager::diskUsageChanged,
             this,          &DriverStation::diskUsageChanged);
    connect (m_manager,     &DS_ProtocolManager::ramUsageChanged,
             this,          &DriverStation::ramUsageChanged);
    connect (m_manager,     &DS_ProtocolManager::cpuUsageChanged,
             this,          &DriverStation::cpuUsageChanged);
    connect (m_manager,     &DS_ProtocolManager::voltageChanged,
             this,          &DriverStation::voltageChanged);
    connect (m_manager,     &DS_ProtocolManager::voltageBrownoutChanged,
             this,          &DriverStation::voltageBrownoutChanged);
    connect (m_manager,     &DS_ProtocolManager::CANInfoReceived,
             this,          &DriverStation::CANInfoReceived);
    connect (m_manager,     &DS_ProtocolManager::emergencyStopped,
             this,          &DriverStation::emergencyStopped);
    connect (m_manager,     &DS_ProtocolManager::fmsChanged,
             this,          &DriverStation::fmsChanged);
    connect (m_manager,     &DS_ProtocolManager::communicationsChanged,
             m_elapsedTime, &DS_ElapsedTime::stopTimer);
    connect (m_manager,     &DS_ProtocolManager::libVersionChanged,
             this,          &DriverStation::libVersionChanged);
    connect (m_manager,     &DS_ProtocolManager::rioVersionChanged,
             this,          &DriverStation::rioVersionChanged);
    connect (m_manager,     &DS_ProtocolManager::pcmVersionChanged,
             this,          &DriverStation::pcmVersionChanged);
    connect (m_manager,     &DS_ProtocolManager::pdpVersionChanged,
             this,          &DriverStation::pdpVersionChanged);
    connect (m_manager,     &DS_ProtocolManager::robotAddressChanged,
             m_client,      &DS_Client::setRobotAddress);
    connect (m_elapsedTime, &DS_ElapsedTime::elapsedTimeChanged,
             this,          &DriverStation::elapsedTimeChanged);
    connect (m_netConsole,  &DS_NetConsole::newMessage,
             this,          &DriverStation::newMessage);
    connect (m_client,      &DS_Client::robotPacketReceived,
             this,          &DriverStation::readRobotPacket);
    connect (m_client,      &DS_Client::fmsPacketReceived,
             this,          &DriverStation::readFmsPacket);

    /* Lamda-functions */
    connect (m_manager, &DS_ProtocolManager::codeChanged,
             [ = ] (bool ignored)
    {
        Q_UNUSED (ignored);
        emit robotStatusChanged (getRobotStatus());
    });
    connect (m_manager, &DS_ProtocolManager::communicationsChanged,
             [ = ] (DS_CommStatus status)
    {
        emit communicationsChanged (status);
    });
    connect (m_manager, &DS_ProtocolManager::communicationsChanged,
             [ = ] (DS_CommStatus status)
    {
        emit communicationsChanged (status);
        emit robotStatusChanged (getRobotStatus());
    });
}

//=============================================================================
// DriverStation::getInstance
//=============================================================================

DriverStation* DriverStation::getInstance()
{
    static DriverStation instance;
    return &instance;
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

    list.append (tr ("Red 1"));
    list.append (tr ("Red 2"));
    list.append (tr ("Red 3"));
    list.append (tr ("Blue 1"));
    list.append (tr ("Blue 2"));
    list.append (tr ("Blue 3"));

    return list;
}

//=============================================================================
// DriverStation::protocols
//=============================================================================

QStringList DriverStation::protocols()
{
    QStringList list;
    list.append (tr ("2016 Protocol"));
    list.append (tr ("2015 Protocol"));
    list.append (tr ("2014 Protocol"));
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
    if (isConnected())
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
// DriverStation::team
//=============================================================================

int DriverStation::team()
{
    if (m_manager->isValid())
        return m_manager->currentProtocol()->team();

    return 0;
}

//=============================================================================
// DriverStation::acceptsConsoleCommands
//=============================================================================

bool DriverStation::acceptsConsoleCommands()
{
    if (m_manager->isValid())
        return m_manager->currentProtocol()->acceptsConsoleCommands();

    return false;
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
            QTimer::singleShot (500, this,   SLOT (resetEverything()));

            DS_LogMessage (kLibLevel, "DS Initialized");
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
// DriverStation::sendConsoleCommand
//=============================================================================

void DriverStation::sendCommand (QString command)
{
    m_netConsole->sendCommand (command);
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
            m_manager->setProtocol        (protocol);
            m_client->setFmsInputPort     (protocol->fmsInputPort());
            m_client->setFmsOutputPort    (protocol->fmsOutputPort());
            m_client->setRobotInputPort   (protocol->robotInputPort());
            m_client->setRobotOutputPort  (protocol->robotOutputPort());
            m_netConsole->setInputPort    (protocol->netConsoleInputPort());
            m_netConsole->setOutputPort   (protocol->netConsoleOutputPort());
            m_netConsole->setAcceptsInput (protocol->acceptsConsoleCommands());

            sendToFms();
            sendToRobot();

            emit protocolChanged();
        }
}

//=============================================================================
// DriverStation::setProtocol
//=============================================================================

void DriverStation::setProtocol (ProtocolType protocol)
{
    if (protocol == kProtocol2016)
        setProtocol (new DS_Protocol2016);

    if (protocol == kProtocol2015)
        setProtocol (new DS_Protocol2015);

    if (protocol == kProtocol2014)
        setProtocol (new DS_Protocol2014);
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

void DriverStation::updateJoystickAxis (int js, int axis, float value)
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
                return tr ("Emergency Stopped");

            if (!m_manager->currentProtocol()->hasCode())
                return tr ("No Robot Code");

            return QString ("%1 %2")
                   .arg (DS_GetControlModeString (controlMode()))
                   .arg (isEnabled() ? tr ("Enabled") : tr ("Disabled"));
        }

    return tr ("No Robot Communication");
}

//=============================================================================
// DriverStation::sendFMSPacket
//=============================================================================

void DriverStation::sendToFms()
{
    if (!m_manager->isValid())
        return;

    m_client->sendToFms (m_manager->currentProtocol()->createFmsPacket());
    QTimer::singleShot (1000 / m_manager->currentProtocol()->fmsFrequency(),
                        Qt::PreciseTimer,
                        this, SLOT (sendToFms()));
}

//=============================================================================
// DriverStation::sendClientPacket
//=============================================================================

void DriverStation::sendToRobot()
{
    if (!m_manager->isValid())
        return;

    m_client->sendToRobot (m_manager->currentProtocol()->createRobotPacket());
    QTimer::singleShot (1000 / m_manager->currentProtocol()->robotFrequency(),
                        Qt::PreciseTimer,
                        this, SLOT (sendToRobot()));
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

    DS_LogMessage (kLibLevel, "Reseting to initial values...");
}

//=============================================================================
// DriverStation::readFmsPacket
//=============================================================================

void DriverStation::readFmsPacket (QByteArray response)
{
    if (m_manager->isValid())
        m_manager->currentProtocol()->readFmsPacket (response);
}

//=============================================================================
// DriverStation::readRobotPacket
//=============================================================================

void DriverStation::readRobotPacket (QByteArray response)
{
    if (m_manager->isValid())
        m_manager->currentProtocol()->readRobotPacket (response);
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
