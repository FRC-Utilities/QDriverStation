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

/**
 * The values used by the protocol to represent the different operation modes
 */
enum ControlModes {
    pControlTest            = 0x05u, /**< Individual actuators can be moved */
    pControlDisabled        = 0x01u, /**< Robot is idle */
    pControlAutonomous      = 0x06u, /**< Robot takes over the world */
    pControlTeleoperated    = 0x04u, /**< User moves the robot */
    pControlEmergencyStop   = 0x00u, /**< Forced robot stop */
    pControlNoCommunication = 0x02u  /**< Trying to connect with robot... */
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
 * 'Special' operation codes, we should dig more in this section...
 */
enum RobotStatus {
    pStatusNormal           = 0x10u, /**< Normal operation */
    pStatusInvalid          = 0x00u, /**< No communication */
    pStatusRebootRio        = 0x18u, /**< Reboot the roboRIO */
    pStatusRestartCode      = 0x14u  /**< Kill the user code */
};

/**
 * Echo codes from the robot and (probably) software status
 */
enum ProgramTrace {
    pProgramTest            = 0x38u, /**< Control mode confirmation code */
    pProgramNoCode          = 0x00u, /**< Not sure if this is correct */
    pProgramDisabled        = 0x31u, /**< Control mode confirmation code */
    pProgramAutonomous      = 0x30u, /**< Control mode confirmation code */
    pProgramRequestTime     = 0x01u, /**< Robot wants to get the date/time */
    pProgramTeleoperated    = 0x32u  /**< Control mode confirmation code */
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

DS_Protocol2015::DS_Protocol2015() {
    reset();
    connect (&m_manager, SIGNAL (finished           (QNetworkReply*)),
             this,       SLOT   (onDownloadFinished (QNetworkReply*)));
}

quint16 DS_Protocol2015::robotPort() {
    return 1110;
}

quint16 DS_Protocol2015::clientPort() {
    return 1150;
}

QString DS_Protocol2015::defaultRadioAddress() {
    return DS_GetStaticIp (team(), 1);
}

QString DS_Protocol2015::defaultRobotAddress() {
    return QString ("roboRIO-%1.local").arg (team());
}

void DS_Protocol2015::reboot() {
    updateStatus (pStatusRebootRio);
}

void DS_Protocol2015::restartCode() {
    updateStatus (pStatusRestartCode);
}

void DS_Protocol2015::resetProtocol() {
    updateStatus (pStatusInvalid);
}

void DS_Protocol2015::downloadRobotInformation() {
    QString host = "FTP_://" + robotAddress();
    m_manager.get (QNetworkRequest (host + d_LibPath));
    m_manager.get (QNetworkRequest (host + d_PcmPath));
    m_manager.get (QNetworkRequest (host + d_PdpPath));
}

void DS_Protocol2015::readRobotData (QByteArray data) {
    /* We just have connected to the robot */
    if (!isConnected()) {
        downloadRobotInformation();
        updateCommunications (kFull);
        setControlMode (kControlDisabled);
    }

    /* Get robot program status */
    else if (isConnected()) {
        /* Reset status bit if needed */
        if (status() == pStatusInvalid)
            updateStatus (pStatusNormal);

        /* Get robot code value */
        bool code = (data.at (4) != pProgramNoCode);
        if (robotCode() != code)
            updateRobotCode (code);

        /* Know if robot requests timezone to be sent */
        updateSendDateTime (data.at (7) == pProgramRequestTime);

        /* Reset the watchdog and update the DS */
        emit packetReceived();
    }

    /* Get robot voltage */
    updateVoltage (data.at (6), data.at (7));
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

QByteArray DS_Protocol2015::generateClientPacket() {
    QByteArray data;

    /* Add packet index data */
    data.append (sentPackets() / 0xFF);
    data.append (sentPackets() - (sentPackets() / 0xFF));

    /* Add general header data, without it, we are worthless */
    data.append (pHeaderGeneral);

    /* Add the current control mode (e.g TeleOp, Auto, etc) */
    data.append (getControlCode (controlMode()));

    /* Add the current robot status (e.g. normal, reboot, etc) */
    data.append (isConnected() ? status() : pStatusInvalid);

    /* Add the current alliance and position */
    data.append (getAllianceCode (alliance()));

    /* Send joystick data if possible */
    if (isConnected() && !sendDateTime())
        data.append (generateJoystickData());

    /* Robot wants to know what time is it */
    else if (sendDateTime())
        data.append (generateTimezoneData());

    return data;
}

QByteArray DS_Protocol2015::generateJoystickData() {
    QByteArray data;

    /* Generate data for each joystick */
    for (quint8 i = 0; i < joysticks()->count(); ++i) {
        quint8 numAxes = joysticks()->at (i)->numAxes;
        quint8 numButtons = joysticks()->at (i)->numButtons;
        quint8 numPovHats = joysticks()->at (i)->numPovHats;

        /* Add joystick information and put the section header */
        data.append (getJoystickSize (joysticks()->at (i)) + 1);
        data.append (pHeaderJoystick);

        /* Add axis data */
        if (numAxes > 0) {
            data.append (numAxes);
            for (quint8 axis = 0; axis < numAxes; ++axis)
                data.append (joysticks()->at (i)->axes [axis] * 0x80);
        }

        /* Add button data */
        if (numButtons > 0) {
            QBitArray buttons (numButtons);
            for (quint8 button = 0; button < numButtons; ++button)
                buttons [button] = (quint8) joysticks()->at (i)->buttons [button];

            data.append (joysticks()->at (i)->numButtons);
            data.append (bitsToBytes (buttons));
        }

        /* Add hat/pov data */
        if (numPovHats > 0) {
            data.append (numPovHats);
            for (quint8 hat = 0; hat < numPovHats; ++hat) {
                quint8 value = joysticks()->at (i)->povHats [hat];
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

DS_ControlMode DS_Protocol2015::getControlMode (quint8 byte) {
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

quint8 DS_Protocol2015::getControlCode (DS_ControlMode mode) {
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
        return pControlEmergencyStop;
        break;
    case kControlNoCommunication:
        return pControlNoCommunication;
        break;
    }

    return pControlDisabled;
}

quint8 DS_Protocol2015::getAllianceCode (DS_Alliance alliance) {
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

quint8 DS_Protocol2015::getJoystickSize (DS_Joystick* joystick) {
    return  5
            + (joystick->numAxes > 0 ? joystick->numAxes : 0)
            + (joystick->numButtons / 8)
            + (joystick->numButtons % 8 == 0 ? 0 : 1)
            + (joystick->numPovHats > 0 ? joystick->numPovHats * 2 : 0);
}
