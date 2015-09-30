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

DriverStation* DriverStation::m_instance = Q_NULLPTR;

DriverStation::DriverStation() {
    m_init = false;

    checkConnection();
    sendRobotPackets();
    setProtocol (new DS_Protocol2015);

    connect (&m_manager,     SIGNAL (codeChanged (bool)),
             this,           SIGNAL (codeChanged (bool)));
    connect (&m_manager,     SIGNAL (controlModeChanged (DS_ControlMode)),
             this,           SIGNAL (controlModeChanged (DS_ControlMode)));
    connect (&m_manager,     SIGNAL (diskUsageChanged (int, int)),
             this,           SIGNAL (diskUsageChanged (int, int)));
    connect (&m_manager,     SIGNAL (ramUsageChanged (int, int)),
             this,           SIGNAL (ramUsageChanged (int, int)));
    connect (&m_manager,     SIGNAL (voltageChanged (double)),
             this,           SIGNAL (voltageChanged (double)));
    connect (&m_manager,     SIGNAL (libVersionChanged (QString)),
             this,           SIGNAL (libVersionChanged (QString)));
    connect (&m_manager,     SIGNAL (rioVersionChanged (QString)),
             this,           SIGNAL (rioVersionChanged (QString)));
    connect (&m_manager,     SIGNAL (pcmVersionChanged (QString)),
             this,           SIGNAL (pcmVersionChanged (QString)));
    connect (&m_manager,     SIGNAL (pdpVersionChanged (QString)),
             this,           SIGNAL (pdpVersionChanged (QString)));
    connect (&m_elapsedTime, SIGNAL (elapsedTimeChanged (QString)),
             this,           SIGNAL (elapsedTimeChanged (QString)));
    connect (&m_netConsole,  SIGNAL (newMessage (QString)),
             this,           SIGNAL (newNetConsoleMessage (QString)));

    connect (&m_manager,     SIGNAL (controlModeChanged (DS_ControlMode)),
             this,           SLOT   (onControlModeChanged (DS_ControlMode)));
}

DriverStation::~DriverStation() {
    delete m_instance;
}

DriverStation* DriverStation::getInstance() {
    if (m_instance == Q_NULLPTR)
        m_instance = new DriverStation();

    return m_instance;
}

bool DriverStation::canBeEnabled() {
    return m_manager.protocol()->robotCode() &&
           m_networkDiagnostics.robotIsAlive();
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

QString DriverStation::radioAddress() {
    return m_manager.protocol()->radioAddress();
}

QString DriverStation::robotAddress() {
    return m_manager.protocol()->robotAddress();
}

DS_ControlMode DriverStation::controlMode() {
    return m_manager.protocol()->controlMode();
}

bool DriverStation::robotHasCode() {
    return m_manager.protocol()->robotCode();
}

bool DriverStation::networkAvailable() {
    return m_networkDiagnostics.robotIsAlive();
}

void DriverStation::init() {
    if (!m_init) {
        m_init = true;
        resetInternalValues();
    }
}

void DriverStation::reboot() {
    if (m_init)
        m_manager.protocol()->reboot();
}

void DriverStation::restartCode() {
    if (m_init)
        m_manager.protocol()->restartCode();
}

void DriverStation::showLogWindow() {
    if (m_init)
        m_logWindow.show();
}

void DriverStation::startPractice (int countdown,
                                   int autonomous,
                                   int delay,
                                   int teleop,
                                   int endgame) {
    Q_UNUSED (countdown);
    Q_UNUSED (autonomous);
    Q_UNUSED (delay);
    Q_UNUSED (teleop);
    Q_UNUSED (endgame);

    DS_WARNING ("Function not implemented");
}

void DriverStation::setProtocol (DS_Protocol* protocol) {
    if (protocol != Q_NULLPTR)
        m_manager.setProtocol (protocol);

    else
        DS_ERROR ("Invalid protocol object");
}

void DriverStation::setTeamNumber (int team) {
    m_netConsole.setTeamNumber (team);
    m_manager.protocol()->setTeamNumber (team);
    m_client.setRobotAddress (m_manager.protocol()->robotAddress());
    m_networkDiagnostics.setRobotAddress (m_manager.protocol()->robotAddress());
    m_networkDiagnostics.setRadioAddress (m_manager.protocol()->radioAddress());
}

void DriverStation::setAlliance (DS_Alliance alliance) {
    if (m_init)
        m_manager.protocol()->setAlliance (alliance);
}

void DriverStation::setControlMode (DS_ControlMode mode) {
    if (m_init)
        m_manager.protocol()->setControlMode (mode);
}

void DriverStation::setCustomAddress (QString address) {
    m_networkDiagnostics.setRobotAddress (address);
    m_manager.protocol()->setRobotAddress (address);
}

void DriverStation::clearJoysticks() {
    m_manager.clearJoysticks();
}

void DriverStation::updateJoystickPovHat (int js, int hat, int angle) {
    if (m_init)
        m_manager.updateJoystickPovHat (js, hat, angle);
}

void DriverStation::addJoystick (int axes, int buttons, int povHats) {
    if (m_init)
        m_manager.addJoystick (axes, buttons, povHats);
}

void DriverStation::updateJoystickAxis (int js, int axis, double value) {
    if (m_init)
        m_manager.updateJoystickAxis (js, axis, value);
}

void DriverStation::updateJoystickButton (int js, int button, bool state) {
    if (m_init)
        m_manager.updateJoystickButton (js, button, state);
}

void DriverStation::resetInternalValues() {
    emit codeChanged (false);
    emit networkChanged (false);
    emit voltageChanged (QString());
    emit elapsedTimeChanged ("00:00.0");
}

void DriverStation::checkConnection() {
    if (m_init) {
        m_networkDiagnostics.refresh();
        m_manager.updateNetworkStatus (networkAvailable());
        emit networkChanged (networkAvailable());
    }

    QTimer::singleShot (1000, Qt::PreciseTimer, this, SLOT (checkConnection()));
}

void DriverStation::sendRobotPackets() {
    if (m_networkDiagnostics.robotIsAlive() && m_init)
        m_client.sendToRobot (m_manager.protocol()->generateClientPacket());

    emit robotStatusChanged (getStatus());
    QTimer::singleShot (20, Qt::PreciseTimer, this, SLOT (sendRobotPackets()));
}

QString DriverStation::getStatus() {
    if (!m_networkDiagnostics.robotIsAlive())
        return "No Robot Communication";

    else if (!m_manager.protocol()->robotCode())
        return "No Robot Code";

    return DS_GetControlModeString (m_manager.protocol()->controlMode());
}

void DriverStation::onControlModeChanged (DS_ControlMode mode) {
    if (mode == DS_ControlDisabled)
        m_elapsedTime.stop();

    else
        m_elapsedTime.reset();
}
