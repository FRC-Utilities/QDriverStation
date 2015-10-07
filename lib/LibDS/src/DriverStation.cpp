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

DriverStation* DriverStation::m_instance = Q_NULLPTR;

DriverStation::DriverStation() {
    m_init = false;

    sendRobotPackets();
    setProtocol (&m_protocol);

    connect (&m_manager,     SIGNAL (codeChanged (bool)),
             this,           SIGNAL (codeChanged (bool)));
    connect (&m_manager,     SIGNAL (communicationsChanged (bool)),
             this,           SIGNAL (communicationsChanged (bool)));
    connect (&m_manager,     SIGNAL (controlModeChanged (DS_ControlMode)),
             this,           SIGNAL (controlModeChanged (DS_ControlMode)));
    connect (&m_manager,     SIGNAL (diskUsageChanged (int, int)),
             this,           SIGNAL (diskUsageChanged (int, int)));
    connect (&m_manager,     SIGNAL (ramUsageChanged (int, int)),
             this,           SIGNAL (ramUsageChanged (int, int)));
    connect (&m_manager,     SIGNAL (voltageChanged (QString)),
             this,           SIGNAL (voltageChanged (QString)));
    connect (&m_manager,     SIGNAL (libVersionChanged (QString)),
             this,           SIGNAL (libVersionChanged (QString)));
    connect (&m_manager,     SIGNAL (rioVersionChanged (QString)),
             this,           SIGNAL (rioVersionChanged (QString)));
    connect (&m_manager,     SIGNAL (pcmVersionChanged (QString)),
             this,           SIGNAL (pcmVersionChanged (QString)));
    connect (&m_manager,     SIGNAL (pdpVersionChanged (QString)),
             this,           SIGNAL (pdpVersionChanged (QString)));

    connect (&m_manager,     SIGNAL (codeChanged           (bool)),
             this,           SLOT   (updateStatus          (bool)));
    connect (&m_manager,     SIGNAL (communicationsChanged (bool)),
             this,           SLOT   (updateStatus          (bool)));
    connect (&m_manager,     SIGNAL (controlModeChanged    (DS_ControlMode)),
             this,           SLOT   (onControlModeChanged  (DS_ControlMode)));

    connect (&m_manager,     SIGNAL (robotAddressChanged (QString)),
             &m_client,      SLOT   (setRobotAddress     (QString)));

    connect (&m_elapsedTime, SIGNAL (elapsedTimeChanged (QString)),
             this,           SIGNAL (elapsedTimeChanged (QString)));
    connect (&m_netConsole,  SIGNAL (newMessage (QString)),
             this,           SIGNAL (newNetConsoleMessage (QString)));
    connect (&m_client,      SIGNAL (dataReceived  (QByteArray)),
             &m_manager,     SLOT   (readRobotData (QByteArray)));

    connect (DS_Times::getInstance(), SIGNAL (timeout20()),
             this,                    SLOT   (sendRobotPackets()));
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
           m_manager.protocol()->robotCommunication();
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
    return m_manager.protocol()->robotCommunication();
}

void DriverStation::init() {
    if (!m_init) {
        m_init = true;
        resetInternalValues();
        DS_Times::getInstance()->start();
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

void DriverStation::setGraphPalette (QPalette palette) {
    m_logWindow.setGraphPalette (palette);
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
}

void DriverStation::setProtocol (DS_Protocol* protocol) {
    if (protocol != Q_NULLPTR) {
        m_manager.setProtocol (protocol);
        m_client.setRobotPort (protocol->robotPort());
        m_client.setClientPort (protocol->clientPort());
    }
}

void DriverStation::setTeamNumber (int team) {
    m_netConsole.setTeamNumber (team);
    m_manager.protocol()->setTeamNumber (team);
}

void DriverStation::setAlliance (DS_Alliance alliance) {
    if (m_init) m_manager.protocol()->setAlliance (alliance);
}

void DriverStation::setControlMode (DS_ControlMode mode) {
    if (m_init) m_manager.protocol()->setControlMode (mode);
}

void DriverStation::setCustomAddress (QString address) {
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
    m_elapsedTime.reset();
    m_elapsedTime.stop();

    emit codeChanged (false);
    emit communicationsChanged (false);
    emit voltageChanged (QString (""));
    emit elapsedTimeChanged ("00:00.0");
}

void DriverStation::sendRobotPackets() {
    if (m_init)
        m_client.sendToRobot (m_manager.protocol()->generateClientPacket());
}

QString DriverStation::getStatus() {
    if (!m_manager.protocol()->robotCommunication())
        return "No Robot Communication";

    else if (!m_manager.protocol()->robotCode())
        return "No Robot Code";

    return DS_GetControlModeString (m_manager.protocol()->controlMode());
}

void DriverStation::updateStatus (bool ignore_me) {
    Q_UNUSED (ignore_me);
    emit robotStatusChanged (getStatus());
}

void DriverStation::onControlModeChanged (DS_ControlMode mode) {
    mode == DS_ControlDisabled || mode == DS_ControlEmergencyStop ?
    m_elapsedTime.stop() : m_elapsedTime.reset();

    emit robotStatusChanged (getStatus());
}
