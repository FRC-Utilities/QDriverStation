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

#include "../headers/DS_Protocol2015.h"

const QString _PCM_FILE ("/tmp/frc_versions/PCM-0-versions.ini");
const QString _PDP_FILE ("/tmp/frc_versions/PDP-0-versions.ini");
const QString _LIB_FILE ("/tmp/frc_versions/FRC_Lib_Version.ini");

DS_Protocol2015::DS_Protocol2015()
{
    reset();
    connect (&m_manager, SIGNAL (finished (QNetworkReply*)),
             this,       SLOT   (onDownloadFinished (QNetworkReply*)));
}

void DS_Protocol2015::reset()
{
    m_index = 0;
    p_robotCode = false;
    m_justConnected = false;
    m_status = RobotStatus::Normal;
    emit codeChanged (p_robotCode);
    setControlMode (DS_ControlDisabled);
}

void DS_Protocol2015::reboot()
{
    m_status = RobotStatus::RebootRobot;
}

int DS_Protocol2015::robotPort()
{
    return Ports::RobotPort;
}

int DS_Protocol2015::clientPort()
{
    return Ports::ClientPort;
}

void DS_Protocol2015::restartCode()
{
    m_status = RobotStatus::RestartCode;
}

QString DS_Protocol2015::robotAddress()
{
    if (p_robotAddress.isEmpty())
        return QString ("roboRIO-%1.local").arg (p_team);

    return p_robotAddress;
}

QString DS_Protocol2015::radioAddress()
{
    if (p_radioAddress.isEmpty())
        return DS_GetStaticIp (p_team, 1);

    return p_radioAddress;
}

void DS_Protocol2015::downloadRobotInformation()
{
    QString host = "ftp://" + robotAddress();
    m_manager.get (QNetworkRequest (host + _LIB_FILE));
    m_manager.get (QNetworkRequest (host + _PCM_FILE));
    m_manager.get (QNetworkRequest (host + _PDP_FILE));
}

QByteArray DS_Protocol2015::generateClientPacket()
{
    /* Generate ping index */
    m_index += 1;
    if (m_index >= 0xffff)
        m_index = 0;

    /* Generate the ping data */
    QByteArray data;
    DS_PingData ping;
    ping.generatePingData (m_index);

    /* Add ping data */
    data.append (ping.byte1);
    data.append (ping.byte2);

    /* Add the section header */
    data.append (SectionHeaders::GeneralHeader);

    /* Add the desired control mode, robot status and alliance data */
    data.append (getControlCode (controlMode()));
    data.append (m_status);
    data.append (getAllianceCode (alliance()));

    /* Add joystick input information if the robot is in TeleOp */
    if (controlMode() == DS_ControlTeleOp)
        data.append (generateJoystickData());

    return data;
}

QByteArray DS_Protocol2015::generateJoystickData()
{
    QByteArray data;

    for (int i = 0; i < p_joysticks->count(); ++i)
    {
        int numAxes = p_joysticks->at (i)->numAxes;
        int numButtons = p_joysticks->at (i)->numButtons;
        int numPovHats = p_joysticks->at (i)->numPovHats;

        data.append ((uint8_t) getJoystickSize (p_joysticks->at (i)) + 1);
        data.append ((uint8_t) SectionHeaders::JoystickHeader);

        /* Add axis data */
        if (numAxes > 0)
        {
            data.append ((uint8_t) numAxes);
            for (int axis = 0; axis < numAxes; ++axis)
                data.append ((uint8_t)
                             (p_joysticks->at (i)->axes [axis] * (0xff / 2)));
        }

        /* Add button data */
        if (numButtons > 0)
        {
            QBitArray buttons ((uint8_t) numButtons);
            for (int button = 0; button < numButtons; ++button)
                buttons [button] = p_joysticks->at (i)->buttons [button];

            data.append ((uint8_t) p_joysticks->at (i)->numButtons);
            data.append (bitsToBytes (buttons));
        }

        /* Add hat/pov data */
        if (numPovHats > 0)
        {
            data.append ((uint8_t) numPovHats);
            for (int hat = 0; hat < numPovHats; ++hat)
            {
                int value = p_joysticks->at (i)->povHats [hat];

                if (value <= 0)
                    value = -1;

                data.append ((uint8_t) value);
                data.append ((uint8_t) value);
            }
        }
    }

    return data;
}

void DS_Protocol2015::readRobotData (QByteArray data)
{
    /* Get robot voltage */
    QString major = QString::number (data.at (5));
    QString minor = QString::number (data.at (6));
    minor = minor.replace ("-", "");

    if (minor.length() > 2)
        minor = QString ("%1%2").arg (minor.at (0), minor.at (1));

    emit voltageChanged (QString ("%1.%2").arg (major, minor));

    /* Get robot code */
    bool code = data.at (4) != ProgramStatus::NoProgram;
    if (p_robotCode != code)
    {
        p_robotCode = code;
        emit codeChanged (code);
    }

    /* We are sure that we are talking with a robot, download its info */
    if (!m_justConnected)
    {
        m_justConnected = true;
        downloadRobotInformation();
    }
}

char DS_Protocol2015::getControlCode (DS_ControlMode mode)
{
    switch (mode)
    {
    case DS_ControlTest:
        return OperationModes::Test;
        break;
    case DS_ControlTeleOp:
        return OperationModes::TeleOperated;
        break;
    case DS_ControlDisabled:
        return OperationModes::Disabled;
        break;
    case DS_ControlAutonomous:
        return OperationModes::Autonomous;
        break;
    case DS_ControlEmergencyStop:
        return OperationModes::EmergencyStop;
        break;
    }

    DS_ERROR ("Invalid operation mode" << mode);
    DS_DEBUG ("Using" << OperationModes::Disabled);

    return OperationModes::Disabled;
}

DS_ControlMode DS_Protocol2015::getControlMode (char mode)
{
    switch (mode)
    {
    case OperationModes::Test:
        return DS_ControlTest;
        break;
    case OperationModes::Disabled:
        return DS_ControlDisabled;
        break;
    case OperationModes::Autonomous:
        return DS_ControlAutonomous;
        break;
    case OperationModes::TeleOperated:
        return DS_ControlTeleOp;
        break;
    }

    return DS_ControlEmergencyStop;
}

char DS_Protocol2015::getAllianceCode (DS_Alliance alliance)
{
    switch (alliance)
    {
    case DS_AllianceRed1:
        return Alliances::Red1;
        break;
    case DS_AllianceRed2:
        return Alliances::Red2;
        break;
    case DS_AllianceRed3:
        return Alliances::Red3;
        break;
    case DS_AllianceBlue1:
        return Alliances::Blue1;
        break;
    case DS_AllianceBlue2:
        return Alliances::Blue2;
        break;
    case DS_AllianceBlue3:
        return Alliances::Blue3;
        break;
    }

    DS_ERROR ("Invalid alliance" << alliance);
    DS_DEBUG ("Using" << Alliances::Red1);

    return Alliances::Red1;
}

int DS_Protocol2015::getJoystickSize (DS_Joystick* joystick)
{
    return  5
            + (joystick->numAxes > 0 ? joystick->numAxes : 0)
            + (joystick->numButtons / 8)
            + (joystick->numButtons % 8 == 0 ? 0 : 1)
            + (joystick->numPovHats > 0 ? joystick->numPovHats * 2 : 0);
}

void DS_Protocol2015::onDownloadFinished (QNetworkReply* reply)
{
    /* Get URL and downloaded data */
    QString url = reply->url().toString();
    QString data = QString::fromUtf8 (reply->readAll());

    /* Data or URL is invalid */
    if (data.isEmpty() || url.isEmpty())
        return;

    /* This is the PCM information file */
    else if (url.contains (_PCM_FILE, Qt::CaseInsensitive))
    {
        QString version;
        QString key = "currentVersion";

        version.append (data.at (data.indexOf (key) + key.length() + 1));
        version.append (data.at (data.indexOf (key) + key.length() + 2));
        version.append (data.at (data.indexOf (key) + key.length() + 3));
        version.append (data.at (data.indexOf (key) + key.length() + 4));

        emit pcmVersionChanged (version);
    }

    /* This is the PDP information file */
    else if (url.contains (_PDP_FILE, Qt::CaseInsensitive))
    {
        QString version;
        QString key = "currentVersion";

        version.append (data.at (data.indexOf (key) + key.length() + 1));
        version.append (data.at (data.indexOf (key) + key.length() + 2));
        version.append (data.at (data.indexOf (key) + key.length() + 3));
        version.append (data.at (data.indexOf (key) + key.length() + 4));

        emit pdpVersionChanged (version);
    }

    /* This is the library version file */
    else if (url.contains (_LIB_FILE, Qt::CaseInsensitive))
        emit libVersionChanged (data);
}
