/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#ifndef _LIB_DS_DRIVERSTATION_H
#define _LIB_DS_DRIVERSTATION_H

#include <Core/DS_Base.h>

class Sockets;
class Watchdog;
class Protocol;
class DS_Config;
class NetConsole;

class DriverStation : public DS_Base
{
    Q_OBJECT
    Q_ENUMS (ProtocolType)
    Q_ENUMS (TeamStation)

public:
    static DriverStation* getInstance();

    enum ProtocolType {
        kFRC2016 = 0,
        kFRC2015 = 1,
        kFRC2014 = 2,
    };

    enum TeamStation {
        kRed1  = 0,
        kRed2  = 1,
        kRed3  = 2,
        kBlue1 = 3,
        kBlue2 = 4,
        kBlue3 = 5,
    };

    Q_INVOKABLE bool canBeEnabled();
    Q_INVOKABLE bool running() const;
    Q_INVOKABLE bool isInTest() const;
    Q_INVOKABLE bool isEnabled() const;
    Q_INVOKABLE bool isInAutonomous() const;
    Q_INVOKABLE bool isInTeleoperated() const;
    Q_INVOKABLE bool isConnectedToFMS() const;
    Q_INVOKABLE bool isVoltageBrownout() const;
    Q_INVOKABLE bool isEmergencyStopped() const;
    Q_INVOKABLE bool isConnectedToRobot() const;
    Q_INVOKABLE bool isConnectedToRadio() const;
    Q_INVOKABLE bool isRobotCodeRunning() const;

    Q_INVOKABLE QString filesPath() const;
    Q_INVOKABLE QString appLoggerPath() const;
    Q_INVOKABLE QString robotLoggerPath() const;

    Q_INVOKABLE float currentBatteryVoltage() const;
    Q_INVOKABLE float nominalBatteryVoltage() const;
    Q_INVOKABLE float nominalBatteryAmperage() const;

    Q_INVOKABLE int team() const;
    Q_INVOKABLE int cpuUsage() const;
    Q_INVOKABLE int ramUsage() const;
    Q_INVOKABLE int diskUsage() const;
    Q_INVOKABLE int maxPOVCount() const;
    Q_INVOKABLE int maxAxisCount() const;
    Q_INVOKABLE int maxButtonCount() const;
    Q_INVOKABLE int maxJoystickCount() const;

    Q_INVOKABLE int getNumAxes (int joystick);
    Q_INVOKABLE int getNumPOVs (int joystick);
    Q_INVOKABLE int getNumButtons (int joystick);
    Q_INVOKABLE int getRealNumAxes (int joystick);
    Q_INVOKABLE int getRealNumPOVs (int joystick);
    Q_INVOKABLE int getRealNumButtons (int joystick);

    Q_INVOKABLE int joystickCount();
    Q_INVOKABLE JoystickList* joysticks();

    Q_INVOKABLE Alliance alliance() const;
    Q_INVOKABLE Position position() const;
    Q_INVOKABLE ControlMode controlMode() const;
    Q_INVOKABLE CommStatus fmsCommStatus() const;
    Q_INVOKABLE EnableStatus enableStatus() const;
    Q_INVOKABLE CommStatus radioCommStatus() const;
    Q_INVOKABLE CommStatus robotCommStatus() const;
    Q_INVOKABLE CodeStatus robotCodeStatus() const;
    Q_INVOKABLE VoltageStatus voltageStatus() const;

    Q_INVOKABLE QString radioAddress() const;
    Q_INVOKABLE QString robotAddress() const;
    Q_INVOKABLE QString generalStatus() const;
    Q_INVOKABLE QString customRadioAddress() const;
    Q_INVOKABLE QString customRobotAddress() const;
    Q_INVOKABLE QString defaultRadioAddress() const;
    Q_INVOKABLE QString defaultRobotAddress() const;
    Q_INVOKABLE OperationStatus operationStatus() const;

    Q_INVOKABLE QStringList protocols() const;
    Q_INVOKABLE QStringList teamStations() const;


    Q_INVOKABLE bool registerJoystick (const int& axes,
                                       const int& buttons,
                                       const int& povs);

public slots:
    void init();
    void rebootRobot();
    void enableRobot();
    void disableRobot();
    void resetJoysticks();
    void restartRobotCode();
    void switchToTestMode();
    void switchToAutonomous();
    void switchToTeleoperated();
    void setTeam (const int& team);
    void removeJoystick (const int& id);
    void setEnabled (const bool& enabled);
    void setProtocol (Protocol* protocol);
    void setTeamStation (const int& station);
    void setProtocolType (const int& protocol);
    void setAlliance (const Alliance& alliance);
    void setPosition (const Position& position);
    void setControlMode (const ControlMode& mode);
    void setParallelSocketCount (const int& count);
    void setCustomRobotAddress (const QString& address);
    void setCustomRadioAddress (const QString& address);
    void setEnabled (const EnableStatus& statusChanged);
    void setOperationStatus (const OperationStatus& statusChanged);
    void updatePOV (const int& id, const int& pov, const int& angle);
    void updateAxis (const int& id, const int& axis, const float& value);
    void updateButton (const int& id, const int& button, const bool& pressed);

private slots:
    void stop();
    void start();
    void resetFMS();
    void resetRadio();
    void resetRobot();
    void sendFMSPacket();
    void sendRadioPacket();
    void sendRobotPacket();
    void calculateScanSpeed();
    void reconfigureJoysticks();
    void readFMSPacket (const QByteArray& data);
    void readRadioPacket (const QByteArray& data);
    void readRobotPacket (const QByteArray& data);

protected:
    DriverStation();
    ~DriverStation();

signals:
    void resetted();
    void initialized();
    void protocolChanged();
    void newMessage (QString message);
    void joystickCountChanged (int count);

private:
    bool m_init;
    bool m_running;
    int m_fmsInterval;
    int m_radioInterval;
    int m_robotInterval;

    JoystickList m_joysticks;
    QString m_customRadioAddress;
    QString m_customRobotAddress;

    Sockets* m_sockets;
    Protocol* m_protocol;
    NetConsole* m_console;

    Watchdog* m_fmsWatchdog;
    Watchdog* m_radioWatchdog;
    Watchdog* m_robotWatchdog;

    DS_Config* config() const;
    Protocol* protocol() const;
};

#endif
