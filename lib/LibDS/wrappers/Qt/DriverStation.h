/*
 * Copyright (C) 2015-2016 Alex Spataru <alex_spataru@outlook>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef _DRIVERSTATION_H
#define _DRIVERSTATION_H

#include <QTime>
#include <QObject>
#include <QStringList>
#include <DS_Protocol.h>

class DriverStation : public QObject
{
    Q_OBJECT
    Q_PROPERTY (int joystickCount
                READ joystickCount
                NOTIFY joystickCountChanged())
    Q_PROPERTY (int cpuUsage
                READ cpuUsage
                NOTIFY cpuUsageChanged)
    Q_PROPERTY (int canUsage
                READ canUsage
                NOTIFY canUsageChanged)
    Q_PROPERTY (int ramUsage
                READ ramUsage
                NOTIFY ramUsageChanged)
    Q_PROPERTY (int diskUsage
                READ diskUsage
                NOTIFY diskUsageChanged)
    Q_PROPERTY (int teamNumber
                READ teamNumber
                WRITE setTeamNumber
                NOTIFY teamNumberChanged)
    Q_PROPERTY (bool enabled
                READ isEnabled
                WRITE setEnabled
                NOTIFY enabledChanged)
    Q_PROPERTY (QString status
                READ generalStatus
                NOTIFY statusChanged)
    Q_PROPERTY (float voltage
                READ voltage
                NOTIFY voltageChanged)
    Q_PROPERTY (qreal maximumBatteryVoltage
                READ maximumBatteryVoltage
                NOTIFY protocolChanged)
    Q_PROPERTY (bool robotCode
                READ hasRobotCode
                NOTIFY robotCodeChanged)
    Q_PROPERTY (Control controlMode
                READ controlMode
                WRITE setControlMode
                NOTIFY controlModeChanged)
    Q_PROPERTY (Station station
                READ teamStation
                WRITE setTeamStation
                NOTIFY stationChanged)
    Q_PROPERTY (Alliance alliance
                READ teamAlliance
                WRITE setTeamAlliance
                NOTIFY allianceChanged)
    Q_PROPERTY (Position position
                READ teamPosition
                WRITE setTeamPosition
                NOTIFY positionChanged)
    Q_PROPERTY (bool connectedToFMS
                READ connectedToFMS
                NOTIFY fmsCommunicationsChanged)
    Q_PROPERTY (bool connectedToRadio
                READ connectedToRadio
                NOTIFY radioCommunicationsChanged)
    Q_PROPERTY (bool connectedToRobot
                READ connectedToRobot
                NOTIFY robotCommunicationsChanged)
    Q_PROPERTY (bool emergencyStop
                READ emergencyStopped
                WRITE setEmergencyStopped
                NOTIFY emergencyStoppedChanged)
    Q_PROPERTY (QString voltageString
                READ voltageString
                NOTIFY voltageChanged)
    Q_PROPERTY (QString appliedFMSAddress
                READ appliedFMSAddress
                NOTIFY fmsAddressChanged)
    Q_PROPERTY (QString appliedRadioAddress
                READ appliedRadioAddress
                NOTIFY robotAddressChanged)
    Q_PROPERTY (QString appliedRobotAddress
                READ appliedRobotAddress
                NOTIFY robotAddressChanged)
    Q_PROPERTY (QString defaultFMSAddress
                READ defaultFMSAddress
                NOTIFY fmsAddressChanged)
    Q_PROPERTY (QString defaultRadioAddress
                READ defaultRadioAddress
                NOTIFY robotAddressChanged)
    Q_PROPERTY (QString defaultRobotAddress
                READ defaultRobotAddress
                NOTIFY robotAddressChanged)
    Q_PROPERTY (QString customFMSAddress
                READ customFMSAddress
                WRITE setCustomFMSAddress
                NOTIFY fmsAddressChanged)
    Q_PROPERTY (QString customRadioAddress
                READ customRadioAddress
                WRITE setCustomRadioAddress
                NOTIFY robotAddressChanged)
    Q_PROPERTY (QString customRobotAddress
                READ customRobotAddress
                WRITE setCustomRobotAddress
                NOTIFY robotAddressChanged)
    Q_PROPERTY (QString elapsedTime
                READ elapsedTime
                NOTIFY elapsedTimeChanged)
    Q_PROPERTY (QStringList stations
                READ stations
                CONSTANT)
    Q_PROPERTY (QStringList protocols
                READ protocols
                CONSTANT)
    Q_PROPERTY (int fmsPacketLoss
                READ fmsPacketLoss)
    Q_PROPERTY (int radioPacketLoss
                READ radioPacketLoss)
    Q_PROPERTY (int robotPacketLoss
                READ robotPacketLoss)
    Q_PROPERTY (bool isTestMode
                READ isTestMode
                NOTIFY controlModeChanged)
    Q_PROPERTY (bool isAutonomous
                READ isAutonomous
                NOTIFY controlModeChanged)
    Q_PROPERTY (bool isTeleoperated
                READ isTeleoperated
                NOTIFY controlModeChanged)
    Q_PROPERTY (bool canBeEnabled
                READ canBeEnabled)

public:
    static DriverStation* getInstance();

    enum Control {
        kControlTest = 0x00,
        kControlAutonomous = 0x01,
        kControlTeleoperated = 0x02,
    };
    Q_ENUMS (Control)

    enum Protocol {
        kProtocol2016 = 0x00,
        kProtocol2015 = 0x01,
        kProtocol2014 = 0x02,
    };
    Q_ENUMS (Protocol)

    enum Alliance {
        kAllianceRed = 0x00,
        kAllianceBlue = 0x01,
    };
    Q_ENUMS (Alliance)

    enum Position {
        kPosition1 = 0x00,
        kPosition2 = 0x01,
        kPosition3 = 0x02,
    };
    Q_ENUMS (Position)

    enum Station {
        kStationRed1 = 0x00,
        kStationRed2 = 0x01,
        kStationRed3 = 0x02,
        kStationBlue1 = 0x03,
        kStationBlue2 = 0x04,
        kStationBlue3 = 0x05,
    };
    Q_ENUMS (Station)

    int teamNumber() const;
    int joystickCount() const;

    int cpuUsage() const;
    int canUsage() const;
    int ramUsage() const;
    int diskUsage() const;

    int fmsPacketLoss() const;
    int radioPacketLoss() const;
    int robotPacketLoss() const;

    bool isEnabled() const;
    bool isTestMode() const;
    bool canBeEnabled() const;
    bool hasRobotCode() const;
    bool isAutonomous() const;
    bool isTeleoperated() const;
    bool connectedToFMS() const;
    bool connectedToRadio() const;
    bool connectedToRobot() const;
    bool emergencyStopped() const;

    qreal voltage() const;
    QString voltageString() const;
    qreal maximumBatteryVoltage() const;

    Control controlMode() const;
    Station teamStation() const;
    Alliance teamAlliance() const;
    Position teamPosition() const;

    QString appliedFMSAddress() const;
    QString appliedRadioAddress() const;
    QString appliedRobotAddress() const;

    QString defaultFMSAddress() const;
    QString defaultRadioAddress() const;
    QString defaultRobotAddress() const;

    QString elapsedTime();
    QString generalStatus() const;
    QString customFMSAddress() const;
    QString customRadioAddress() const;
    QString customRobotAddress() const;

    QStringList stations() const;
    QStringList protocols() const;

    Q_INVOKABLE int getNumAxes (const int joystick) const;
    Q_INVOKABLE int getNumHats (const int joystick) const;
    Q_INVOKABLE int getNumButtons (const int joystick) const;

public slots:
    void start();
    void rebootRobot();
    void resetJoysticks();
    void restartRobotCode();
    void setEnabled (const bool enabled);
    void setTeamNumber (const int number);
    void loadProtocol (DS_Protocol* protocol);
    void setControlMode (const Control mode);
    void setProtocol (const Protocol protocol);
    void setTeamStation (const Station station);
    void setTeamAlliance (const Alliance alliance);
    void setTeamPosition (const Position position);
    void setEmergencyStopped (const bool stopped);
    void setCustomFMSAddress (const QString& address);
    void setCustomRadioAddress (const QString& address);
    void setCustomRobotAddress (const QString& address);
    void sendNetConsoleMessage (const QString& message);

    void addJoystick (int axes, int hats, int buttons);
    void setJoystickHat (int joystick, int hat, int angle);
    void setJoystickAxis (int joystick, int axis, float value);
    void setJoystickButton (int joystick, int button, bool pressed);

private slots:
    void quitDS();
    void processEvents();
    void resetElapsedTime();
    void updateElapsedTime();

private:
    bool addressIsValid (const QString& address);

signals:
    void stationChanged();
    void protocolChanged();
    void fmsAddressChanged();
    void radioAddressChanged();
    void robotAddressChanged();
    void joystickCountChanged();
    void canUsageChanged (const int usage);
    void cpuUsageChanged (const int usage);
    void ramUsageChanged (const int usage);
    void diskUsageChanged (const int usage);
    void enabledChanged (const bool enabled);
    void newMessage (const QString& message);
    void teamNumberChanged (const int number);
    void statusChanged (const QString& status);
    void voltageChanged (const float voltage);
    void robotCodeChanged (const bool robotCode);
    void controlModeChanged (const Control mode);
    void allianceChanged (const Alliance alliance);
    void positionChanged (const Position position);
    void elapsedTimeChanged (const QString& string);
    void fmsCommunicationsChanged (const bool connected);
    void radioCommunicationsChanged (const bool connected);
    void robotCommunicationsChanged (const bool connected);
    void emergencyStoppedChanged (const bool emergencyStopped);

private:
    QTime m_time;
    QString m_elapsedTime;
};

#endif
