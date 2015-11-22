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

#include "LibDS/Core/Timers.h"
#include "LibDS/Core/Client.h"
#include "LibDS/Core/NetConsole.h"
#include "LibDS/Core/ElapsedTimer.h"
#include "LibDS/Core/ProtocolManager.h"
#include "LibDS/Protocols/Protocol2015.h"

#include "LibDS/DriverStation.h"

DriverStation* DriverStation::s_instance = Q_NULLPTR;

DriverStation::DriverStation() {
    m_init = false;

    /* Initialize private members */
    m_client      = new DS_Client;
    m_netConsole  = new DS_NetConsole;
    m_elapsedTime = new DS_ElapsedTime;
    m_manager     = new DS_ProtocolManager;
    m_protocol    = Q_NULLPTR;

    /* Update internal values and notify object on robot status events */
    connect (m_manager, SIGNAL (codeChanged           (bool)),
             this,      SLOT   (updateStatus          (bool)));
    connect (m_manager, SIGNAL (communicationsChanged (bool)),
             this,      SLOT   (updateStatus          (bool)));
    connect (m_manager, SIGNAL (controlModeChanged    (DS_ControlMode)),
             this,      SLOT   (onControlModeChanged  (DS_ControlMode)));
    connect (m_manager, SIGNAL (codeChanged           (bool)),
             this,      SIGNAL (codeChanged           (bool)));
    connect (m_manager, SIGNAL (communicationsChanged (bool)),
             this,      SIGNAL (communicationsChanged (bool)));
    connect (m_manager, SIGNAL (controlModeChanged    (DS_ControlMode)),
             this,      SIGNAL (controlModeChanged    (DS_ControlMode)));
    connect (m_manager, SIGNAL (diskUsageChanged      (int)),
             this,      SIGNAL (diskUsageChanged      (int)));
    connect (m_manager, SIGNAL (ramUsageChanged       (int)),
             this,      SIGNAL (ramUsageChanged       (int)));
    connect (m_manager, SIGNAL (voltageChanged        (QString)),
             this,      SIGNAL (voltageChanged        (QString)));

    /* Stop timer when the communications status changes */
    connect (m_manager,     SIGNAL (communicationsChanged (bool)),
             m_elapsedTime, SLOT   (stop()));

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
             this,      SLOT   (readRobotPackets (QByteArray)));
    connect (DS_Timers::getInstance(), SIGNAL (timeout20()),
             this,                     SLOT   (sendRobotPackets()));
}

DriverStation::~DriverStation() {
    delete m_client;
    delete m_manager;
    delete m_protocol;
    delete m_netConsole;
    delete m_elapsedTime;

    delete s_instance;
}

DriverStation* DriverStation::getInstance() {
    if (s_instance == Q_NULLPTR)
        s_instance = new DriverStation();

    return s_instance;
}

bool DriverStation::canBeEnabled() {
    return robotHasCode() && isConnected() && !isEmergencyStop();
}

QStringList DriverStation::alliances() {
    QStringList list;

    list.append ("Red 1");
    list.append ("Red 2");
    list.append ("Red 3");
    list.append ("Blue 1");
    list.append ("Blue 2");
    list.append ("Blue 3");

    return list;
}

QStringList DriverStation::protocols() {
    QStringList list;
    list.append ("2015 Protocol");
    return list;
}

QString DriverStation::radioAddress() {
    if (m_manager->protocolIsValid())
        return m_manager->protocol()->radioAddress();

    return "";
}

QString DriverStation::robotAddress() {
    if (m_manager->protocolIsValid())
        return m_manager->protocol()->robotAddress();

    return "";
}

DS_ControlMode DriverStation::controlMode() {
    if (m_manager->protocolIsValid())
        return m_manager->protocol()->controlMode();

    return kControlNoCommunication;
}

bool DriverStation::robotHasCode() {
    if (m_manager->protocolIsValid())
        return m_manager->protocol()->robotCode();

    return false;
}

bool DriverStation::isTest() {
    return controlMode() == kControlTest;
}

bool DriverStation::isDisabled() {
    return controlMode() == kControlDisabled;
}

bool DriverStation::isAutonomous() {
    return controlMode() == kControlAutonomous;
}

bool DriverStation::isTeleoperated() {
    return controlMode() == kControlTeleoperated;
}

bool DriverStation::isEmergencyStop() {
    return controlMode() == kControlEmergencyStop;
}

bool DriverStation::isConnected() {
    if (m_manager->protocolIsValid())
        return m_manager->protocol()->isConnected();

    return false;
}

void DriverStation::init() {
    if (!m_init) {
        m_init = true;

        QTimer::singleShot (1500, this, SLOT (resetInternalValues()));
        QTimer::singleShot (1500, DS_Timers::getInstance(), SLOT (start()));
    }
}

void DriverStation::reboot() {
    if (m_manager->protocolIsValid())
        m_manager->protocol()->reboot();
}

void DriverStation::restartCode() {
    if (m_manager->protocolIsValid())
        m_manager->protocol()->restartCode();
}

void DriverStation::startPractice (int countdown,
                                   int autonomous,
                                   int delay,
                                   int teleop,
                                   int endgame) {
    /* Transform the times from seconds to milliseconds */
    delay *= 1000;
    teleop *= 1000;
    endgame *= 1000;
    countdown *= 1000;
    autonomous *= 1000;

    /* Calculate the time frames */
    int _startAutonomous = countdown;
    int _startTeleoperated = _startAutonomous + delay + autonomous;
    int _startEndGame = _startTeleoperated + delay + teleop - endgame;
    int _stop = _startEndGame + endgame;

    /* Configure the sound/media timers */
    QTimer::singleShot (_startAutonomous, Qt::PreciseTimer,
                        this, SLOT (playAutonomousStart()));
    QTimer::singleShot (_startTeleoperated, Qt::PreciseTimer,
                        this, SLOT (playTeleopStart()));
    QTimer::singleShot (_stop, Qt::PreciseTimer,
                        this, SLOT (playMatchEnd()));
    QTimer::singleShot (_startEndGame, Qt::PreciseTimer,
                        this,
                        SLOT (playEndGame()));

    /* Configure the control mode timers */
    QTimer::singleShot (_startTeleoperated, Qt::PreciseTimer,
                        this, SLOT (startTeleoperated()));
    QTimer::singleShot (_startAutonomous, Qt::PreciseTimer,
                        this, SLOT (startAutonomous()));
    QTimer::singleShot (_stop, Qt::PreciseTimer,
                        this, SLOT (startDisabled()));
}

void DriverStation::startTest() {
    setControlMode (kControlTest);
}

void DriverStation::startDisabled() {
    setControlMode (kControlDisabled);
}

void DriverStation::startAutonomous() {
    setControlMode (kControlAutonomous);
}

void DriverStation::startTeleoperated() {
    setControlMode (kControlTeleoperated);
}

void DriverStation::startEmergencyStop() {
    setControlMode (kControlEmergencyStop);
}

void DriverStation::setProtocol (DS_ProtocolBase* protocol) {
    if (protocol != Q_NULLPTR) {
        m_manager->setProtocol (protocol);
        m_client->setRobotPort (protocol->robotPort());
        m_client->setClientPort (protocol->clientPort());
    }
}

void DriverStation::setProtocol (ProtocolType protocol) {
    if (protocol == kProtocol2015) {
        m_protocol = new DS_Protocol2015;
        setProtocol (m_protocol);
    }
}

void DriverStation::setTeamNumber (int team) {
    if (m_manager->protocolIsValid())
        m_manager->protocol()->setTeamNumber (team);
}

void DriverStation::setAlliance (DS_Alliance alliance) {
    if (m_manager->protocolIsValid())
        m_manager->protocol()->setAlliance (alliance);
}

void DriverStation::setControlMode (DS_ControlMode mode) {
    if (m_manager->protocolIsValid()) {
        emit controlModeChanged (mode);
        m_manager->protocol()->setControlMode (mode);
    }
}

void DriverStation::setCustomAddress (QString address) {
    if (m_manager->protocolIsValid())
        m_manager->protocol()->setRobotAddress (address);
}

void DriverStation::clearJoysticks() {
    m_manager->clearJoysticks();
}

void DriverStation::updateJoystickPovHat (int js,
        int hat,
        int angle) {
    if (m_manager->protocolIsValid())
        m_manager->updateJoystickPovHat (js, hat, angle);
}

void DriverStation::addJoystick (int axes,
                                 int buttons,
                                 int povHats) {
    if (m_manager->protocolIsValid())
        m_manager->addJoystick (axes, buttons, povHats);
}

void DriverStation::updateJoystickAxis (int js,
                                        int axis,
                                        double value) {
    if (m_manager->protocolIsValid())
        m_manager->updateJoystickAxis (js, axis, value);
}

void DriverStation::updateJoystickButton (int js,
        int button,
        bool state) {
    if (m_manager->protocolIsValid())
        m_manager->updateJoystickButton (js, button, state);
}

QString DriverStation::getStatus() {
    if (m_manager->protocolIsValid()) {
        if (m_manager->protocol()->isConnected()
                && !m_manager->protocol()->robotCode())
            return "No Robot Code";

        return DS_GetControlModeString (m_manager->protocol()->controlMode());
    }

    return DS_GetControlModeString (kControlNoCommunication);
}

void DriverStation::sendRobotPackets() {
    if (m_manager->protocolIsValid())
        m_client->sendToRobot (m_manager->protocol()->getClientPacket());
}

void DriverStation::resetInternalValues() {
    m_elapsedTime->reset();
    m_elapsedTime->stop();

    emit codeChanged (false);
    emit communicationsChanged (false);
    emit voltageChanged (QString (""));
    emit elapsedTimeChanged ("00:00.0");
}

void DriverStation::readRobotPackets (QByteArray robotResponse) {
    if (m_manager->protocolIsValid())
        m_manager->protocol()->readRobotPacket (robotResponse);
}

void DriverStation::updateStatus (bool ignored) {
    Q_UNUSED (ignored);
    emit robotStatusChanged (getStatus());
}

void DriverStation::onControlModeChanged (DS_ControlMode mode) {
    if (mode == kControlEmergencyStop
            || mode == kControlDisabled
            || mode == kControlNoCommunication)
        m_elapsedTime->stop();

    else
        m_elapsedTime->reset();

    emit robotStatusChanged (getStatus());
}

void DriverStation::playEndGame() {
    QSound::play (":/LibDS/Start of End Game_normalized.wav");
}

void DriverStation::playMatchEnd() {
    QSound::play (":/LibDS/Match End_normalized.wav");
}

void DriverStation::playTeleopStart() {
    QSound::play (":/LibDS/Start Teleop_normalized.wav");
}

void DriverStation::playAutonomousStart() {
    QSound::play (":/LibDS/Start Auto_normalized.wav");
}
