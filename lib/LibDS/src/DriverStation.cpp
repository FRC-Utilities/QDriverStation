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

#include "DriverStation.h"

#include "LibDS/DS_Timers.h"
#include "LibDS/DS_Client.h"
#include "LibDS/DS_NetConsole.h"
#include "LibDS/DS_ElapsedTime.h"
#include "LibDS/DS_ProtocolManager.h"

#include "Protocols/DS_Protocol2015.h"

#include <QDebug>

DriverStation* DriverStation::s_instance = Q_NULLPTR;

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
    connect (m_manager, SIGNAL (newMessage            (QString)),
             this,      SLOT   (writeMessage          (QString)));

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
    connect (m_netConsole,  SIGNAL (newMessage   (QString)),
             this,          SLOT   (writeMessage (QString)));

    /* Send and read robot packets */
    connect (m_client,  SIGNAL (dataReceived     (QByteArray)),
             this,      SLOT   (readRobotPackets (QByteArray)));
    connect (DS_Timers::getInstance(), SIGNAL (timeout20()),
             this,                     SLOT   (sendRobotPackets()));
}

DriverStation::~DriverStation()
{
    delete m_client;
    delete m_manager;
    delete m_protocol;
    delete m_netConsole;
    delete m_elapsedTime;

    delete s_instance;
}

DriverStation* DriverStation::getInstance()
{
    if (s_instance == Q_NULLPTR)
        s_instance = new DriverStation();

    return s_instance;
}

bool DriverStation::canBeEnabled()
{
    if (m_manager->protocolIsValid()) {
        return m_manager->protocol()->robotCode() &&
               m_manager->protocol()->robotCommunication() &&
               m_manager->protocol()->controlMode() != DS_ControlEmergencyStop;
    }

    return false;
}

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

QStringList DriverStation::protocols()
{
    QStringList list;
    list.append ("2015 Protocol");
    return list;
}

QString DriverStation::radioAddress()
{
    if (m_manager->protocolIsValid())
        return m_manager->protocol()->radioAddress();

    return "";
}

QString DriverStation::robotAddress()
{
    if (m_manager->protocolIsValid())
        return m_manager->protocol()->robotAddress();

    return "";
}

DS_ControlMode DriverStation::controlMode()
{
    if (m_manager->protocolIsValid())
        return m_manager->protocol()->controlMode();

    return DS_ControlNoCommunication;
}

bool DriverStation::robotHasCode()
{
    if (m_manager->protocolIsValid())
        return m_manager->protocol()->robotCode();

    return false;
}

bool DriverStation::networkAvailable()
{
    if (m_manager->protocolIsValid())
        return m_manager->protocol()->robotCommunication();

    return false;
}

void DriverStation::init()
{
    if (!m_init) {
        m_init = true;
        resetInternalValues();
        DS_Timers::getInstance()->start();
    }
}

void DriverStation::reboot()
{
    if (m_manager->protocolIsValid() && m_init)
        m_manager->protocol()->reboot();
}

void DriverStation::restartCode()
{
    if (m_manager->protocolIsValid() && m_init)
        m_manager->protocol()->restartCode();
}

void DriverStation::startPractice (int countdown,
                                   int autonomous,
                                   int delay,
                                   int teleop,
                                   int endgame)
{
    Q_UNUSED (countdown);
    Q_UNUSED (autonomous);
    Q_UNUSED (delay);
    Q_UNUSED (teleop);
    Q_UNUSED (endgame);
}

void DriverStation::setProtocol (DS_ProtocolBase* protocol)
{
    if (protocol != Q_NULLPTR) {
        m_manager->setProtocol (protocol);
        m_client->setRobotPort (protocol->robotPort());
        m_client->setClientPort (protocol->clientPort());
    }
}

void DriverStation::setProtocol (ProtocolType protocol)
{
    if (protocol == Protocol2015) {
        m_protocol = new DS_Protocol2015;
        m_manager->setProtocol (m_protocol);
    }
}

void DriverStation::setTeamNumber (int team)
{
    if (m_manager->protocolIsValid() && m_init)
        m_manager->protocol()->setTeamNumber (team);
}

void DriverStation::writeMessage (QString message)
{
    emit newMessage (QString ("<font color='#888'>[DS] %1</font>").arg (message));
}

void DriverStation::setAlliance (AllianceType allianceType)
{
    DS_Alliance alliance;

    switch (allianceType) {
    case Red1:
        alliance = DS_AllianceRed1;
        break;
    case Red2:
        alliance = DS_AllianceRed2;
        break;
    case Red3:
        alliance = DS_AllianceRed3;
        break;
    case Blue1:
        alliance = DS_AllianceBlue1;
        break;
    case Blue2:
        alliance = DS_AllianceBlue2;
        break;
    case Blue3:
        alliance = DS_AllianceBlue3;
        break;
    default:
        alliance = DS_AllianceRed1;
        break;
    }

    if (m_manager->protocolIsValid() && m_init)
        m_manager->protocol()->setAlliance (alliance);
}

void DriverStation::setControlMode (ControlMode mode)
{
    switch (mode) {
    case Teleoperated:
        setControlMode (DS_ControlTeleOp);
        break;
    case Autonomous:
        setControlMode (DS_ControlAutonomous);
        break;
    case Test:
        setControlMode (DS_ControlTest);
        break;
    case EmergencyStop:
        setControlMode (DS_ControlEmergencyStop);
        break;
    default:
        setControlMode (DS_ControlDisabled);
        break;
    }
}

void DriverStation::setControlMode (DS_ControlMode mode)
{
    if (m_manager->protocolIsValid() && m_init)
        m_manager->protocol()->setControlMode (mode);
}

void DriverStation::setCustomAddress (QString address)
{
    if (m_manager->protocolIsValid())
        m_manager->protocol()->setRobotAddress (address);
}

void DriverStation::clearJoysticks()
{
    m_manager->clearJoysticks();
}

void DriverStation::updateJoystickPovHat (int js, int hat, int angle)
{
    if (m_manager->protocolIsValid() && m_init)
        m_manager->updateJoystickPovHat (js, hat, angle);
}

void DriverStation::addJoystick (int axes, int buttons, int povHats)
{
    if (m_manager->protocolIsValid() && m_init)
        m_manager->addJoystick (axes, buttons, povHats);
}

void DriverStation::updateJoystickAxis (int js, int axis, double value)
{
    if (m_manager->protocolIsValid() && m_init)
        m_manager->updateJoystickAxis (js, axis, value);
}

void DriverStation::updateJoystickButton (int js, int button, bool state)
{
    if (m_manager->protocolIsValid() && m_init)
        m_manager->updateJoystickButton (js, button, state);
}

void DriverStation::sendRobotPackets()
{
    if (m_manager->protocolIsValid() && m_init)
        m_client->sendToRobot (m_manager->protocol()->getClientPacket());
}

void DriverStation::resetInternalValues()
{
    m_elapsedTime->reset();
    m_elapsedTime->stop();

    emit codeChanged (false);
    emit communicationsChanged (false);
    emit voltageChanged (QString (""));
    emit elapsedTimeChanged ("00:00.0");
}

void DriverStation::readRobotPackets (QByteArray robotResponse)
{
    if (m_manager->protocolIsValid() && m_init)
        m_manager->protocol()->readRobotPacket (robotResponse);
}

QString DriverStation::getStatus()
{
    if (m_manager->protocolIsValid() && m_init) {
        if (m_manager->protocol()->robotCommunication()
            && !m_manager->protocol()->robotCode())
            return "No Robot Code";

        return DS_GetControlModeString (m_manager->protocol()->controlMode());
    }

    return DS_GetControlModeString (DS_ControlNoCommunication);
}

void DriverStation::updateStatus (bool ignored)
{
    Q_UNUSED (ignored);
    emit robotStatusChanged (getStatus());
}

void DriverStation::onControlModeChanged (DS_ControlMode mode)
{
    if (!canBeEnabled() || mode == DS_ControlDisabled)
        m_elapsedTime->stop();

    else
        m_elapsedTime->reset();

    emit robotStatusChanged (getStatus());
}
