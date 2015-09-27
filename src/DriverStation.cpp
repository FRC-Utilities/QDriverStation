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

DriverStation* DriverStation::m_instance = Q_NULLPTR;

DriverStation::DriverStation()
{
    m_team = 0;
    m_count = 0;
    m_code = false;
    m_enabled = false;
    m_oldConnection = false;
    m_justConnected = false;
    m_justDisconnected = false;

    m_status = DS_StatusNormal;
    m_mode = DS_ControlDisabled;
    m_alliance = DS_AllianceRed1;

    m_logWindow.hide();

    connect (&m_netConsole,      SIGNAL (newMessage           (QString)),
             this,               SIGNAL (newMessage           (QString)));
    connect (&m_versionAnalyzer, SIGNAL (libVersionChanged    (QString)),
             this,               SIGNAL (libVersionChanged    (QString)));
    connect (&m_versionAnalyzer, SIGNAL (pcmVersionChanged    (QString)),
             this,               SIGNAL (pcmVersionChanged    (QString)));
    connect (&m_versionAnalyzer, SIGNAL (pdpVersionChanged    (QString)),
             this,               SIGNAL (pdpVersionChanged    (QString)));
    connect (&m_versionAnalyzer, SIGNAL (rioVersionChanged    (QString)),
             this,               SIGNAL (rioVersionChanged    (QString)));
    connect (&m_elapsedTime,     SIGNAL (elapsedTimeChanged   (QString)),
             this,               SIGNAL (elapsedTimeChanged   (QString)));
    connect (&m_receiver,        SIGNAL (voltageChanged       (QString)),
             this,               SIGNAL (voltageChanged       (QString)));
    connect (&m_receiver,        SIGNAL (voltageChanged       (double)),
             this,               SIGNAL (voltageChanged       (double)));
    connect (&m_receiver,        SIGNAL (userCodeChanged      (bool)),
             this,               SLOT   (onCodeChanged        (bool)));
    connect (&m_receiver,        SIGNAL (controlModeChanged   (DS_ControlMode)),
             this,               SLOT   (onControlModeChanged (DS_ControlMode)));

    connect (this, SIGNAL (codeChanged    (bool)),
             this, SLOT   (updateStatus   (bool)));
    connect (this, SIGNAL (networkChanged (bool)),
             this, SLOT   (updateStatus   (bool)));
    connect (this, SIGNAL (controlModeChanged (DS_ControlMode)),
             this, SLOT   (updateStatus       (DS_ControlMode)));

    connect (qApp, SIGNAL (aboutToQuit()), this, SLOT (stopDriverStation()));
}

DriverStation::~DriverStation()
{
    delete m_instance;
}

DriverStation* DriverStation::getInstance()
{
    if (m_instance == Q_NULLPTR)
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

bool DriverStation::robotCodeLoaded()
{
    return m_code;
}

bool DriverStation::networkAvailable()
{
    return m_netDiagnostics.roboRioIsAlive();
}

void DriverStation::init()
{
    if (!m_enabled) {
        m_enabled = true;
        m_netDiagnostics.setEnabled (true);

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
        sendRobotPackets();

        /* Configure elapsed time */
        m_elapsedTime.stop();
        emit elapsedTimeChanged ("00:00.0");
    }
}

void DriverStation::reboot()
{
    m_status = DS_StatusRebootRobot;
}

void DriverStation::restartCode()
{
    m_status = DS_StatusRestartCode;
}

void DriverStation::showLogWindow()
{
    m_logWindow.show();
}

void DriverStation::hideLogWindow()
{
    m_logWindow.hide();
}

void DriverStation::setTeamNumber (const int& team)
{
    if (m_team != team) {
        m_team = team;
        m_netConsole.setTeamNumber (m_team);
        m_netDiagnostics.setTeamNumber (m_team);
    }
}

void DriverStation::setAlliance (const DS_Alliance& alliance)
{
    m_alliance = alliance;
}

void DriverStation::setCustomAddress (const QString& address)
{
    m_netDiagnostics.setCustomAddress (address);
}

void DriverStation::setControlMode (const DS_ControlMode& mode)
{
    if (mode == DS_ControlDisabled)
        m_elapsedTime.stop();

    else if (m_mode != mode)
        m_elapsedTime.reset();

    m_mode = mode;

    emit controlModeChanged (m_mode);
}

void DriverStation::removeAllJoysticks()
{
    m_joystickManager.removeAllJoysticks();
}

void DriverStation::removeJoystick (const short& joystick)
{
    m_joystickManager.removeJoystick (joystick);
}

void DriverStation::addJoystick (const short& axes,
                                 const short& buttons,
                                 const short& hats)
{
    m_joystickManager.addJoystick (axes, buttons, hats);
}

void DriverStation::updateJoystickHat (const short& js,
                                       const short& hat,
                                       const int& angle)
{
    m_joystickManager.updateHat (js, hat, angle);
}

void DriverStation::updateJoystickAxis (const short& js,
                                        const short& axis,
                                        const double& value)
{
    m_joystickManager.updateAxis (js, axis, value);
}

void DriverStation::updateJoystickButton (const short& js,
        const short& button,
        const bool& pressed)
{
    m_joystickManager.updateButton (js, button, pressed);
}

void DriverStation::startPractice (const int& countdown,
                                   const int& autonomous,
                                   const int& delay,
                                   const int& teleop,
                                   const int& endgame)
{
    Q_UNUSED (countdown);
    Q_UNUSED (autonomous);
    Q_UNUSED (delay);
    Q_UNUSED (teleop);
    Q_UNUSED (endgame);
}

void DriverStation::stopDriverStation()
{
    m_enabled = false;
    m_netDiagnostics.setEnabled (false);

    resetInternalValues();
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
    if (!m_enabled)
        return;

    m_netDiagnostics.refresh();
    m_justConnected = m_netDiagnostics.roboRioIsAlive() && !m_oldConnection;
    m_justDisconnected = !m_netDiagnostics.roboRioIsAlive() && m_oldConnection;

    if (m_justDisconnected)
        resetInternalValues();

    else if (m_justConnected) {
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
    m_status = DS_StatusNormal;

    emit codeChanged (false);
    emit networkChanged (false);
    emit voltageChanged (QString (""));

    setControlMode (DS_ControlDisabled);
}

void DriverStation::sendRobotPackets()
{
    if (m_netDiagnostics.roboRioIsAlive()) {
        /* Increment the ping data */
        ++m_count;
        if (m_count > 0xffff)
            m_count = 0;

        /* Send the packet */
        m_sender.send (m_count,
                       roboRioAddress(),
                       m_status,
                       m_mode,
                       m_alliance,
                       m_joystickManager.getData());

    }

    QTimer::singleShot (DS_Times::RobotPacketInterval, Qt::PreciseTimer,
                        this, SLOT (sendRobotPackets()));
}

void DriverStation::onCodeChanged (const bool& available)
{
    if (m_code != available) {
        m_code = available;
        emit codeChanged (m_code);
    }
}

void DriverStation::onControlModeChanged (const DS_ControlMode& mode)
{
    m_mode = mode;
    emit controlModeChanged (m_mode);
}

void DriverStation::updateStatus (const bool& boolean)
{
    Q_UNUSED (boolean);
    emit robotStatusChanged (getStatus());
}

void DriverStation::updateStatus (const DS_ControlMode& mode)
{
    Q_UNUSED (mode);
    emit robotStatusChanged (getStatus());
}
