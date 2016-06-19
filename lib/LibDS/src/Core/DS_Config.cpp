/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#include "DS_Config.h"
#include "DriverStation.h"

#include <QElapsedTimer>
#include <Core/RobotLogger.h>

DS_Config::DS_Config() {
    m_timer = new QElapsedTimer;
    m_robotLogger = new RobotLogger;

    m_team = 0;
    m_voltage = 0;
    m_cpuUsage = 0;
    m_ramUsage = 0;
    m_diskUsage = 0;
    m_libVersion = "";
    m_pcmVersion = "";
    m_pdpVersion = "";
    m_simulated = false;
    m_timerEnabled = false;
    m_position = kPosition1;
    m_alliance = kAllianceRed;
    m_codeStatus = kCodeFailing;
    m_operationStatus = kNormal;
    m_fmsCommStatus = kCommsFailing;
    m_enableStatus = kDisabled;
    m_voltageStatus = kVoltageNormal;
    m_radioCommStatus = kCommsFailing;
    m_robotCommStatus = kCommsFailing;
    m_controlMode = kControlTeleoperated;

    updateElapsedTime();
}

DS_Config::~DS_Config() {
    delete m_timer;
    delete m_robotLogger;
}

RobotLogger* DS_Config::robotLogger() {
    return m_robotLogger;
}

DS_Config* DS_Config::getInstance() {
    static DS_Config instance;
    return &instance;
}

int DS_Config::team() const {
    return m_team;
}

int DS_Config::cpuUsage() const {
    return m_cpuUsage;
}

int DS_Config::ramUsage() const {
    return m_ramUsage;
}

int DS_Config::diskUsage() const {
    return m_diskUsage;
}

float DS_Config::voltage() const {
    if (isConnectedToRobot())
        return m_voltage;

    return 0;
}

bool DS_Config::isEnabled() const {
    return enableStatus() == DS::kEnabled;
}

bool DS_Config::isSimulated() const {
    return m_simulated;
}

DS::Alliance DS_Config::alliance() const {
    return m_alliance;
}

DS::Position DS_Config::position() const {
    return m_position;
}

bool DS_Config::isFMSAttached() const {
    return fmsCommStatus() == DS::kCommsWorking;
}

QString DS_Config::libVersion() const {
    return m_libVersion;
}

QString DS_Config::pcmVersion() const {
    return m_pcmVersion;
}

QString DS_Config::pdpVersion() const {
    return m_pdpVersion;
}

bool DS_Config::isEmergencyStopped() const {
    return operationStatus() == DS::kEmergencyStop;
}

bool DS_Config::isRobotCodeRunning() const {
    return robotCodeStatus() == DS::kCodeRunning;
}

bool DS_Config::isConnectedToRadio() const {
    return radioCommStatus() == DS::kCommsWorking;
}

bool DS_Config::isConnectedToRobot() const {
    return robotCommStatus() == DS::kCommsWorking;
}

DS::ControlMode DS_Config::controlMode() const {
    return m_controlMode;
}

DS::CommStatus DS_Config::fmsCommStatus() const {
    return m_fmsCommStatus;
}

DS::EnableStatus DS_Config::enableStatus() const {
    return m_enableStatus;
}

DS::CommStatus DS_Config::radioCommStatus() const {
    return m_radioCommStatus;
}

DS::CommStatus DS_Config::robotCommStatus() const {
    return m_robotCommStatus;
}

DS::CodeStatus DS_Config::robotCodeStatus() const {
    return m_codeStatus;
}

DS::VoltageStatus DS_Config::voltageStatus() const {
    return m_voltageStatus;
}

DS::OperationStatus DS_Config::operationStatus() const {
    return m_operationStatus;
}

void DS_Config::updateTeam (int team) {
    if (m_team != team) {
        m_team = team;
        emit teamChanged (m_team);

        qDebug() << "Team number set to" << team;
    }
}

void DS_Config::setRobotCode (bool code) {
    DS::CodeStatus status = DS::kCodeFailing;
    if (code) status = DS::kCodeRunning;

    updateRobotCodeStatus (status);
}

void DS_Config::setEnabled (bool enabled) {
    DS::EnableStatus status = DS::kDisabled;
    if (enabled) status = DS::kEnabled;

    updateEnabled (status);
}

void DS_Config::updateCpuUsage (int usage) {
    m_cpuUsage = 0;
    emit cpuUsageChanged (usage);
}

void DS_Config::updateRamUsage (int usage) {
    m_ramUsage = 0;
    emit ramUsageChanged (usage);
}

void DS_Config::updateDiskUsage (int usage) {
    m_diskUsage = 0;
    emit diskUsageChanged (usage);
}


void DS_Config::setBrownout (bool brownout) {
    DS::VoltageStatus status = DS::kVoltageNormal;
    if (brownout) status = DS::kVoltageBrownout;

    updateVoltageStatus (status);
}

void DS_Config::setEmergencyStop (bool estop) {
    DS::OperationStatus status = DS::kNormal;
    if (estop) status = DS::kEmergencyStop;

    updateOperationStatus (status);
}

void DS_Config::updateVoltage (float voltage) {
    /* Round voltage to two decimal places */
    m_voltage = roundf (voltage * 100) / 100;

    /* Avoid this: http://i.imgur.com/iAAi1bX.png */
    if (m_voltage > DriverStation::getInstance()->maxBatteryVoltage())
        m_voltage = DriverStation::getInstance()->maxBatteryVoltage();

    /* Separate voltage into natural and decimal numbers */
    int integer = static_cast<int>(m_voltage);
    int decimal = static_cast<float>(m_voltage - integer) * 100;

    /* Convert the obtained numbers into strings */
    QString integer_str = QString::number (integer);
    QString decimal_str = QString::number (decimal);

    /* Prepend a 0 to the decimal numbers if required */
    if (decimal < 10)
        decimal_str.prepend ("0");

    /* Emit signals */
    emit voltageChanged (m_voltage);
    emit voltageChanged (integer_str + "." + decimal_str + " V");
}

void DS_Config::updateSimulated (bool simulated) {
    m_simulated = simulated;
    emit simulatedChanged (simulated);
}

void DS_Config::updateAlliance (Alliance alliance) {
    if (m_alliance != alliance) {
        m_alliance = alliance;
        m_robotLogger->registerAlliance (alliance);
    }

    emit allianceChanged (m_alliance);
}

void DS_Config::updatePosition (Position position) {
    if (m_position != position) {
        m_position = position;
        m_robotLogger->registerPosition (position);
    }

    emit positionChanged (m_position);
}

void DS_Config::updateRobotCodeStatus (CodeStatus status) {
    if (m_codeStatus != status) {
        m_codeStatus = status;
        m_robotLogger->registerCodeStatus (status);
    }

    emit codeStatusChanged (m_codeStatus);
    emit statusChanged (DriverStation::getInstance()->generalStatus());
}

void DS_Config::updateControlMode (ControlMode mode) {
    if (m_controlMode != mode) {
        m_controlMode = mode;
        m_robotLogger->registerControlMode (mode);
    }

    emit controlModeChanged (m_controlMode);
    emit statusChanged (DriverStation::getInstance()->generalStatus());
}

void DS_Config::updateLibVersion (const QString& version) {
    if (m_libVersion != version) {
        m_libVersion = version;
        qDebug() << "LIB version set to" << version;
    }

    emit libVersionChanged (m_libVersion);
}

void DS_Config::updatePcmVersion (const QString& version) {
    if (m_pcmVersion != version) {
        m_pcmVersion = version;
        qDebug() << "PCM version set to" << version;
    }

    emit pcmVersionChanged (m_pcmVersion);
}

void DS_Config::updatePdpVersion (const QString& version) {
    if (m_pdpVersion != version) {
        m_pdpVersion = version;
        qDebug() << "PDP version set to" << version;
    }

    emit pdpVersionChanged (m_pdpVersion);
}

void DS_Config::updateEnabled (EnableStatus status) {
    if (m_enableStatus != status) {
        m_enableStatus = status;

        if (status == DS::kEnabled) {
            m_timer->restart();
            m_timerEnabled = true;
        }

        else
            m_timerEnabled = false;

        m_robotLogger->registerEnableStatus (status);
    }

    emit enabledChanged (m_enableStatus);
    emit statusChanged (DriverStation::getInstance()->generalStatus());
}

void DS_Config::updateFMSCommStatus (CommStatus status) {
    if (m_fmsCommStatus != status) {
        m_fmsCommStatus = status;
        qDebug() << "FMS comm. status set to" << status;
    }

    emit fmsCommStatusChanged (m_fmsCommStatus);
    emit statusChanged (DriverStation::getInstance()->generalStatus());
}

void DS_Config::updateRadioCommStatus (CommStatus status) {
    if (m_radioCommStatus != status) {
        m_radioCommStatus = status;
        m_robotLogger->registerRadioCommStatus (status);
    }

    emit radioCommStatusChanged (m_radioCommStatus);
}

void DS_Config::updateRobotCommStatus (CommStatus status) {
    if (m_robotCommStatus != status) {
        m_robotCommStatus = status;
        m_robotLogger->registerRobotCommStatus (status);
    }

    emit robotCommStatusChanged (m_robotCommStatus);
    emit statusChanged (DriverStation::getInstance()->generalStatus());
}

void DS_Config::updateVoltageStatus (VoltageStatus status) {
    if (m_voltageStatus != status) {
        m_voltageStatus = status;
        m_robotLogger->registerVoltageStatus (status);
    }

    emit voltageStatusChanged (m_voltageStatus);
    emit statusChanged (DriverStation::getInstance()->generalStatus());

}

void DS_Config::updateOperationStatus (OperationStatus status) {
    if (m_operationStatus != status) {
        m_operationStatus = status;
        m_robotLogger->registerOperationStatus (status);
    }

    emit operationStatusChanged (m_operationStatus);
    emit statusChanged (DriverStation::getInstance()->generalStatus());
}

void DS_Config::updateElapsedTime() {
    if (m_timerEnabled && isConnectedToRobot() && !isEmergencyStopped()) {
        quint32 msec = m_timer->elapsed();
        quint32 secs = (msec / 1000);
        quint32 mins = (secs / 60) % 60;

        secs = secs % 60;
        msec = msec % 1000;

        emit elapsedTimeChanged (msec);
        emit elapsedTimeChanged (QString ("%1:%2.%3")
                                 .arg (mins, 2, 10, QLatin1Char ('0'))
                                 .arg (secs, 2, 10, QLatin1Char ('0'))
                                 .arg (QString::number (msec).at (0)));
    }

    DS_Schedule (200, this, SLOT (updateElapsedTime()));
}
