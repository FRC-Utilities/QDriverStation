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

#include <DriverStation.h>

DriverStation* DriverStation::m_instance = nullptr;

DriverStation::DriverStation()
{
    m_team = 0;
    m_code = false;
    m_init = false;
    m_oldConnection = false;
    m_justConnected = false;
    m_justDisconnected = false;

    m_status = DS_Normal;
    m_alliance = DS_Red1;
    m_mode = DS_Disabled;

    connect (&m_netConsole,      SIGNAL (newMessage (QString)),
             this,               SIGNAL (newMessage (QString)));
    connect (&m_receiver,        SIGNAL (voltageChanged (QString)),
             this,               SIGNAL (voltageChanged (QString)));
    connect (&m_versionAnalyzer, SIGNAL (libVersionChanged (QString)),
             this,               SIGNAL (libVersionChanged (QString)));
    connect (&m_versionAnalyzer, SIGNAL (pcmVersionChanged (QString)),
             this,               SIGNAL (pcmVersionChanged (QString)));
    connect (&m_versionAnalyzer, SIGNAL (pdpVersionChanged (QString)),
             this,               SIGNAL (pdpVersionChanged (QString)));
    connect (&m_versionAnalyzer, SIGNAL (rioVersionChanged (QString)),
             this,               SIGNAL (rioVersionChanged (QString)));
    connect (&m_elapsedTime,     SIGNAL (elapsedTimeChanged (QString)),
             this,               SIGNAL (elapsedTimeChanged (QString)));
    //connect (&m_receiver,        SIGNAL (dataReceived (int)),
    //       this,               SLOT   (sendPacketsToRobot (int)));
}

DriverStation* DriverStation::getInstance()
{
    if (m_instance == nullptr)
        m_instance = new DriverStation();

    return m_instance;
}

bool DriverStation::canBeEnabled()
{
    return m_netDiagnostics.roboRioIsAlive() && m_code;
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

DS_ControlMode DriverStation::operationMode()
{
    return m_mode;
}

QString DriverStation::roboRioAddress()
{
    return m_netDiagnostics.roboRioIpAddress();
}

QString DriverStation::radioAddress()
{
    return m_netDiagnostics.radioIpAddress();
}

DS_ControlMode DriverStation::controlMode()
{
    return m_mode;
}

void DriverStation::init()
{
    if (!m_init) {
        /* Send signals to force object update */
        emit codeChanged (false);
        emit networkChanged (false);
        emit voltageChanged ("");
        emit libVersionChanged ("");
        emit rioVersionChanged ("");
        emit pdpVersionChanged ("");
        emit ramUsageChanged (0, 0);
        emit diskUsageChanged (0, 0);

        /* Begin internal loops */
        checkConnection();
        sendRobotPacketsLoop();

        /* Configure elapsed time */
        m_elapsedTime.stop();
        emit elapsedTimeChanged ("00:00.0");

        /* Ensure that this code will only run once */
        m_init = true;
    }
}

void DriverStation::reboot()
{
    m_status = DS_RebootRobot;
}

void DriverStation::restartCode()
{
    m_status = DS_RestartCode;
}

void DriverStation::setTeamNumber (int team)
{
    if (m_team != team) {
        m_team = team;
        m_netConsole.setTeamNumber (m_team);
        m_netDiagnostics.setTeamNumber (m_team);
    }
}

void DriverStation::setAlliance (DS_Alliance alliance)
{
    m_alliance = alliance;
}

void DriverStation::setCustomAddress (QString address)
{
    m_netDiagnostics.setCustomAddress (address);
}

void DriverStation::setControlMode (DS_ControlMode mode)
{
    if (mode == DS_Disabled)
        m_elapsedTime.stop();

    else if (m_mode != mode)
        m_elapsedTime.reset();

    m_mode = mode;
    emit controlModeChanged (m_mode);
}

void DriverStation::putJoystickData (DS_JoystickData joystickData)
{
    m_joystickData = joystickData;
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

QString DriverStation::getStatus()
{
    if (!m_netDiagnostics.roboRioIsAlive())
        return tr ("No Robot Communication");

    else if (!m_code)
        return tr ("No Robot Code");

    return DS_GetControlModeString (m_mode);
}

void DriverStation::checkConnection()
{
    m_netDiagnostics.refresh();
    m_justConnected = m_netDiagnostics.roboRioIsAlive() && !m_oldConnection;
    m_justDisconnected = !m_netDiagnostics.roboRioIsAlive() && m_oldConnection;

    if (m_justDisconnected) {
        resetInternalValues();
        QTimer::singleShot (DS_Times::SafetyDisableTimeout, Qt::PreciseTimer,
                            this, SLOT (safetyDisableCheck()));
    }

    else if (m_justConnected) {
        m_code = true;
        emit codeChanged (m_code);
        emit networkChanged (m_netDiagnostics.roboRioIsAlive());
        m_versionAnalyzer.downloadRobotInformation (roboRioAddress());
    }

    if (m_radioStatus != m_netDiagnostics.robotRadioIsAlive()) {
        m_radioStatus = m_netDiagnostics.robotRadioIsAlive();
        emit radioChanged (m_radioStatus);
    }

    m_oldConnection = m_netDiagnostics.roboRioIsAlive();

    QTimer::singleShot (DS_Times::TestConnectionInterval, Qt::PreciseTimer,
                        this, SLOT (checkConnection()));
}

void DriverStation::resetInternalValues()
{
    m_count = 0;
    m_code = false;
    m_status = DS_Normal;

    emit codeChanged (false);
    emit networkChanged (false);
    emit voltageChanged (QString (""));

    setControlMode (DS_Disabled);
}

void DriverStation::safetyDisableCheck()
{
    if (!m_netDiagnostics.roboRioIsAlive())
        setControlMode (DS_Disabled);
}

void DriverStation::sendRobotPacketsLoop()
{
    if (m_netDiagnostics.roboRioIsAlive()) {
        ++m_count;
        sendPacketsToRobot (m_count);
    }

    emit robotStatusChanged (getStatus());

    QTimer::singleShot (DS_Times::RobotPacketInterval, Qt::PreciseTimer,
                        this, SLOT (sendRobotPacketsLoop()));
}

void DriverStation::sendPacketsToRobot (int ping)
{
    if (ping != m_count)
        m_count = ping;

    if (m_netDiagnostics.roboRioIsAlive()) {
        DS_ControlPacket packet;
        packet.mode = m_mode;
        packet.status = m_status;
        packet.alliance = m_alliance;

        m_sender.send (ping, roboRioAddress(), m_joystickData, packet);
    }
}
