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

#define CONTROL_DISABLED 0x00
#define CONTROL_TELEOP   0x04
#define CONTROL_TEST     0x05
#define CONTROL_AUTO     0x06
#define CONTROL_ESTOP    0x80

#define SECTION_GENERAL  0x01
#define SECTION_JOYSTICK 0x0C

#define STATUS_OK        0x10
#define STATUS_REBOOT    0x18
#define STATUS_KILL_CODE 0x14

#define PROGRAM_DISABLED 0x01
#define PROGRAM_TELEOP   0x02
#define PROGRAM_AUTO     0x04
#define PROGRAM_TEST     0x08
#define PROGRAM_ROBORIO  0x10
#define PROGRAM_USERCODE 0x20
#define PROGRAM_NO_CODE  0x00

#define ALLIANCE_RED1    0x00
#define ALLIANCE_RED2    0x01
#define ALLIANCE_RED3    0x02
#define ALLIANCE_BLUE1   0x03
#define ALLIANCE_BLUE2   0x04
#define ALLIANCE_BLUE3   0x05

#define PORT_ROBOT       0x456
#define PORT_CLIENT      0x47E
#define MAX_PING_SIZE    0xFFFF
#define DOUBLE_TO_CHAR   0x7F

#define FTP_PCM          "/tmp/frc_versions/PCM-0-versions.ini"
#define FTP_PDP          "/tmp/frc_versions/PDP-0-versions.ini"
#define FTP_LIB          "/tmp/frc_versions/FRC_Lib_Version.ini"

DS_Protocol2015::DS_Protocol2015() {
    reset();
    connect (&m_manager, SIGNAL (finished           (QNetworkReply*)),
             this,       SLOT   (onDownloadFinished (QNetworkReply*)));
}

void DS_Protocol2015::reset() {
    m_index = 0;
    p_robotCode = false;
    m_status = STATUS_OK;
    p_robotCommunication = false;

    emit codeChanged (p_robotCode);
    emit communicationsChanged (p_robotCommunication);

    setControlMode (DS_ControlDisabled);
}

void DS_Protocol2015::reboot() {
    m_status = STATUS_REBOOT;
}

int DS_Protocol2015::robotPort() {
    return PORT_ROBOT;
}

int DS_Protocol2015::clientPort() {
    return PORT_CLIENT;
}

void DS_Protocol2015::restartCode() {
    m_status = STATUS_KILL_CODE;
}

QString DS_Protocol2015::robotAddress() {
    if (p_robotAddress.isEmpty())
        return QString ("roboRIO-%1.local").arg (p_team);

    return p_robotAddress;
}

QString DS_Protocol2015::radioAddress() {
    if (p_radioAddress.isEmpty())
        return DS_GetStaticIp (p_team, 1);

    return p_radioAddress;
}

void DS_Protocol2015::downloadRobotInformation() {
    QString host = "ftp://" + robotAddress();
    m_manager.get (QNetworkRequest (host + FTP_LIB));
    m_manager.get (QNetworkRequest (host + FTP_PCM));
    m_manager.get (QNetworkRequest (host + FTP_PDP));
}

QByteArray DS_Protocol2015::generateClientPacket() {
    /* Generate ping index */
    m_index += 1;
    if (m_index >= MAX_PING_SIZE)
        m_index = 0;

    /* Generate the ping data */
    QByteArray data;
    DS_PingData ping;
    ping.generatePingData (m_index);

    /* Add ping data */
    data.append ((uint8_t) ping.byte1);
    data.append ((uint8_t) ping.byte2);

    /* Add the section header */
    data.append ((uint8_t) SECTION_GENERAL);

    /* Add the desired control mode, robot status and alliance data */
    data.append ((uint8_t) getControlCode (controlMode()));
    data.append ((uint8_t) m_status);
    data.append ((uint8_t) getAllianceCode (alliance()));

    /* Add joystick input information if the robot is in TeleOp */
    if (controlMode() == DS_ControlTeleOp)
        data.append (generateJoystickData());

    return data;
}

QByteArray DS_Protocol2015::generateJoystickData() {
    QByteArray data;

    for (int i = 0; i < p_joysticks->count(); ++i) {
        int numAxes = p_joysticks->at (i)->numAxes;
        int numButtons = p_joysticks->at (i)->numButtons;
        int numPovHats = p_joysticks->at (i)->numPovHats;

        data.append ((uint8_t) getJoystickSize (p_joysticks->at (i)) + 1);
        data.append ((uint8_t) SECTION_JOYSTICK);

        /* Add axis data */
        if (numAxes > 0) {
            data.append ((uint8_t) numAxes);
            for (int axis = 0; axis < numAxes; ++axis)
                data.append (p_joysticks->at (i)->axes [axis] * DOUBLE_TO_CHAR);
        }

        /* Add button data */
        if (numButtons > 0) {
            int extraBit = (numButtons % 8 == 0) ? 0 : 1;
            int byteCount = (numButtons / 8) + extraBit;

            QBitArray buttons ((uint8_t) numButtons + extraBit);
            for (int button = 0; button < numButtons; ++button)
                buttons [button] = p_joysticks->at (i)->buttons [button];

            buttons.setBit (buttons.count() - 1, byteCount * 8);

            data.append ((uint8_t) p_joysticks->at (i)->numButtons);
            data.append (bitsToBytes (buttons));
        }

        /* Add hat/pov data */
        if (numPovHats > 0) {
            data.append ((uint8_t) numPovHats);
            for (int hat = 0; hat < numPovHats; ++hat) {
                int value = p_joysticks->at (i)->povHats [hat];
                if (value <= 0) value = -1;

                data.append ((uint8_t) value);
                data.append ((uint8_t) value);
            }
        }
    }

    return data;
}

void DS_Protocol2015::readRobotData (QByteArray data) {
    /* Restart the watch dog and udpate internal values */
    restartWatchdog();

    /* Do not try to read an invalid robot packet */
    if (data.isEmpty() || data.length() < 8)
        return;

    /* Update the status of the communications and download robot information */
    if (!p_robotCommunication) {
        p_robotCommunication = true;
        downloadRobotInformation();
        emit communicationsChanged (true);
    }

    /* Get robot voltage */
    QString major = QString::number (data.at (5));
    QString minor = QString::number (data.at (6));
    minor = minor.replace ("-", "");

    if (minor.length() > 2)
        minor = QString ("%1%2").arg (minor.at (0), minor.at (1));

    emit voltageChanged (QString ("%1.%2").arg (major, minor));

    /* Get robot code */
    bool code = data.at (4) != PROGRAM_NO_CODE;
    if (p_robotCode != code) {
        p_robotCode = code;
        emit codeChanged (code);
    }
}

char DS_Protocol2015::getControlCode (DS_ControlMode mode) {
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
    }

    return CONTROL_DISABLED;
}

DS_ControlMode DS_Protocol2015::getControlMode (char byte) {
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
    case CONTROL_ESTOP:
        return DS_ControlEmergencyStop;
        break;
    }

    return DS_ControlDisabled;
}

char DS_Protocol2015::getAllianceCode (DS_Alliance alliance) {
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

int DS_Protocol2015::getJoystickSize (DS_Joystick* joystick) {
    return  5
            + (joystick->numAxes > 0 ? joystick->numAxes : 0)
            + (joystick->numButtons / 8)
            + (joystick->numButtons % 8 == 0 ? 0 : 1)
            + (joystick->numPovHats > 0 ? joystick->numPovHats * 2 : 0);
}

void DS_Protocol2015::onDownloadFinished (QNetworkReply* reply) {
    /* Get URL and downloaded data */
    QString url = reply->url().toString();
    QString data = QString::fromUtf8 (reply->readAll());

    /* Data or URL is invalid */
    if (data.isEmpty() || url.isEmpty())
        return;

    /* This is the PCM information file */
    else if (url.contains (FTP_PCM, Qt::CaseInsensitive)) {
        QString version;
        QString key = "currentVersion";

        version.append (data.at (data.indexOf (key) + key.length() + 1));
        version.append (data.at (data.indexOf (key) + key.length() + 2));
        version.append (data.at (data.indexOf (key) + key.length() + 3));
        version.append (data.at (data.indexOf (key) + key.length() + 4));

        emit pcmVersionChanged (version);
    }

    /* This is the PDP information file */
    else if (url.contains (FTP_PDP, Qt::CaseInsensitive)) {
        QString version;
        QString key = "currentVersion";

        version.append (data.at (data.indexOf (key) + key.length() + 1));
        version.append (data.at (data.indexOf (key) + key.length() + 2));
        version.append (data.at (data.indexOf (key) + key.length() + 3));
        version.append (data.at (data.indexOf (key) + key.length() + 4));

        emit pdpVersionChanged (version);
    }

    /* This is the library version file */
    else if (url.contains (FTP_LIB, Qt::CaseInsensitive))
        emit libVersionChanged (data);
}
