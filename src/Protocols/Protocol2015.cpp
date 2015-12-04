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

DS_Protocol2015::DS_Protocol2015() {
    reset();
    connect (&m_manager, SIGNAL (finished           (QNetworkReply*)),
             this,       SLOT   (onDownloadFinished (QNetworkReply*)));
}

int DS_Protocol2015::robotPort() {
    return 1110;
}

int DS_Protocol2015::clientPort() {
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
    /* Packet length is invalid */
    if (data.length() < 8)
        return;

    /* We just have connected to the robot, update internal values */
    if (!isConnected()) {
        downloadRobotInformation();
        updateCommunications (kFull);
        setControlMode (kControlDisabled);

        if (status() == pStatusInvalid)
            updateStatus (pStatusNormal);
    }

    /* Read robot packet */
    int status       = data.at (4);
    int request      = data.at (7);
    int majorVoltage = data.at (5);
    int minorVoltage = data.at (6);

    /* Update client information */
    updateVoltage (majorVoltage, minorVoltage);
    updateSendDateTime (request == pProgramRequestTime);
    updateRobotCode ((status & pProgramCodePresent) != 0);

    /* Reset watchdog */
    emit packetReceived();
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
    data.append ((quint8) pHeaderGeneral);

    /* Add the current control mode (e.g TeleOp, Auto, etc) */
    data.append (getControlCode (controlMode()));

    /* Add the current robot status (e.g. normal, reboot, etc) */
    data.append (isConnected() ? status() : (int) pStatusInvalid);

    /* Add the current alliance and position */
    data.append (getAllianceCode (alliance()));

    /* Send joystick data or timezone code */
    data.append (sendDateTime() ? generateTimezoneData() : generateJoystickData());

    return data;
}

QByteArray DS_Protocol2015::generateJoystickData() {
    QByteArray data;

    /* Do not send JS data on DS init */
    if (sentPackets() <= 5)
        return data;

    /* Generate data for each joystick */
    for (int i = 0; i < joysticks()->count(); ++i) {
        int numAxes    = joysticks()->at (i)->numAxes;
        int numButtons = joysticks()->at (i)->numButtons;
        int numPovHats = joysticks()->at (i)->numPovHats;

        /* Add joystick information and put the section header */
        data.append (getJoystickSize (joysticks()->at (i)) + 1);
        data.append ((quint8) pHeaderJoystick);

        /* Add axis data */
        if (numAxes > 0) {
            data.append (numAxes);
            for (int axis = 0; axis < numAxes; ++axis)
                data.append (joysticks()->at (i)->axes [axis] * 0x80);
        }

        /* Add button data */
        if (numButtons > 0) {
            QBitArray buttons (numButtons);
            for (int button = 0; button < numButtons; ++button)
                buttons.setBit (button, joysticks()->at (i)->buttons [button]);

            data.append (joysticks()->at (i)->numButtons);
            data.append (bitsToBytes (buttons));
        }

        /* Add hat/pov data */
        if (numPovHats > 0) {
            data.append (numPovHats);
            for (int hat = 0; hat < numPovHats; ++hat) {
                int value = joysticks()->at (i)->povHats [hat];
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
    }

    return kControlEmergencyStop;
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
        return pControlEmergencyStop;
        break;
    }

    return pControlDisabled;
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
