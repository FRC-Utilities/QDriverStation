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

#define d_PcmPath "/tmp/frc_versions/PCM-0-versions.ini"
#define d_PdpPath "/tmp/frc_versions/PDP-0-versions.ini"
#define d_LibPath "/tmp/frc_versions/FRC_Lib_Version.ini"

enum ControlModes {
    pControlTest = 0x05,
    pControlDisabled = 0x01,
    pControlAutonomous = 0x06,
    pControlTeleoperated = 0x04,
    pControlEmmergencyStop = 0x80,
    pControlNoCommunication = 0x02
};

enum Headers {
    pHeaderTime = 0x0f,
    pHeaderGeneral = 0x01,
    pHeaderJoystick = 0x0c,
    pHeaderTimezone = 0x10
};

enum RobotStatus {
    pStatusNormal = 0x10,
    pStatusInvalid = 0x00,
    pStatusRebootRio = 0x18,
    pStatusRestartCode = 0x14
};

enum ProgramStatus {
    pProgramTest = 0x38,
    pProgramNoCode = 0x00,
    pProgramDisabled = 0x31,
    pProgramAutonomous = 0x30,
    pProgramRequestTime = 0x01,
    pProgramTeleoperated = 0x32
};

enum Alliances {
    pRed1 = 0x00,
    pRed2 = 0x01,
    pRed3 = 0x03,
    pBlue1 = 0x04,
    pBlue2 = 0x05,
    pBlue3 = 0x06
};

DS_Protocol2015::DS_Protocol2015() {
    p_minPacketLength = 8;

    reset();
    connect (&m_manager, SIGNAL (finished           (QNetworkReply*)),
             this,       SLOT   (onDownloadFinished (QNetworkReply*)));
}

void DS_Protocol2015::reboot() {
    p_status = pStatusRebootRio;
}

void DS_Protocol2015::restartCode() {
    p_status = pStatusRestartCode;
}

int DS_Protocol2015::robotPort() {
    return 1110;
}

int DS_Protocol2015::clientPort() {
    return 1150;
}

QByteArray DS_Protocol2015::getClientPacket() {
    QByteArray data;

    /* Add ping data */
    p_sentPackets += 1;
    data.append (DS_ToBytes (p_sentPackets));

    /* Add the client header */
    data.append (pHeaderGeneral);

    /* Add the current control mode */
    data.append (getControlCode (controlMode()));

    /* Add the current status (normal, reboot, etc) */
    data.append (p_robotCommunication ? p_status : (char) pStatusInvalid);

    /* Add the current alliance */
    data.append (getAllianceCode (alliance()));

    /* Add joystick input data */
    if (p_robotCommunication && !p_sendDateTime)
        data.append (generateJoystickData());

    /* Send the timezone data if required */
    else if (p_sendDateTime)
        data.append (generateTimezoneData());

    return data;
}

void DS_Protocol2015::resetProtocol() {
    p_status = pStatusInvalid;
}

void DS_Protocol2015::downloadRobotInformation() {
    QString host = "FTP_://" + robotAddress();
    m_manager.get (QNetworkRequest (host + d_LibPath));
    m_manager.get (QNetworkRequest (host + d_PcmPath));
    m_manager.get (QNetworkRequest (host + d_PdpPath));
}

void DS_Protocol2015::readRobotData (QByteArray data) {
    /* We just have connected to the robot */
    if (!p_robotCommunication) {
        p_robotCommunication = true;
        emit communicationsChanged (true);

        downloadRobotInformation();
        setControlMode (kControlDisabled);
    }

    /* Get robot program status */
    if (p_robotCommunication) {
        /* Reset status bit */
        if (p_status == pStatusInvalid)
            p_status = pStatusNormal;

        /* Get robot code value */
        bool code = (data.at (4) != pProgramNoCode);
        if (p_robotCode != code) {
            p_robotCode = code;
            emit codeChanged (code);
        }

        /* Know if robot requests timezone to be sent */
        p_sendDateTime = (data.at (7) == pProgramRequestTime);

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

void DS_Protocol2015::onDownloadFinished (QNetworkReply* reply) {
    QString url = reply->url().toString();
    QString data = QString::fromUtf8 (reply->readAll());

    if (data.isEmpty() || url.isEmpty())
        return;

    else if (url.contains (d_PcmPath, Qt::CaseInsensitive)) {
        QString version;
        QString key = "currentVersion";

        version.append (data.at (data.indexOf (key) + key.length() + 1));
        version.append (data.at (data.indexOf (key) + key.length() + 2));
        version.append (data.at (data.indexOf (key) + key.length() + 3));
        version.append (data.at (data.indexOf (key) + key.length() + 4));

        emit pcmVersionChanged (version);
    }

    else if (url.contains (d_PdpPath, Qt::CaseInsensitive)) {
        QString version;
        QString key = "currentVersion";

        version.append (data.at (data.indexOf (key) + key.length() + 1));
        version.append (data.at (data.indexOf (key) + key.length() + 2));
        version.append (data.at (data.indexOf (key) + key.length() + 3));
        version.append (data.at (data.indexOf (key) + key.length() + 4));

        emit pdpVersionChanged (version);
    }

    else if (url.contains (d_LibPath, Qt::CaseInsensitive))
        emit libVersionChanged (data);

    delete reply;
}

QString DS_Protocol2015::defaultRadioAddress() {
    return DS_GetStaticIp (p_team, 1);
}

QString DS_Protocol2015::defaultRobotAddress() {
    return QString ("roboRIO-%1.local").arg (p_team);
}

QByteArray DS_Protocol2015::generateJoystickData() {
    QByteArray data;

    /* Generate data for each joystick */
    for (int i = 0; i < p_joysticks->count(); ++i) {
        int numAxes = p_joysticks->at (i)->numAxes;
        int numButtons = p_joysticks->at (i)->numButtons;
        int numPovHats = p_joysticks->at (i)->numPovHats;

        /* Add joystick information and put the section header */
        data.append (getJoystickSize (p_joysticks->at (i)) + 1);
        data.append (pHeaderJoystick);

        /* Add axis data */
        if (numAxes > 0) {
            data.append (numAxes);
            for (int axis = 0; axis < numAxes; ++axis)
                data.append (p_joysticks->at (i)->axes [axis] * 0x80);
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

QByteArray DS_Protocol2015::generateTimezoneData() {
    QByteArray data;

    /* Add size (always 11) */
    data.append (0x0b);

    /* Get current date/time */
    QDateTime dt = QDateTime::currentDateTime();
    QDate date = dt.date();
    QTime time = dt.time();

    /* Add current date/time */
    data.append (pHeaderTime);
    data.append (DS_ToBytes (time.msec()));
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

int DS_Protocol2015::getControlCode (DS_ControlMode mode) {
    switch (mode) {
    case kControlTest:
        return pControlTest;
        break;
    case kControlTeleoperated:
        return pControlTeleoperated;
        break;
    case kControlDisabled:
        return pControlDisabled;
        break;
    case kControlAutonomous:
        return pControlAutonomous;
        break;
    case kControlEmergencyStop:
        return pControlEmmergencyStop;
        break;
    case kControlNoCommunication:
        return pControlNoCommunication;
        break;
    }

    return pControlDisabled;
}

DS_ControlMode DS_Protocol2015::getControlMode (int byte) {
    switch (byte) {
    case pControlDisabled:
        return kControlDisabled;
        break;
    case pControlTeleoperated:
        return kControlTeleoperated;
        break;
    case pControlTest:
        return kControlTest;
        break;
    case pControlAutonomous:
        return kControlAutonomous;
        break;
    case pControlNoCommunication:
        return kControlNoCommunication;
        break;
    }

    return kControlEmergencyStop;
}

int DS_Protocol2015::getAllianceCode (DS_Alliance alliance) {
    switch (alliance) {
    case kAllianceRed1:
        return pRed1;
        break;
    case kAllianceRed2:
        return pRed2;
        break;
    case kAllianceRed3:
        return pRed3;
        break;
    case kAllianceBlue1:
        return pBlue1;
        break;
    case kAllianceBlue2:
        return pBlue2;
        break;
    case kAllianceBlue3:
        return pBlue3;
        break;
    }

    return pRed1;
}

int DS_Protocol2015::getJoystickSize (DS_Joystick* joystick) {
    return  5
            + (joystick->numAxes > 0 ? joystick->numAxes : 0)
            + (joystick->numButtons / 8)
            + (joystick->numButtons % 8 == 0 ? 0 : 1)
            + (joystick->numPovHats > 0 ? joystick->numPovHats * 2 : 0);
}
