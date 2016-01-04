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

//=============================================================================
// DriverStation::DriverStation
//=============================================================================

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
    connect (m_manager, SIGNAL (CodeChanged            (bool)),
             this,      SLOT   (UpdateStatus           (bool)));
    connect (m_manager, SIGNAL (CommunicationsChanged  (DS_CommStatus)),
             this,      SLOT   (UpdateStatus           (DS_CommStatus)));
    connect (m_manager, SIGNAL (ControlModeChanged     (DS_ControlMode)),
             this,      SLOT   (OnControlModeChanged   (DS_ControlMode)));
    connect (m_manager, SIGNAL (CodeChanged            (bool)),
             this,      SIGNAL (CodeChanged            (bool)));
    connect (m_manager, SIGNAL (CommunicationsChanged  (DS_CommStatus)),
             this,      SIGNAL (CommunicationsChanged  (DS_CommStatus)));
    connect (m_manager, SIGNAL (ControlModeChanged     (DS_ControlMode)),
             this,      SIGNAL (ControlModeChanged     (DS_ControlMode)));
    connect (m_manager, SIGNAL (DiskUsageChanged       (int)),
             this,      SIGNAL (DiskUsageChanged       (int)));
    connect (m_manager, SIGNAL (RAMUsageChanged        (int)),
             this,      SIGNAL (RAMUsageChanged        (int)));
    connect (m_manager, SIGNAL (CPUUsageChanged        (int)),
             this,      SIGNAL (CPUUsageChanged        (int)));
    connect (m_manager, SIGNAL (VoltageChanged         (QString)),
             this,      SIGNAL (VoltageChanged         (QString)));
    connect (m_manager, SIGNAL (VoltageBrownoutChanged (bool)),
             this,      SIGNAL (VoltageBrownoutChanged (bool)));
    connect (m_manager, SIGNAL (CANInfoReceived        (DS_CAN)),
             this,      SIGNAL (CANInfoReceived        (DS_CAN)));

    /* Stop timer when the communications status changes */
    connect (m_manager,     SIGNAL (CommunicationsChanged (DS_CommStatus)),
             m_elapsedTime, SLOT   (Stop()));

    /* Robot information has changed */
    connect (m_manager, SIGNAL (LibVersionChanged (QString)),
             this,      SIGNAL (LibVersionChanged (QString)));
    connect (m_manager, SIGNAL (RIOVersionChanged (QString)),
             this,      SIGNAL (RIOVersionChanged (QString)));
    connect (m_manager, SIGNAL (PCMVersionChanged (QString)),
             this,      SIGNAL (PCMVersionChanged (QString)));
    connect (m_manager, SIGNAL (PDPVersionChanged (QString)),
             this,      SIGNAL (PDPVersionChanged (QString)));

    /* Sync robot address and calculated IPs automatically */
    connect (m_manager, SIGNAL (RobotAddressChanged (QString)),
             m_client,  SLOT   (SetRobotAddress     (QString)));

    /* Update the elapsed time text automatically */
    connect (m_elapsedTime, SIGNAL (ElapsedTimeChanged (QString)),
             this,          SIGNAL (ElapsedTimeChanged (QString)));

    /* New NetConsole message received */
    connect (m_netConsole,  SIGNAL (NewMessage (QString)),
             this,          SIGNAL (NewMessage (QString)));

    /* Send and read robot packets */
    connect (m_client,  SIGNAL (DataReceived     (QByteArray)),
             this,      SLOT   (ReadRobotPacket  (QByteArray)));
    connect (DS_Timers::GetInstance(), SIGNAL    (Timeout20()),
             this,                     SLOT      (SendToRobot()));
}

//=============================================================================
// DriverStation::~DriverStation
//=============================================================================

DriverStation::~DriverStation()
{
    delete m_client;
    delete m_manager;
    delete m_protocol;
    delete m_netConsole;
    delete m_elapsedTime;

    delete s_instance;
}

//=============================================================================
// DriverStation::GetInstance
//=============================================================================

DriverStation* DriverStation::GetInstance()
{
    if (s_instance == Q_NULLPTR)
        s_instance = new DriverStation();

    return s_instance;
}

//=============================================================================
// DriverStation::CanBeEnabled
//=============================================================================

bool DriverStation::CanBeEnabled()
{
    return RobotHasCode() && IsConnected() && !IsEmergencyStopped();
}

//=============================================================================
// DriverStation::Alliances
//=============================================================================

QStringList DriverStation::Alliances()
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

//=============================================================================
// DriverStation::Protocols
//=============================================================================

QStringList DriverStation::Protocols()
{
    QStringList list;
    list.append ("2015 Protocol");
    return list;
}

//=============================================================================
// DriverStation::RadioAddress
//=============================================================================

QString DriverStation::RadioAddress()
{
    if (m_manager->ProtocolIsValid())
        return m_manager->CurrentProtocol()->DefaultRadioAddresses().at (0);

    return "";
}

//=============================================================================
// DriverStation::RobotAddress
//=============================================================================

QString DriverStation::RobotAddress()
{
    if (m_manager->ProtocolIsValid())
        return m_manager->CurrentProtocol()->RobotAddress();

    return "";
}

//=============================================================================
// DriverStation::DefaultRobotAddress
//=============================================================================

QString DriverStation::DefaultRobotAddress()
{
    if (m_manager->ProtocolIsValid())
        return m_manager->CurrentProtocol()->DefaultRobotAddresses().at (0);

    return "";
}

//=============================================================================
// DriverStation::DefaultRobotAddresses
//=============================================================================

QStringList DriverStation::DefaultRobotAddresses()
{
    if (m_manager->ProtocolIsValid())
        return m_manager->CurrentProtocol()->DefaultRobotAddresses();

    return QStringList ("");
}

//=============================================================================
// DriverStation::ControlMode
//=============================================================================

DS_ControlMode DriverStation::ControlMode()
{
    if (m_manager->ProtocolIsValid())
        return m_manager->CurrentProtocol()->ControlMode();

    return kControlInvalid;
}

//=============================================================================
// DriverStation::RobotHasCode
//=============================================================================

bool DriverStation::RobotHasCode()
{
    if (m_manager->ProtocolIsValid())
        return m_manager->CurrentProtocol()->RobotHasCode();

    return false;
}

//=============================================================================
// DriverStation::IsEnabled
//=============================================================================

bool DriverStation::IsEnabled()
{
    if (m_manager->ProtocolIsValid())
        return m_manager->CurrentProtocol()->IsEnabled();

    return false;
}

//=============================================================================
// DriverStation::IsEmergencyStopped
//=============================================================================

bool DriverStation::IsEmergencyStopped()
{
    if (m_manager->ProtocolIsValid())
        return m_manager->CurrentProtocol()->IsEmergencyStopped();

    return false;
}

//=============================================================================
// DriverStation::IsConnected
//=============================================================================

bool DriverStation::IsConnected()
{
    if (m_manager->ProtocolIsValid())
        return m_manager->CurrentProtocol()->IsConnectedToRobot();

    return false;
}

//=============================================================================
// DriverStation::Init
//=============================================================================

void DriverStation::Init()
{
    if (!m_init)
        {
            m_init = true;

            QTimer::singleShot (1500, this, SLOT (ResetInternalValues()));
            QTimer::singleShot (1500, DS_Timers::GetInstance(), SLOT (Start()));
        }
}

//=============================================================================
// DriverStation::RebootRobot
//=============================================================================

void DriverStation::RebootRobot()
{
    if (m_manager->ProtocolIsValid())
        m_manager->CurrentProtocol()->Reboot();
}

//=============================================================================
// DriverStation::RestartCode
//=============================================================================

void DriverStation::RestartCode()
{
    if (m_manager->ProtocolIsValid())
        m_manager->CurrentProtocol()->RestartCode();
}

//=============================================================================
// DriverStation::SetEnabled
//=============================================================================

void DriverStation::SetEnabled (bool enabled)
{
    if (m_manager->ProtocolIsValid())
        {
            m_manager->CurrentProtocol()->SetEnabled (enabled);

            if (enabled)
                m_elapsedTime->Reset();

            else
                m_elapsedTime->Stop();
        }
}

//=============================================================================
// DriverStation::SetEmergencyStopped
//=============================================================================

void DriverStation::SetEmergencyStopped (bool emergency_stop)
{
    if (m_manager->ProtocolIsValid())
        m_manager->CurrentProtocol()->SetEmergencyStopped (emergency_stop);
}

//=============================================================================
// DriverStation::StartPractice
//=============================================================================

void DriverStation::Practice (int countdown,
                              int autonomous,
                              int delay,
                              int teleop,
                              int endgame)
{
    qDebug() << Q_FUNC_INFO << "TODO";

    /* Transform the times from seconds to milliseconds */
    delay *= 1000;
    teleop *= 1000;
    endgame *= 1000;
    countdown *= 1000;
    autonomous *= 1000;

    /* Calculate the time frames */
    quint32 _startAutonomous = countdown;
    quint32 _startTeleoperated = _startAutonomous + delay + autonomous;
    quint32 _startEndGame = _startTeleoperated + delay + teleop - endgame;
    quint32 _stop = _startEndGame + endgame;

    /* Configure the sound/media timers */
    QTimer::singleShot (_startAutonomous, Qt::PreciseTimer,
                        this, SLOT (AutonomousStartSoundEffect()));
    QTimer::singleShot (_startTeleoperated, Qt::PreciseTimer,
                        this, SLOT (TeleopStartSoundEffect()));
    QTimer::singleShot (_stop, Qt::PreciseTimer,
                        this, SLOT (MatchEndSoundEffect()));
    QTimer::singleShot (_startEndGame, Qt::PreciseTimer,
                        this, SLOT (EndGameSoundEffect()));
}

//=============================================================================
// DriverStation::SetProtocol
//=============================================================================

void DriverStation::SetProtocol (DS_ProtocolBase* protocol)
{
    if (protocol != Q_NULLPTR)
        {
            m_manager->SetProtocol  (protocol);
            m_client->SetRobotPort  (protocol->RobotPort());
            m_client->SetClientPort (protocol->ClientPort());
            m_netConsole->SetPort   (protocol->NetConsolePort());
        }
}

//=============================================================================
// DriverStation::SetProtocol
//=============================================================================

void DriverStation::SetProtocol (ProtocolType protocol)
{
    if (protocol == kProtocol2015)
        {
            m_protocol = new DS_Protocol2015;
            SetProtocol (m_protocol);
        }
}

//=============================================================================
// DriverStation::SetTeamNumber
//=============================================================================

void DriverStation::SetTeamNumber (int team)
{
    if (m_manager->ProtocolIsValid())
        m_manager->CurrentProtocol()->SetTeamNumber (team);
}

//=============================================================================
// DriverStation::SetAlliance
//=============================================================================

void DriverStation::SetAlliance (DS_Alliance alliance)
{
    if (m_manager->ProtocolIsValid())
        m_manager->CurrentProtocol()->SetAlliance (alliance);
}

//=============================================================================
// DriverStation::SetControlMode
//=============================================================================

void DriverStation::SetControlMode (DS_ControlMode mode)
{
    if (m_manager->ProtocolIsValid())
        m_manager->CurrentProtocol()->SetControlMode (mode);
}

//=============================================================================
// DriverStation::SetCustomAddress
//=============================================================================

void DriverStation::SetCustomAddress (QString address)
{
    if (m_manager->ProtocolIsValid())
        m_manager->CurrentProtocol()->SetRobotAddress (address);
}

//=============================================================================
// DriverStation::ClearJoysticks
//=============================================================================

void DriverStation::ClearJoysticks()
{
    m_manager->ClearJoysticks();
}

//=============================================================================
// DriverStation::UpdateJoystickPOV
//=============================================================================

void DriverStation::UpdateJoystickPOV (int js, int hat, int angle)
{
    if (m_manager->ProtocolIsValid())
        m_manager->UpdateJoystickPOV (js, hat, angle);
}

//=============================================================================
// DriverStation::AddJoystick
//=============================================================================

void DriverStation::AddJoystick (int axes, int buttons, int povHats)
{
    if (m_manager->ProtocolIsValid())
        m_manager->AddJoystick (axes, buttons, povHats);
}

//=============================================================================
// DriverStation::UpdateJoystickAxis
//=============================================================================

void DriverStation::UpdateJoystickAxis (int js, int axis, double value)
{
    if (m_manager->ProtocolIsValid())
        m_manager->UpdateJoystickAxis (js, axis, value);
}

//=============================================================================
// DriverStation::UpdateJoystickButton
//=============================================================================

void DriverStation::UpdateJoystickButton (int js, int button, bool pressed)
{
    if (m_manager->ProtocolIsValid())
        m_manager->UpdateJoystickButton (js, button, pressed);
}

//=============================================================================
// DriverStation::GetClientStatus
//=============================================================================

QString DriverStation::GetClientStatus()
{
    if (m_manager->ProtocolIsValid()
            && m_manager->CurrentProtocol()->IsConnectedToRobot())
        {
            if (m_manager->CurrentProtocol()->IsEmergencyStopped())
                return "Emergency Stopped";

            if (!m_manager->CurrentProtocol()->RobotHasCode())
                return "No Robot Code";

            return QString ("%1 %2")
                   .arg (DS_GetControlModeString (ControlMode()))
                   .arg (IsEnabled() ? "Enabled" : "Disabled");
        }

    return QString ("No Robot Communication");
}

//=============================================================================
// DriverStation::SendToRobot
//=============================================================================

void DriverStation::SendToRobot()
{
    if (m_manager->ProtocolIsValid())
        m_client->SendToRobot (m_manager->CurrentProtocol()->CreateClientPacket());
}

//=============================================================================
// DriverStation::ResetInternalValues
//=============================================================================

void DriverStation::ResetInternalValues()
{
    m_elapsedTime->Reset();
    m_elapsedTime->Stop();

    emit CodeChanged (false);
    emit VoltageChanged (QString (""));
    emit ElapsedTimeChanged ("00:00.0");
    emit CommunicationsChanged (kFailing);
}

//=============================================================================
// DriverStation::ReadRobotPacket
//=============================================================================

void DriverStation::ReadRobotPacket (QByteArray robotResponse)
{
    if (m_manager->ProtocolIsValid())
        m_manager->CurrentProtocol()->ReadRobotPacket (robotResponse);
}

//=============================================================================
// DriverStation::UpdateStatus
//=============================================================================

void DriverStation::UpdateStatus (bool ignored)
{
    Q_UNUSED (ignored);
    emit RobotStatusChanged (GetClientStatus());
}

//=============================================================================
// DriverStation::UpdateStatus
//=============================================================================

void DriverStation::UpdateStatus (DS_CommStatus ignored)
{
    Q_UNUSED (ignored);
    emit RobotStatusChanged (GetClientStatus());
}

//=============================================================================
// DriverStation::OnControlModeChanged
//=============================================================================

void DriverStation::OnControlModeChanged (DS_ControlMode mode)
{
    Q_UNUSED (mode);
    emit RobotStatusChanged (GetClientStatus());
}

//=============================================================================
// DriverStation::EndGameSoundEffect
//=============================================================================

void DriverStation::EndGameSoundEffect()
{
    QSound::play (":/LibDS/Start of End Game_normalized.wav");
}

//=============================================================================
// DriverStation::MatchEndSoundEffect
//=============================================================================

void DriverStation::MatchEndSoundEffect()
{
    QSound::play (":/LibDS/Match End_normalized.wav");
}

//=============================================================================
// DriverStation::TeleopStartSoundEffect
//=============================================================================

void DriverStation::TeleopStartSoundEffect()
{
    QSound::play (":/LibDS/Start Teleop_normalized.wav");
}

//=============================================================================
// DriverStation::AutonomousStartSoundEffect
//=============================================================================

void DriverStation::AutonomousStartSoundEffect()
{
    QSound::play (":/LibDS/Start Auto_normalized.wav");
}
