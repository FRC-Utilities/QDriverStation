/*
 * Copyright (c) 2015-2016 WinT 3794 <http://wint3794.org>
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

#include <QList>

//=============================================================================
// Library includes
//=============================================================================

#include "LibDS/Core/ProtocolBase.h"
#include "LibDS/Core/ProtocolManager.h"

using namespace DS_CORE;

//=============================================================================
// Joystick requirements
//=============================================================================

const int MAX_AXES      = 12;
const int MAX_POVS      = 12;
const int MAX_BUTTONS   = 24;
const int MAX_JOYSTICKS =  6;

//=============================================================================
// Joystick error messages/warnings
//=============================================================================

const QString ERR_POV_CNT     = "<p>"
                                "<font color=#FFF959><b>WARNING:</b></font> "
                                "<font color=#FFFFFF>Joystick %1 has exceed "
                                "the POV range (0 to %2 POVs). "
                                "Joystick %1 has %3 POVs.</font>"
                                "</p>";
const QString ERR_AXIS_CNT    = "<p>"
                                "<font color=#FFF959><b>WARNING:</b></font> "
                                "<font color=#FFFFFF>Joystick %1 has exceed "
                                "the axis range (0 to %2 axes). "
                                "Joystick %1 has %3 axes.</font>"
                                "</p>";
const QString ERR_BUTTON_CNT  = "<p>"
                                "<font color=#FFF959><b>WARNING:</b></font> "
                                "<font color=#FFFFFF>Joystick %1 has exceed "
                                "the button range (0 to %2 buttons). "
                                "Joystick %1 has %3 buttons.</font>"
                                "</p>";
const QString ERR_JOYSTICKS   = "<p>"
                                "<font color=#FFF959><b>WARNING:</b></font> "
                                "<font color=#FFFFFF>Reached maximum number of "
                                "joysticks (%1), further joysticks will be "
                                "ignored.</font>"
                                "</p>";

//=============================================================================
//ProtocolManager::DS_ProtocolManager
//=============================================================================

ProtocolManager::ProtocolManager (QObject* parent) : QObject (parent) {
    m_protocol = Q_NULLPTR;
    m_joysticks = new QList<DS::Joystick*>;
}

//=============================================================================
//ProtocolManager::currentProtocol
//=============================================================================

ProtocolBase* ProtocolManager::currentProtocol() const {
    return isValid() ? m_protocol : Q_NULLPTR;
}

//=============================================================================
//ProtocolManager::isValid
//=============================================================================

bool ProtocolManager::isValid() const {
    return (m_protocol != Q_NULLPTR);
}

//=============================================================================
//ProtocolManager::joystickCount
//=============================================================================

int ProtocolManager::joystickCount() const {
    return isValid() ? currentProtocol()->joysticks()->count() : 0;
}

//=============================================================================
//ProtocolManager::setProtocol
//=============================================================================

void ProtocolManager::setProtocol (ProtocolBase* protocol) {
    if (protocol != Q_NULLPTR) {
        /* There is already a running protocol */
        if (isValid()) {
            /* Pass current protocol settings to new protocol */
            protocol->setTeam         (m_protocol->team());
            protocol->setRobotAddress (m_protocol->robotAddress());

            /* De-configure the old protocol */
            m_protocol->disconnect();
        }

        /* Re-assign protocol and joysticks */
        m_protocol = protocol;
        m_protocol->setJoysticks (m_joysticks);

        /* Re-connect protocol signals/slots */
        connect (m_protocol, &ProtocolBase::emergencyStopped,
                 this,       &ProtocolManager::emergencyStopped);
        connect (m_protocol, &ProtocolBase::codeChanged,
                 this,       &ProtocolManager::codeChanged);
        connect (m_protocol, &ProtocolBase::radioCommChanged,
                 this,       &ProtocolManager::radioCommChanged);
        connect (m_protocol, &ProtocolBase::communicationsChanged,
                 this,       &ProtocolManager::communicationsChanged);
        connect (m_protocol, &ProtocolBase::robotAddressChanged,
                 this,       &ProtocolManager::robotAddressChanged);
        connect (m_protocol, &ProtocolBase::controlModeChanged,
                 this,       &ProtocolManager::controlModeChanged);
        connect (m_protocol, &ProtocolBase::diskUsageChanged,
                 this,       &ProtocolManager::diskUsageChanged);
        connect (m_protocol, &ProtocolBase::ramUsageChanged,
                 this,       &ProtocolManager::ramUsageChanged);
        connect (m_protocol, &ProtocolBase::cpuUsageChanged,
                 this,       &ProtocolManager::cpuUsageChanged);
        connect (m_protocol, &ProtocolBase::voltageChanged,
                 this,       &ProtocolManager::voltageChanged);
        connect (m_protocol, &ProtocolBase::voltageBrownoutChanged,
                 this,       &ProtocolManager::voltageBrownoutChanged);
        connect (m_protocol, &ProtocolBase::CANInfoReceived,
                 this,       &ProtocolManager::CANInfoReceived);
        connect (m_protocol, &ProtocolBase::fmsChanged,
                 this,       &ProtocolManager::fmsChanged);


        DS::Log (DS::kLibLevel,
                 QString ("Protocol 0x%1 loaded into memory")
                 .arg ((quintptr)protocol,
                       QT_POINTER_SIZE * 2, 16, QChar ('0')));
    }

    else
        DS::Log (DS::kWarnLevel, "Input protocol pointer is NULL");
}

//=============================================================================
//ProtocolManager::resetJoysticks
//=============================================================================

void ProtocolManager::resetJoysticks() {
    m_joysticks->clear();

    if (isValid())
        currentProtocol()->_onJoysticksChanged();
}

//=============================================================================
//ProtocolManager::addJoystick
//=============================================================================

void ProtocolManager::addJoystick (int axes, int buttons, int POVs) {
    /* The DS has exceeded the supported number of joysticks */
    if (m_joysticks->count() >= MAX_JOYSTICKS) {
        DS::Log  (DS::kLibLevel, "Reached maximum number of joysticks");
        DS::Log  (DS::kLibLevel, "Ignoring future joysticks");
        DS::SendMessage (ERR_JOYSTICKS.arg (MAX_JOYSTICKS));
        return;
    }

    /* The input joysticks has too many POVs */
    else if (POVs > MAX_POVS) {
        DS::Log (DS::kLibLevel, "Input device has exceeded POV range");
        DS::SendMessage (ERR_POV_CNT
                         .arg (m_joysticks->count())
                         .arg (MAX_POVS)
                         .arg (POVs));
        return;
    }

    /* The input joysticks has too many axes */
    else if (axes > MAX_AXES) {
        DS::Log  (DS::kLibLevel, "Input device has exceeded axis range");
        DS::SendMessage (ERR_AXIS_CNT
                         .arg (m_joysticks->count())
                         .arg (MAX_AXES)
                         .arg (axes));
        return;
    }

    /* The input joysticks has too many buttons */
    else if (buttons > MAX_BUTTONS) {
        DS::Log  (DS::kLibLevel, "Input device has exceeded button range");
        DS::SendMessage (ERR_AXIS_CNT
                         .arg (m_joysticks->count())
                         .arg (MAX_BUTTONS)
                         .arg (buttons));
        return;
    }

    /* Everything OK, register the joystick */
    else {
        DS::Joystick* js = new DS::Joystick;

        js->numAxes = axes;
        js->numPOVs = POVs;
        js->numButtons = buttons;

        js->axes = new float  [axes];
        js->POVs = new int  [POVs];
        js->buttons = new bool [buttons];

        for (int i = 0; i < js->numAxes; i++)
            js->axes [i] = 0;

        for (int i = 0; i < js->numPOVs; i++)
            js->POVs [i] = -1;

        for (int i = 0; i < js->numButtons; i++)
            js->buttons [i] = false;

        m_joysticks->append (js);

        if (isValid())
            currentProtocol()->_onJoysticksChanged();

        DS::Log (DS::kLibLevel, QString ("Registered joystick with: "
                                         "%1 axes, "
                                         "%2 buttons and "
                                         "%3 POVs")
                 .arg (QString::number (js->numAxes))
                 .arg (QString::number (js->numButtons))
                 .arg (QString::number (js->numPOVs)));

        DS::Log (DS::kLibLevel, QString ("Current number of joysticks is %1")
                 .arg (QString::number (m_joysticks->count())));
    }
}

//=============================================================================
//ProtocolManager::updateJoystickPOV
//=============================================================================

void ProtocolManager::updateJoystickPOV (int js, int hat, int angle) {
    if (joystickExists (js))
        m_joysticks->at (js)->POVs [hat] = angle;
}

//=============================================================================
//ProtocolManager::updateJoystickAxis
//=============================================================================

void ProtocolManager::updateJoystickAxis (int js, int axis, float value) {
    if (joystickExists (js))
        m_joysticks->at (js)->axes [axis] = value;
}

//=============================================================================
//ProtocolManager::updateJoystickButton
//=============================================================================

void ProtocolManager::updateJoystickButton (int js, int bt, bool status) {
    if (joystickExists (js))
        m_joysticks->at (js)->buttons [bt] = status;
}

//=============================================================================
//ProtocolManager::readRobotPacket
//=============================================================================

void ProtocolManager::readRobotPacket (QByteArray data) {
    if (isValid())
        currentProtocol()->readRobotPacket (data);
}

//=============================================================================
//ProtocolManager::joystickExists
//=============================================================================

bool ProtocolManager::joystickExists (int js) const {
    return (js < m_joysticks->count());
}
