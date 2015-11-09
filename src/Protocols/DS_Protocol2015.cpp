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

#include "Protocols/DS_Protocol2015.h"
using namespace DS_Protocol2015_Definitions;

DS_Protocol2015::DS_Protocol2015()
{
    p_minPacketLength = 8;

    reset();
    connect (&m_manager, SIGNAL (finished           (QNetworkReply*)),
             this,       SLOT   (onDownloadFinished (QNetworkReply*)));
}

void DS_Protocol2015::reboot()
{
    m_status = STATUS_REBOOT;
}

int DS_Protocol2015::robotPort()
{
    return 1110;
}

int DS_Protocol2015::clientPort()
{
    return 1150;
}

void DS_Protocol2015::restartCode()
{
    m_status = STATUS_KILL_CODE;
}

QByteArray DS_Protocol2015::getClientPacket()
{
    QByteArray data;

    /* Add ping data */
    p_sentPackets += 1;
    data.append (DS_ToBytes (p_sentPackets));

    /* Add the client header */
    data.append (HEADER_GENERAL);

    /* Add the current control mode */
    data.append (getControlCode (controlMode()));

    /* Add the current status (normal, reboot, etc) */
    data.append (p_robotCommunication ? m_status : (char) STATUS_NULL);

    /* Add the current alliance */
    data.append (getAllianceCode (alliance()));

    /* Add joystick input data */
    if (p_robotCommunication && !m_sendDateTime)
        data.append (generateJoystickData());

    /* Send the timezone data if required */
    else if (m_sendDateTime)
        data.append (generateTimezoneData());

    return data;
}

void DS_Protocol2015::resetProtocol()
{
    m_status = STATUS_NULL;
    m_sendDateTime = false;
}

QString DS_Protocol2015::defaultRadioAddress()
{
    return DS_GetStaticIp (p_team, 1);
}

QString DS_Protocol2015::defaultRobotAddress()
{
    return QString ("roboRIO-%1.local").arg (p_team);
}

void DS_Protocol2015::downloadRobotInformation()
{
    QString host = "ftp://" + robotAddress();
    m_manager.get (QNetworkRequest (host + FTP_LIB_PATH));
    m_manager.get (QNetworkRequest (host + FTP_PCM_PATH));
    m_manager.get (QNetworkRequest (host + FTP_PDP_PATH));
}

QByteArray DS_Protocol2015::generateJoystickData()
{
    QByteArray data;

    /* Generate data for each joystick */
    for (int i = 0; i < p_joysticks->count(); ++i) {
        int numAxes = p_joysticks->at (i)->numAxes;
        int numButtons = p_joysticks->at (i)->numButtons;
        int numPovHats = p_joysticks->at (i)->numPovHats;

        /* Add joystick information and put the section header */
        data.append (getJoystickSize (p_joysticks->at (i)) + 1);
        data.append (HEADER_JOYSTICK);

        /* Add axis data */
        if (numAxes > 0) {
            data.append (numAxes);
            for (int axis = 0; axis < numAxes; ++axis)
                data.append (p_joysticks->at (i)->axes [axis] * 0x7Fu);
        }

        /* Add button data */
        if (numButtons > 0) {
            QBitArray buttons (numButtons);
            for (int button = 0; button < numButtons; ++button)
                buttons [button] = (char) p_joysticks->at (i)->buttons [button];

            data.append (p_joysticks->at (i)->numButtons);
            data.append (bitsToBytes (buttons));
        }

        /* Add hat/pov data */
        if (numPovHats > 0) {
            data.append (numPovHats);
            for (int hat = 0; hat < numPovHats; ++hat) {
                int value = p_joysticks->at (i)->povHats [hat];
                if (value <= 0) value = -1;

                data.append (value);
                data.append (value);
            }
        }
    }

    return data;
}

QByteArray DS_Protocol2015::generateTimezoneData()
{
    QByteArray data;

    /* Add size (always 11) */
    data.append (0x0b);

    /* Get current date/time */
    QDateTime dt = QDateTime::currentDateTime();
    QDate date = dt.date();
    QTime time = dt.time();

    /* Add current date/time */
    data.append (HEADER_TIME);
    data.append (DS_ToBytes (time.msec()));
    data.append (time.second());
    data.append (time.minute());
    data.append (time.hour());
    data.append (date.day());
    data.append (date.month());
    data.append (date.year() - 1900);

    /* Add timezone data */
    data.append (DS_GetTimezoneCode().length() + 1);
    data.append (HEADER_TIMEZONE);
    data.append (DS_GetTimezoneCode());

    return data;
}

void DS_Protocol2015::readRobotData (QByteArray data)
{
    /* We just have connected to the robot */
    if (!p_robotCommunication) {
        p_robotCommunication = true;
        emit communicationsChanged (true);

        downloadRobotInformation();
        setControlMode (DS_ControlDisabled);
    }

    /* Get robot program status */
    if (p_robotCommunication) {
        /* Reset status bit */
        if (m_status == STATUS_NULL)
            m_status = STATUS_OK;

        /* Get robot code value */
        bool code = (data.at (4) != PROGRAM_NO_CODE);
        if (p_robotCode != code) {
            p_robotCode = code;
            emit codeChanged (code);
        }

        /* Know if robot requests timezone to be sent */
        m_sendDateTime = (data.at (7) == PROGRAM_REQUEST_TIME);

        /* Reset the watchdog and update the DS */
        emit packetReceived();
    }

    /* Get robot voltage */
    QString major = QString::number (data.at (5));
    QString minor = QString::number (data.at (6));
    minor = minor.replace ("-", "");

    /* Trim the minor voltage to 2 digits */
    if (minor.length() == 1) minor = minor + "0";
    else if (minor.length() > 2)
        minor = QString (minor.at (0)) + QString (minor.at (1));

    emit voltageChanged (QString ("%1.%2").arg (major, minor));
}

char DS_Protocol2015::getControlCode (DS_ControlMode mode)
{
    switch (mode) {
    case DS_ControlTest:
        return CONTROL_TEST;
        break;
    case DS_ControlTeleOp:
        return CONTROL_TELEOP;
        break;
    case DS_ControlDisabled:
        return CONTROL_DISABLED;
        break;
    case DS_ControlAutonomous:
        return CONTROL_AUTO;
        break;
    case DS_ControlEmergencyStop:
        return CONTROL_ESTOP;
        break;
    case DS_ControlNoCommunication:
        return CONTROL_NO_COMM;
        break;
    }

    return CONTROL_DISABLED;
}

DS_ControlMode DS_Protocol2015::getControlMode (char byte)
{
    switch (byte) {
    case CONTROL_DISABLED:
        return DS_ControlDisabled;
        break;
    case CONTROL_TELEOP:
        return DS_ControlTeleOp;
        break;
    case CONTROL_TEST:
        return DS_ControlTest;
        break;
    case CONTROL_AUTO:
        return DS_ControlAutonomous;
        break;
    case CONTROL_NO_COMM:
        return DS_ControlNoCommunication;
        break;
    }

    return DS_ControlEmergencyStop;
}

char DS_Protocol2015::getAllianceCode (DS_Alliance alliance)
{
    switch (alliance) {
    case DS_AllianceRed1:
        return ALLIANCE_RED1;
        break;
    case DS_AllianceRed2:
        return ALLIANCE_RED2;
        break;
    case DS_AllianceRed3:
        return ALLIANCE_RED3;
        break;
    case DS_AllianceBlue1:
        return ALLIANCE_BLUE1;
        break;
    case DS_AllianceBlue2:
        return ALLIANCE_BLUE2;
        break;
    case DS_AllianceBlue3:
        return ALLIANCE_BLUE3;
        break;
    }

    return ALLIANCE_RED1;
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
    QString url = reply->url().toString();
    QString data = QString::fromUtf8 (reply->readAll());

    if (data.isEmpty() || url.isEmpty())
        return;

    else if (url.contains (FTP_PCM_PATH, Qt::CaseInsensitive)) {
        QString version;
        QString key = "currentVersion";

        version.append (data.at (data.indexOf (key) + key.length() + 1));
        version.append (data.at (data.indexOf (key) + key.length() + 2));
        version.append (data.at (data.indexOf (key) + key.length() + 3));
        version.append (data.at (data.indexOf (key) + key.length() + 4));

        emit pcmVersionChanged (version);
    }

    else if (url.contains (FTP_PDP_PATH, Qt::CaseInsensitive)) {
        QString version;
        QString key = "currentVersion";

        version.append (data.at (data.indexOf (key) + key.length() + 1));
        version.append (data.at (data.indexOf (key) + key.length() + 2));
        version.append (data.at (data.indexOf (key) + key.length() + 3));
        version.append (data.at (data.indexOf (key) + key.length() + 4));

        emit pdpVersionChanged (version);
    }

    else if (url.contains (FTP_LIB_PATH, Qt::CaseInsensitive))
        emit libVersionChanged (data);
}
