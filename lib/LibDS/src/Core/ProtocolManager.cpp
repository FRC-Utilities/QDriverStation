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
// DS_ProtocolManager::DS_ProtocolManager
//=============================================================================

DS_ProtocolManager::DS_ProtocolManager (QObject* parent) : QObject (parent) {
    m_protocol = Q_NULLPTR;
    m_joysticks = new QList<DS_Joystick*>;
}

//=============================================================================
// DS_ProtocolManager::currentProtocol
//=============================================================================

DS_ProtocolBase* DS_ProtocolManager::currentProtocol() const {
    return isValid() ? m_protocol : Q_NULLPTR;
}

//=============================================================================
// DS_ProtocolManager::isValid
//=============================================================================

bool DS_ProtocolManager::isValid() const {
    return (m_protocol != Q_NULLPTR);
}

//=============================================================================
// DS_ProtocolManager::joystickCount
//=============================================================================

int DS_ProtocolManager::joystickCount() const {
    return isValid() ? currentProtocol()->joysticks()->count() : 0;
}

//=============================================================================
// DS_ProtocolManager::setProtocol
//=============================================================================

void DS_ProtocolManager::setProtocol (DS_ProtocolBase* protocol) {
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
        connect (m_protocol, &DS_ProtocolBase::emergencyStopped,
                 this,       &DS_ProtocolManager::emergencyStopped);
        connect (m_protocol, &DS_ProtocolBase::codeChanged,
                 this,       &DS_ProtocolManager::codeChanged);
        connect (m_protocol, &DS_ProtocolBase::radioCommChanged,
                 this,       &DS_ProtocolManager::radioCommChanged);
        connect (m_protocol, &DS_ProtocolBase::communicationsChanged,
                 this,       &DS_ProtocolManager::communicationsChanged);
        connect (m_protocol, &DS_ProtocolBase::robotAddressChanged,
                 this,       &DS_ProtocolManager::robotAddressChanged);
        connect (m_protocol, &DS_ProtocolBase::controlModeChanged,
                 this,       &DS_ProtocolManager::controlModeChanged);
        connect (m_protocol, &DS_ProtocolBase::diskUsageChanged,
                 this,       &DS_ProtocolManager::diskUsageChanged);
        connect (m_protocol, &DS_ProtocolBase::ramUsageChanged,
                 this,       &DS_ProtocolManager::ramUsageChanged);
        connect (m_protocol, &DS_ProtocolBase::cpuUsageChanged,
                 this,       &DS_ProtocolManager::cpuUsageChanged);
        connect (m_protocol, &DS_ProtocolBase::voltageChanged,
                 this,       &DS_ProtocolManager::voltageChanged);
        connect (m_protocol, &DS_ProtocolBase::voltageBrownoutChanged,
                 this,       &DS_ProtocolManager::voltageBrownoutChanged);
        connect (m_protocol, &DS_ProtocolBase::CANInfoReceived,
                 this,       &DS_ProtocolManager::CANInfoReceived);
        connect (m_protocol, &DS_ProtocolBase::fmsChanged,
                 this,       &DS_ProtocolManager::fmsChanged);


        DS_LogMessage (kLibLevel,
                       QString ("Protocol 0x%1 loaded into memory")
                       .arg ((quintptr)protocol,
                             QT_POINTER_SIZE * 2, 16, QChar ('0')));
    }

    else
        DS_LogMessage (kWarnLevel, "Input protocol pointer is NULL");
}

//=============================================================================
// DS_ProtocolManager::resetJoysticks
//=============================================================================

void DS_ProtocolManager::resetJoysticks() {
    m_joysticks->clear();

    if (isValid())
        currentProtocol()->_onJoysticksChanged();
}

//=============================================================================
// DS_ProtocolManager::addJoystick
//=============================================================================

void DS_ProtocolManager::addJoystick (int axes, int buttons, int POVs) {
    /* The DS has exceeded the supported number of joysticks */
    if (m_joysticks->count() >= MAX_JOYSTICKS) {
        DS_LogMessage  (kLibLevel, "Reached maximum number of joysticks");
        DS_LogMessage  (kLibLevel, "Ignoring future joysticks");
        DS_SendMessage (ERR_JOYSTICKS.arg (MAX_JOYSTICKS));
        return;
    }

    /* The input joysticks has too many POVs */
    else if (POVs > MAX_POVS) {
        DS_LogMessage (kLibLevel, "Input device has exceeded POV range");
        DS_SendMessage (ERR_POV_CNT
                        .arg (m_joysticks->count())
                        .arg (MAX_POVS)
                        .arg (POVs));
        return;
    }

    /* The input joysticks has too many axes */
    else if (axes > MAX_AXES) {
        DS_LogMessage  (kLibLevel, "Input device has exceeded axis range");
        DS_SendMessage (ERR_AXIS_CNT
                        .arg (m_joysticks->count())
                        .arg (MAX_AXES)
                        .arg (axes));
        return;
    }

    /* The input joysticks has too many buttons */
    else if (buttons > MAX_BUTTONS) {
        DS_LogMessage  (kLibLevel, "Input device has exceeded button range");
        DS_SendMessage (ERR_AXIS_CNT
                        .arg (m_joysticks->count())
                        .arg (MAX_BUTTONS)
                        .arg (buttons));
        return;
    }

    /* Everything OK, register the joystick */
    else {
        DS_Joystick* js = new DS_Joystick;

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

        DS_LogMessage (kLibLevel, QString ("Registered joystick with: "
                                           "%1 axes, "
                                           "%2 buttons and "
                                           "%3 POVs")
                       .arg (QString::number (js->numAxes))
                       .arg (QString::number (js->numButtons))
                       .arg (QString::number (js->numPOVs)));

        DS_LogMessage (kLibLevel, QString ("Current number of joysticks is %1")
                       .arg (QString::number (m_joysticks->count())));
    }
}

//=============================================================================
// DS_ProtocolManager::updateJoystickPOV
//=============================================================================

void DS_ProtocolManager::updateJoystickPOV (int js, int hat, int angle) {
    if (joystickExists (js))
        m_joysticks->at (js)->POVs [hat] = angle;
}

//=============================================================================
// DS_ProtocolManager::updateJoystickAxis
//=============================================================================

void DS_ProtocolManager::updateJoystickAxis (int js, int axis, float value) {
    if (joystickExists (js))
        m_joysticks->at (js)->axes [axis] = value;
}

//=============================================================================
// DS_ProtocolManager::updateJoystickButton
//=============================================================================

void DS_ProtocolManager::updateJoystickButton (int js, int bt, bool status) {
    if (joystickExists (js))
        m_joysticks->at (js)->buttons [bt] = status;
}

//=============================================================================
// DS_ProtocolManager::readRobotPacket
//=============================================================================

void DS_ProtocolManager::readRobotPacket (QByteArray data) {
    if (isValid())
        currentProtocol()->readRobotPacket (data);
}

//=============================================================================
// DS_ProtocolManager::joystickExists
//=============================================================================

bool DS_ProtocolManager::joystickExists (int js) const {
    return (js < m_joysticks->count());
}
