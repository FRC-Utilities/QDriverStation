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
enum ControlModes {
    pControlTest            = 0x05u, /**< Individual actuators can be moved */
    pControlDisabled        = 0x00u, /**< Robot is idle */
    pControlAutonomous      = 0x06u, /**< Robot takes over the world */
    pControlTeleoperated    = 0x04u, /**< User moves the robot */
    pControlEmergencyStop   = 0x80u, /**< Forced robot stop */
};

/**
 * Used by the robot to identify the different sections of the client packets
 */
enum Headers {
    pHeaderTime             = 0x0Fu, /**< Indicates start of current time data */
    pHeaderGeneral          = 0x01u, /**< Indicates a client packet */
    pHeaderJoystick         = 0x0Cu, /**< Indicates start of JS section */
    pHeaderTimezone         = 0x10u  /**< Indicates start of Timezone data */
};

/**
 * 'Special' operation codes that we send to robot
 */
enum RequestBytes {
    pStatusNormal           = 0x10u, /**< Normal operation */
    pStatusInvalid          = 0x00u, /**< No communication */
    pStatusRebootRio        = 0x18u, /**< Reboot the roboRIO */
    pStatusRestartCode      = 0x14u  /**< Kill the user code */
};

/**
 * Echo codes from the robot and (probably) software status
 */
enum ProgramStatus {
    pProgramTest            = 0x08u, /**< Control mode confirmation code */
    pProgramDisabled        = 0x01u, /**< Control mode confirmation code */
    pProgramAutonomous      = 0x04u, /**< Control mode confirmation code */
    pProgramCodePresent     = 0x20u, /**< Not sure if this is correct */
    pProgramTeleoperated    = 0x02u  /**< Control mode confirmation code */
};

/**
 * Operations that the robot wants us to perform
 */
enum RobotRequestBytes {
    pProgramRequestTime     = 0x01u, /**< Robot wants to get the date/time */
};

/**
 * Used by the user code in some cases, defines the current alliance
 * and position held by the robot
 */
enum Alliances {
    pRed1                   = 0x00u, /**< Red alliance, position 1  */
    pRed2                   = 0x01u, /**< Red alliance, position 2  */
    pRed3                   = 0x03u, /**< Red alliance, position 3  */
    pBlue1                  = 0x04u, /**< Blue alliance, position 1 */
    pBlue2                  = 0x05u, /**< Blue alliance, position 2 */
    pBlue3                  = 0x06u  /**< Blue alliance, position 3 */
};

//=============================================================================
// DS_Protocol2015::DS_Protocol2015
//=============================================================================

DS_Protocol2015::DS_Protocol2015() {
    Reset();
    connect (&m_manager, SIGNAL (finished            (QNetworkReply*)),
             this,       SLOT   (ProcessRobotInformation (QNetworkReply*)));
}

//=============================================================================
// DS_Protocol2015::RobotPort
//=============================================================================

int DS_Protocol2015::RobotPort() {
    return 1110;
}

//=============================================================================
// DS_Protocol2015::ClientPort
//=============================================================================

int DS_Protocol2015::ClientPort() {
    return 1150;
}

//=============================================================================
// DS_Protocol2015::DefaultRadioAddress
//=============================================================================

QString DS_Protocol2015::DefaultRadioAddress() {
    return DS_GetStaticIp (Team(), 1);
}

//=============================================================================
// DS_Protocol2015::DefaultRobotAddress
//=============================================================================

QString DS_Protocol2015::DefaultRobotAddress() {
    return QString ("roboRIO-%1.local").arg (Team());
}

//=============================================================================
// DS_Protocol2015::Reboot
//=============================================================================

void DS_Protocol2015::Reboot() {
    UpdateStatus (pStatusRebootRio);
}

//=============================================================================
// DS_Protocol2015::RestartCode
//=============================================================================

void DS_Protocol2015::RestartCode() {
    UpdateStatus (pStatusRestartCode);
}

//=============================================================================
// DS_Protocol2015::ResetProtocol
//=============================================================================

void DS_Protocol2015::ResetProtocol() {
    UpdateStatus (pStatusInvalid);
}

//=============================================================================
// DS_Protocol2015::GetRobotInformation
//=============================================================================

void DS_Protocol2015::GetRobotInformation() {
    QString host = "FTP_://" + RobotAddress();
    m_manager.get (QNetworkRequest (host + LIB_PATH));
    m_manager.get (QNetworkRequest (host + PCM_PATH));
    m_manager.get (QNetworkRequest (host + PDP_PATH));
}

//=============================================================================
// DS_Protocol2015::ReadRobotPacket
//=============================================================================

bool DS_Protocol2015::ReadRobotPacket (QByteArray data) {
    /* Packet length is invalid, watchdog will not be reset */
    if (data.length() < 8)
        return false;

    /* Be sure to reset status bit */
    if (Status() == pStatusInvalid)
        UpdateStatus (pStatusNormal);

    /* Read robot packet */
    int status       = data.at (4);
    int request      = data.at (7);
    int majorVoltage = data.at (5);
    int minorVoltage = data.at (6);

    /* Update client information */
    UpdateVoltage      (majorVoltage, minorVoltage);
    UpdateSendDateTime (request == pProgramRequestTime);
    UpdateRobotCode    ((status & pProgramCodePresent) != 0);

    /* Packet was successfully read, reset watchdog */
    return true;
}

//=============================================================================
// DS_Protocol2015::ProcessRobotInformation
//=============================================================================

void DS_Protocol2015::ProcessRobotInformation (QNetworkReply* reply) {
    QString url = reply->url().toString();
    QString data = QString::fromUtf8 (reply->readAll());

    if (data.isEmpty() || url.isEmpty())
        return;

    else if (url.contains (PCM_PATH, Qt::CaseInsensitive)) {
        QString version;
        QString key = "currentVersion";

        version.append (data.at (data.indexOf (key) + key.length() + 1));
        version.append (data.at (data.indexOf (key) + key.length() + 2));
        version.append (data.at (data.indexOf (key) + key.length() + 3));
        version.append (data.at (data.indexOf (key) + key.length() + 4));

        emit PCMVersionChanged (version);
    }

    else if (url.contains (PDP_PATH, Qt::CaseInsensitive)) {
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
// DS_Protocol2015::GetClientPacket
//=============================================================================

QByteArray DS_Protocol2015::GetClientPacket() {
    QByteArray data;

    int status = IsConnected() ? Status() : pStatusInvalid;
    QByteArray extensions = SendDateTime() ? GetTimezoneData() : GetJoystickData();

    data.append (DS_ToBytes (SentPackets()));     // Ping data
    data.append ((quint8) pHeaderGeneral);        // Protocol version code
    data.append (GetControlCode (ControlMode())); // Operation mode
    data.append (status);                         // Special instructions
    data.append (GetAllianceCode (Alliance()));   // Alliance & position
    data.append (extensions);                     // Joystick data or UTC info

    return data;
}

//=============================================================================
// DS_Protocol2015::GetJoystickData
//=============================================================================

QByteArray DS_Protocol2015::GetJoystickData() {
    QByteArray data;

    /* Do not send JS data on DS init */
    if (SentPackets() <= 5)
        return data;

    /* Generate data for each joystick */
    for (int i = 0; i < Joysticks()->count(); ++i) {
        int numAxes    = Joysticks()->at (i)->numAxes;
        int numButtons = Joysticks()->at (i)->numButtons;
        int numPovHats = Joysticks()->at (i)->numPovHats;

        /* Add joystick information and put the section header */
        data.append (GetJoystickSize (Joysticks()->at (i)) - 1);
        data.append ((quint8) pHeaderJoystick);

        /* Add axis data */
        if (numAxes > 0) {
            data.append (numAxes);
            for (int axis = 0; axis < numAxes; ++axis) {
                double value = Joysticks()->at (i)->axes [axis];
                value *= value > 0 ? 128 : 127;
                data.append ((char) value);
            }
        }

        /* Add button data */
        if (numButtons > 0) {
            int buttons = 0;
            for (int button = 0; button < numButtons; ++button) {
                bool pressed = Joysticks()->at (i)->buttons [button];
                buttons += pressed ? qPow (2, button) : 0;
            }

            data.append (numButtons);
            data.append (DS_ToBytes (buttons));
        }

        /* Add hat/pov data */
        if (numPovHats > 0) {
            data.append (numPovHats);

            for (int hat = 0; hat < numPovHats; ++hat)
                data.append (DS_ToBytes (Joysticks()->at (i)->povHats [hat]));
        }
    }

    return data;
}

//=============================================================================
// DS_Protocol2015::GetTimezoneData
//=============================================================================

QByteArray DS_Protocol2015::GetTimezoneData() {
    QByteArray data;

    /* Add size (always 11) */
    data.append ((quint8) 0x0B);

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
// DS_Protocol2015::GetControlMode
//=============================================================================

DS_ControlMode DS_Protocol2015::GetControlMode (int byte) {
    DS_ControlMode mode = kControlEmergencyStop;

    switch (byte) {
    case pControlDisabled:
        mode = kControlDisabled;
        break;
    case pControlTeleoperated:
        mode = kControlTeleoperated;
        break;
    case pControlTest:
        mode = kControlTest;
        break;
    case pControlAutonomous:
        mode = kControlAutonomous;
        break;
    }

    return mode;
}

//=============================================================================
// DS_Protocol2015::GetControlCode
//=============================================================================

int DS_Protocol2015::GetControlCode (DS_ControlMode mode) {
    quint8 byte = pControlDisabled;

    switch (mode) {
    case kControlTest:
        byte = pControlTest;
        break;
    case kControlTeleoperated:
        byte = pControlTeleoperated;
        break;
    case kControlDisabled:
        byte = pControlDisabled;
        break;
    case kControlAutonomous:
        byte = pControlAutonomous;
        break;
    case kControlEmergencyStop:
        byte = pControlEmergencyStop;
        break;
    }

    return byte;
}

//=============================================================================
// DS_Protocol2015::GetAllianceCode
//=============================================================================

int DS_Protocol2015::GetAllianceCode (DS_Alliance alliance) {
    quint8 byte = pRed1;

    switch (alliance) {
    case kAllianceRed1:
        byte = pRed1;
        break;
    case kAllianceRed2:
        byte = pRed2;
        break;
    case kAllianceRed3:
        byte = pRed3;
        break;
    case kAllianceBlue1:
        byte = pBlue1;
        break;
    case kAllianceBlue2:
        byte = pBlue2;
        break;
    case kAllianceBlue3:
        byte = pBlue3;
        break;
    }

    return byte;
}

//=============================================================================
// DS_Protocol2015::GetJoystickSize
//=============================================================================

int DS_Protocol2015::GetJoystickSize (DS_Joystick* joystick) {
    return  5
            + (joystick->numAxes > 0 ? joystick->numAxes : 0)
            + (joystick->numButtons / 8)
            + (joystick->numButtons % 8 == 0 ? 0 : 1)
            + (joystick->numPovHats > 0 ? joystick->numPovHats * 2 : 0);
}
