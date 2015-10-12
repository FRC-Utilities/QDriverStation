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

#include "LibDS/DS_ProtocolManager.h"

DS_ProtocolManager::DS_ProtocolManager() {
    m_protocol = Q_NULLPTR;
    m_joysticks = new QList<DS_Joystick*>;
}

DS_ProtocolManager::~DS_ProtocolManager() {
    delete m_protocol;
    delete m_joysticks;
}

DS_Protocol* DS_ProtocolManager::protocol() const {
    return protocolIsValid() ? m_protocol : Q_NULLPTR;
}

bool DS_ProtocolManager::protocolIsValid() const {
    return (m_protocol != Q_NULLPTR);
}

void DS_ProtocolManager::setProtocol (DS_Protocol* protocol) {
    if (protocol != Q_NULLPTR) {
        m_protocol = Q_NULLPTR;

        m_protocol = protocol;
        m_protocol->setJoysticks (m_joysticks);

        connect (m_protocol, SIGNAL (codeChanged           (bool)),
                 this,       SIGNAL (codeChanged           (bool)));
        connect (m_protocol, SIGNAL (communicationsChanged (bool)),
                 this,       SIGNAL (communicationsChanged (bool)));
        connect (m_protocol, SIGNAL (robotAddressChanged   (QString)),
                 this,       SIGNAL (robotAddressChanged   (QString)));
        connect (m_protocol, SIGNAL (controlModeChanged    (DS_ControlMode)),
                 this,       SIGNAL (controlModeChanged    (DS_ControlMode)));
        connect (m_protocol, SIGNAL (diskUsageChanged      (int, int)),
                 this,       SIGNAL (diskUsageChanged      (int, int)));
        connect (m_protocol, SIGNAL (ramUsageChanged       (int, int)),
                 this,       SIGNAL (ramUsageChanged       (int, int)));
        connect (m_protocol, SIGNAL (voltageChanged        (QString)),
                 this,       SIGNAL (voltageChanged        (QString)));
    }
}

void DS_ProtocolManager::clearJoysticks() {
    m_joysticks->clear();
}

void DS_ProtocolManager::addJoystick (int axes, int buttons, int povHats) {
    DS_Joystick* js = new DS_Joystick;

    js->numAxes = axes;
    js->numButtons = buttons;
    js->numPovHats = povHats;

    js->axes = new double [axes];
    js->povHats = new int [povHats];
    js->buttons = new bool [buttons];

    for (int i = 0; i < js->numAxes; i++)
        js->axes [i] = 0;

    for (int i = 0; i < js->numPovHats; i++)
        js->povHats [i] = -1;

    for (int i = 0; i < js->numButtons; i++)
        js->buttons [i] = false;

    m_joysticks->append (js);
}

void DS_ProtocolManager::updateJoystickPovHat (int js, int hat, int angle) {
    if (joystickIsValid (js))
        m_joysticks->at (js)->povHats [hat] = angle;
}

void DS_ProtocolManager::updateJoystickAxis (int js, int axis, double value) {
    if (joystickIsValid (js))
        m_joysticks->at (js)->axes [axis] = value;
}

void DS_ProtocolManager::updateJoystickButton (int js,  int button,
        bool status) {
    if (joystickIsValid (js))
        m_joysticks->at (js)->buttons [button] = status;
}

void DS_ProtocolManager::readRobotData (QByteArray data) {
    if (protocolIsValid())
        protocol()->readRobotData (data);
}

bool DS_ProtocolManager::joystickIsValid (int js) const {
    return (js < m_joysticks->count());
}
