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

#include <QList>

#include "LibDS/Core/ProtocolBase.h"
#include "LibDS/Core/ProtocolManager.h"

//=============================================================================
// DS_ProtocolManager::DS_ProtocolManager
//=============================================================================

DS_ProtocolManager::DS_ProtocolManager()
{
    m_protocol = Q_NULLPTR;
    m_joysticks = new QList<DS_Joystick*>;
}

//=============================================================================
// DS_ProtocolManager::~DS_ProtocolManager
//=============================================================================

DS_ProtocolManager::~DS_ProtocolManager()
{
    delete m_protocol;
    delete m_joysticks;
}

//=============================================================================
// DS_ProtocolManager::CurrentProtocol
//=============================================================================

DS_ProtocolBase* DS_ProtocolManager::CurrentProtocol() const
{
    return ProtocolIsValid() ? m_protocol : Q_NULLPTR;
}

//=============================================================================
// DS_ProtocolManager::ProtocolIsValid
//=============================================================================

bool DS_ProtocolManager::ProtocolIsValid() const
{
    return (m_protocol != Q_NULLPTR);
}

//=============================================================================
// DS_ProtocolManager::SetProtocol
//=============================================================================

void DS_ProtocolManager::SetProtocol (DS_ProtocolBase* protocol)
{
    if (protocol != Q_NULLPTR)
        {
            free (m_protocol);

            m_protocol = protocol;
            m_protocol->SetJoysticks (m_joysticks);

            connect (m_protocol, SIGNAL (CodeChanged            (bool)),
                     this,       SIGNAL (CodeChanged            (bool)));
            connect (m_protocol, SIGNAL (RadioCommChanged       (bool)),
                     this,       SIGNAL (RadioCommChanged       (bool)));
            connect (m_protocol, SIGNAL (CommunicationsChanged  (DS_CommStatus)),
                     this,       SIGNAL (CommunicationsChanged  (DS_CommStatus)));
            connect (m_protocol, SIGNAL (RobotAddressChanged    (QString)),
                     this,       SIGNAL (RobotAddressChanged    (QString)));
            connect (m_protocol, SIGNAL (ControlModeChanged     (DS_ControlMode)),
                     this,       SIGNAL (ControlModeChanged     (DS_ControlMode)));
            connect (m_protocol, SIGNAL (DiskUsageChanged       (int)),
                     this,       SIGNAL (DiskUsageChanged       (int)));
            connect (m_protocol, SIGNAL (RAMUsageChanged        (int)),
                     this,       SIGNAL (RAMUsageChanged        (int)));
            connect (m_protocol, SIGNAL (CPUUsageChanged        (int)),
                     this,       SIGNAL (CPUUsageChanged        (int)));
            connect (m_protocol, SIGNAL (VoltageChanged         (QString)),
                     this,       SIGNAL (VoltageChanged         (QString)));
            connect (m_protocol, SIGNAL (VoltageBrownoutChanged (bool)),
                     this,       SIGNAL (VoltageBrownoutChanged (bool)));
            connect (m_protocol, SIGNAL (CANInfoReceived        (DS_CAN)),
                     this,       SIGNAL (CANInfoReceived        (DS_CAN)));
        }
}

//=============================================================================
// DS_ProtocolManager::ClearJoysticks
//=============================================================================

void DS_ProtocolManager::ClearJoysticks()
{
    m_joysticks->clear();
}

//=============================================================================
// DS_ProtocolManager::AddJoystick
//=============================================================================

void DS_ProtocolManager::AddJoystick (int axes, int buttons, int povHats)
{

    DS_Joystick* js = new DS_Joystick;

    js->numAxes = axes;
    js->numButtons = buttons;
    js->numHats = povHats;

    js->axes = new double  [axes];
    js->hats = new int  [povHats];
    js->buttons = new bool [buttons];

    for (int i = 0; i < js->numAxes; i++)
        js->axes [i] = 0;

    for (int i = 0; i < js->numHats; i++)
        js->hats [i] = -1;

    for (int i = 0; i < js->numButtons; i++)
        js->buttons [i] = false;

    m_joysticks->append (js);
}

//=============================================================================
// DS_ProtocolManager::UpdateJoystickPOV
//=============================================================================

void DS_ProtocolManager::UpdateJoystickPOV (int js, int hat, int angle)
{
    if (JoystickIsValid (js))
        m_joysticks->at (js)->hats [hat] = angle;
}

//=============================================================================
// DS_ProtocolManager::UpdateJoystickAxis
//=============================================================================

void DS_ProtocolManager::UpdateJoystickAxis (int js, int axis, double value)
{
    if (JoystickIsValid (js))
        m_joysticks->at (js)->axes [axis] = value;
}

//=============================================================================
// DS_ProtocolManager::UpdateJoystickButton
//=============================================================================

void DS_ProtocolManager::UpdateJoystickButton (int js, int bt, bool status)
{
    if (JoystickIsValid (js))
        m_joysticks->at (js)->buttons [bt] = status;
}

//=============================================================================
// DS_ProtocolManager::ReadRobotPacket
//=============================================================================

void DS_ProtocolManager::ReadRobotPacket (QByteArray data)
{
    if (ProtocolIsValid())
        CurrentProtocol()->ReadRobotPacket (data);
}

//=============================================================================
// DS_ProtocolManager::JoystickIsValid
//=============================================================================

bool DS_ProtocolManager::JoystickIsValid (int js) const
{
    return (js < m_joysticks->count());
}
