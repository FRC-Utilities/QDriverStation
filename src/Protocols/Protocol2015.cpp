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

#include "LibDS/Protocols/Protocol2015.h"

//=============================================================================
// FTP locations
//=============================================================================

const QString PCM_PATH = "/tmp/frc_versions/PCM-0-versions.ini";
const QString PDP_PATH = "/tmp/frc_versions/PDP-0-versions.ini";
const QString LIB_PATH = "/tmp/frc_versions/FRC_Lib_Version.ini";

//=============================================================================
// Protocol codes/standards
//=============================================================================

/**
 * The values used by the protocol to represent the different operation modes
 */
enum ControlModes
{
    pEmStopOn            = 128, /**< Emergency stop enabled */
    pEmStopOff           =   0, /**< Emergency stop disabled */
    pEnabled             =   4, /**< Robot Enabled */
    pDisabled            =   0, /**< Robot Disabled */
    pControlTest         =   1, /**< Individual actuators can be moved */
    pControlAutonomous   =   2, /**< Robot takes over the world */
    pControlTeleoperated =   0, /**< User moves the robot */
};

/**
 * Used by the robot to identify the different sections of the client packets
 */
enum Headers
{
    pHeaderTime     = 15, /**< Indicates start of current time data */
    pHeaderGeneral  =  1, /**< Indicates a client packet */
    pHeaderJoystick = 12, /**< Indicates start of JS section */
    pHeaderTimezone = 16, /**< Indicates start of Timezone data */
};

/**
 * Used by the DS to identify extra data structures sent by the robot
 */
enum RIOHeaders
{
    pRIOHeaderJoystickOutput  =  1, /**< Asks us to rumble joysticks */
    pRIOHeaderDiskInformation =  4, /**< Gives us roboRIO disk metrics */
    pRIOHeaderCPUInformation  =  5, /**< Gives us roboRIO CPU metrics */
    pRIOHeaderRAMInformation  =  6, /**< Gives us roboRIO RAM metrics */
    pRIOHeaderCANMetrics      = 14, /**< Gives us robot CAN net metrics */
};

/**
 * 'Special' operation codes that we send to robot
 */
enum RequestBytes
{
    pStatusNormal      = 16, /**< Normal operation */
    pStatusInvalid     =  0, /**< No communication */
    pStatusRebootRio   = 24, /**< Reboot the roboRIO */
    pStatusRestartCode = 20  /**< Kill the user code */
};

/**
 * Echo codes from the robot and software status
 */
enum ProgramStatus
{
    pProgramTest         =  pControlTest,         /**< Control mode confirmation code */
    pProgramAutonomous   =  pControlAutonomous,   /**< Control mode confirmation code */
    pProgramCodePresent  =  32,                   /**< Not very elegant, but it works */
    pProgramTeleoperated =  pControlTeleoperated, /**< Control mode confirmation code */
};

/**
 * Operations that the robot wants us to perform
 */
enum RobotRequestBytes
{
    pProgramRequestTime = 1, /**< Robot wants to get the date/time */
};

/**
 * Used by the user code in some cases, defines the current alliance
 * and position held by the robot
 */
enum Alliances
{
    pRed1  = 0, /**< Red alliance, position 1  */
    pRed2  = 1, /**< Red alliance, position 2  */
    pRed3  = 2, /**< Red alliance, position 3  */
    pBlue1 = 3, /**< Blue alliance, position 1 */
    pBlue2 = 4, /**< Blue alliance, position 2 */
    pBlue3 = 5, /**< Blue alliance, position 3 */
};

//=============================================================================
// DS_Protocol2015::DS_Protocol2015
//=============================================================================

DS_Protocol2015::DS_Protocol2015()
{
    Reset();
    connect (&m_manager, SIGNAL (finished (QNetworkReply*)),
             this,       SLOT   (ProcessRobotInformation (QNetworkReply*)));
}

//=============================================================================
// DS_Protocol2015::RobotPort
//=============================================================================

int DS_Protocol2015::RobotPort()
{
    return 1110;
}

//=============================================================================
// DS_Protocol2015::ClientPort
//=============================================================================

int DS_Protocol2015::ClientPort()
{
    return 1150;
}

//=============================================================================
// DS_Protocol2015::NetConsolePort
//=============================================================================

int DS_Protocol2015::NetConsolePort()
{
    return 6666;
}

//=============================================================================
// DS_Protocol2015::NetConsoleAcceptsInput
//=============================================================================

bool DS_Protocol2015::NetConsoleAcceptsInput()
{
    return false;
}

//=============================================================================
// DS_Protocol2015::DefaultRadioAddress
//=============================================================================

QStringList DS_Protocol2015::DefaultRadioAddresses()
{
    QStringList list;
    list.append (DS_GetStaticIp (Team(), 1));
    return list;
}

//=============================================================================
// DS_Protocol2015::DefaultRobotAddress
//=============================================================================

QStringList DS_Protocol2015::DefaultRobotAddresses()
{
    QStringList list;
    list.append (QString ("roboRIO-%1.local").arg (Team())); // Dynamic mDNS
    list.append (QString (DS_GetStaticIp (172, Team(), 2))); // Static USB
    list.append (QString (DS_GetStaticIp (Team(), 2)));      // Static Ethernet
    return list;
}

//=============================================================================
// DS_Protocol2015::Reboot
//=============================================================================

void DS_Protocol2015::Reboot()
{
    UpdateStatus (pStatusRebootRio);
}

//=============================================================================
// DS_Protocol2015::RestartCode
//=============================================================================

void DS_Protocol2015::RestartCode()
{
    UpdateStatus (pStatusRestartCode);
}

//=============================================================================
// DS_Protocol2015::ResetProtocol
//=============================================================================

void DS_Protocol2015::ResetProtocol()
{
    UpdateStatus (pStatusInvalid);
}

//=============================================================================
// DS_Protocol2015::GetRobotInformation
//=============================================================================

void DS_Protocol2015::GetRobotInformation()
{
    QString host = "FTP_://" + RobotAddress();
    m_manager.get (QNetworkRequest (host + LIB_PATH));
    m_manager.get (QNetworkRequest (host + PCM_PATH));
    m_manager.get (QNetworkRequest (host + PDP_PATH));
}

//=============================================================================
// DS_Protocol2015::ProcessRobotInformation
//=============================================================================

void DS_Protocol2015::ProcessRobotInformation (QNetworkReply* reply)
{
    QString url = reply->url().toString();
    QString data = QString::fromUtf8 (reply->readAll());

    if (data.isEmpty() || url.isEmpty())
        return;

    else if (url.contains (PCM_PATH, Qt::CaseInsensitive))
        {
            QString version;
            QString key = "currentVersion";

            version.append (data.at (data.indexOf (key) + key.length() + 1));
            version.append (data.at (data.indexOf (key) + key.length() + 2));
            version.append (data.at (data.indexOf (key) + key.length() + 3));
            version.append (data.at (data.indexOf (key) + key.length() + 4));

            emit PCMVersionChanged (version);
        }

    else if (url.contains (PDP_PATH, Qt::CaseInsensitive))
        {
            QString version;
            QString key = "currentVersion";

            version.append (data.at (data.indexOf (key) + key.length() + 1));
            version.append (data.at (data.indexOf (key) + key.length() + 2));
            version.append (data.at (data.indexOf (key) + key.length() + 3));
            version.append (data.at (data.indexOf (key) + key.length() + 4));

            emit PDPVersionChanged (version);
        }

    else if (url.contains (LIB_PATH, Qt::CaseInsensitive))
        emit LibVersionChanged (data);

    delete reply;
}

//=============================================================================
// DS_Protocol2015::ReadPacket
//=============================================================================

bool DS_Protocol2015::ReadPacket (QByteArray data)
{
    int offset = 8;

    /* Packet length is invalid, watchdog will not be reset */
    if (data.length() < offset)
        return false;

    /* Be sure to reset status bit */
    if (Status() == pStatusInvalid)
        UpdateStatus (pStatusNormal);

    /* Read robot packet */
    quint8 opcode       = data.at (3);
    quint8 status       = data.at (4);
    quint8 request      = data.at (7);
    quint8 majorVoltage = data.at (5);
    quint8 minorVoltage = data.at (6);

    /* Update e-stop information */
    SetEmergencyStopped (opcode == pEmStopOn);

    /* Update client information */
    UpdateVoltageBrownout (false);
    UpdateVoltage         (majorVoltage, minorVoltage);
    UpdateSendDateTime    (request == pProgramRequestTime);
    UpdateRobotCode       ((status & pProgramCodePresent) != 0);

    /* Packet contains additional data structures */
    if (data.length() > offset)
        {
            int id = data.at (offset);

            /* Robot wants us to rumble a joystick */
            if (id == pRIOHeaderJoystickOutput)
                qDebug() << Q_FUNC_INFO << "TODO: Joystick Output";

            /* Packet contains roboRIO CPU info */
            else if (id == pRIOHeaderCPUInformation)
                qDebug() << Q_FUNC_INFO << "TODO: CPU information";

            /* Packet contains roboRIO RAM info */
            else if (id == pRIOHeaderRAMInformation)
                {
                    if (data.size() >= offset + 4)
                        emit RAMUsageChanged (data.at (offset + 3));
                }

            /* Packet contains roboRIO disk info */
            else if (id == pRIOHeaderDiskInformation)
                {
                    if (data.size() >= offset + 4)
                        emit DiskUsageChanged (data.at (offset + 3));
                }

            /* Packet contains CAN metrics data */
            else if (id == pRIOHeaderCANMetrics)
                {
                    if (data.size() >= offset + 14)
                        {
                            DS_CAN can;
                            can.util = data.at (offset + 9);
                            can.busOff = data.at (offset + 10);
                            can.txFull = data.at (offset + 11);
                            can.receive = data.at (offset + 12);
                            can.transmit = data.at (offset + 13);

                            emit CANInfoReceived (can);
                        }
                }
        }

    /* Packet was successfully read, reset watchdog */
    return true;
}

//=============================================================================
// DS_Protocol2015::GetClientPacket
//=============================================================================

QByteArray DS_Protocol2015::GetClientPacket()
{
    QByteArray data;

    /* Used for rebooting the robot and restarting its code */
    quint8 status = IsConnectedToRobot() ? Status() : pStatusInvalid;

    /* Defines operation mode, e-stop state & enabled state */
    quint8 opcode = (IsEmergencyStopped() ? pEmStopOn : pEmStopOff) |
                    (IsEnabled() ? pEnabled : pDisabled) |
                    (GetControlCode());

    /* Gets timezone data or joystick input */
    QByteArray extensions = SendDateTime() ? GetTimezoneData() : GetJoystickData();

    /* Construct the packet */
    data.append (DS_ToBytes (SentPackets())); // Ping data
    data.append (pHeaderGeneral);             // Protocol version code
    data.append (opcode);                     // Operation code
    data.append (status);                     // Special instructions
    data.append (GetAllianceCode());          // Alliance & position
    data.append (extensions);                 // Joystick data or UTC info

    return data;
}

//=============================================================================
// DS_Protocol2015::GetJoystickData
//=============================================================================

QByteArray DS_Protocol2015::GetJoystickData()
{
    QByteArray data;

    /* Do not send JS data on DS init */
    if (SentPackets() <= 5)
        return data;

    /* Generate data for each joystick */
    for (int i = 0; i < Joysticks()->count(); ++i)
        {
            int numAxes    = Joysticks()->at (i)->numAxes;
            int numButtons = Joysticks()->at (i)->numButtons;
            int numPovHats = Joysticks()->at (i)->numHats;

            /* Add joystick information and put the section header */
            data.append (GetJoystickSize (Joysticks()->at (i)) - 1);
            data.append (pHeaderJoystick);

            /* Add axis data */
            data.append (numAxes);
            for (int axis = 0; axis < numAxes; ++axis)
                data.append (Joysticks()->at (i)->axes [axis] * 127);

            /* Generate button data */
            int buttons = 0;
            for (int button = 0; button < numButtons; ++button)
                {
                    bool pressed = Joysticks()->at (i)->buttons [button];
                    buttons += pressed ? qPow (2, button) : 0;
                }

            /* Add button data */
            data.append (numButtons);
            data.append (DS_ToBytes (buttons));

            /* Add hat/pov data */
            data.append (numPovHats);
            for (int hat = 0; hat < numPovHats; ++hat)
                data.append (DS_ToBytes (Joysticks()->at (i)->hats [hat]));
        }

    return data;
}

//=============================================================================
// DS_Protocol2015::GetTimezoneData
//=============================================================================

QByteArray DS_Protocol2015::GetTimezoneData()
{
    QByteArray data;

    /* Add size (always 11) */
    data.append (11);

    /* Get current date/time */
    QDateTime dt = QDateTime::currentDateTime();
    QDate date = dt.date();
    QTime time = dt.time();

    /* Add current date/time */
    data.append (pHeaderTime);
    data.append (time.msec() / 255);
    data.append (time.msec() - (time.msec() / 255));
    data.append (time.second());
    data.append (time.minute());
    data.append (time.hour());
    data.append (date.day());
    data.append (date.month());
    data.append (date.year() - 1900);

    /* Add timezone data */
    data.append (DS_GetTimezoneCode().length() + 1);
    data.append (pHeaderTimezone);
    data.append (DS_GetTimezoneCode());

    return data;
}

//=============================================================================
// DS_Protocol2015::GetControlCode
//=============================================================================

int DS_Protocol2015::GetControlCode()
{
    if (ControlMode() == kControlTest)
        return pControlTest;

    else if (ControlMode() == kControlAutonomous)
        return pControlAutonomous;

    else if (ControlMode() == kControlTeleoperated)
        return pControlTeleoperated;

    return pControlTeleoperated;
}

//=============================================================================
// DS_Protocol2015::GetAllianceCode
//=============================================================================

int DS_Protocol2015::GetAllianceCode()
{
    if (Alliance() == kAllianceRed1)
        return pRed1;

    else if (Alliance() == kAllianceRed2)
        return pRed2;

    else if (Alliance() == kAllianceRed3)
        return pRed3;

    else if (Alliance() == kAllianceBlue1)
        return pBlue1;

    else if (Alliance() == kAllianceBlue2)
        return pBlue2;

    else if (Alliance() == kAllianceBlue3)
        return pBlue3;

    return pRed1;
}

//=============================================================================
// DS_Protocol2015::GetJoystickSize
//=============================================================================

int DS_Protocol2015::GetJoystickSize (DS_Joystick* joystick)
{
    return  5
            + (joystick->numAxes > 0 ? joystick->numAxes : 0)
            + (joystick->numButtons / 8)
            + (joystick->numButtons % 8 == 0 ? 0 : 1)
            + (joystick->numHats > 0 ? joystick->numHats * 2 : 0);
}
