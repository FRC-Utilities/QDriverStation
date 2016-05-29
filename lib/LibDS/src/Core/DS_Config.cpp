/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#include "DS_Config.h"
#include "DriverStation.h"

DS_Config::DS_Config()
{
    m_team = 0;
    m_voltage = 0;
    m_libVersion = "";
    m_pcmVersion = "";
    m_pdpVersion = "";
    m_timerEnabled = false;
    m_position = kPosition1;
    m_alliance = kAllianceRed;
    m_codeStatus = kCodeFailing;
    m_operationStatus = kOperationNormal;
    m_fmsCommStatus = kCommsFailing;
    m_enableStatus = kDisabled;
    m_voltageStatus = kVoltageNormal;
    m_radioCommStatus = kCommsFailing;
    m_robotCommStatus = kCommsFailing;
    m_controlMode = kControlTeleoperated;

    updateElapsedTime();
}

DS_Config* DS_Config::getInstance()
{
    static DS_Config instance;
    return &instance;
}

int DS_Config::team() const
{
    return m_team;
}

float DS_Config::voltage() const
{
    return m_voltage;
}

bool DS_Config::isEnabled() const
{
    return enableStatus() == DS::kEnabled;
}

DS::Alliance DS_Config::alliance() const
{
    return m_alliance;
}

DS::Position DS_Config::position() const
{
    return m_position;
}

bool DS_Config::isFMSAttached() const
{
    return fmsCommStatus() == DS::kCommsWorking;
}

QString DS_Config::libVersion() const
{
    return m_libVersion;
}

QString DS_Config::pcmVersion() const
{
    return m_pcmVersion;
}

QString DS_Config::pdpVersion() const
{
    return m_pdpVersion;
}

bool DS_Config::isEmergencyStopped() const
{
    return operationStatus() == DS::kOperationEmergencyStop;
}

bool DS_Config::isRobotCodeRunning() const
{
    return robotCodeStatus() == DS::kCodeRunning;
}

bool DS_Config::isConnectedToRadio() const
{
    return radioCommStatus() == DS::kCommsWorking;
}

bool DS_Config::isConnectedToRobot() const
{
    return robotCommStatus() == DS::kCommsWorking;
}

DS::ControlMode DS_Config::controlMode() const
{
    return m_controlMode;
}

DS::CommStatus DS_Config::fmsCommStatus() const
{
    return m_fmsCommStatus;
}

DS::EnableStatus DS_Config::enableStatus() const
{
    return m_enableStatus;
}

DS::CommStatus DS_Config::radioCommStatus() const
{
    return m_radioCommStatus;
}

DS::CommStatus DS_Config::robotCommStatus() const
{
    return m_robotCommStatus;
}

DS::CodeStatus DS_Config::robotCodeStatus() const
{
    return m_codeStatus;
}

DS::VoltageStatus DS_Config::voltageStatus() const
{
    return m_voltageStatus;
}

DS::OperationStatus DS_Config::operationStatus() const
{
    return m_operationStatus;
}

void DS_Config::updateTeam (const int& team)
{
    if (m_team != team) {
        m_team = team;
        emit teamChanged (m_team);

        qDebug() << "Team number set to" << team;
    }
}

void DS_Config::setRobotCode (const bool& code)
{
    DS::CodeStatus status = DS::kCodeFailing;
    if (code) status = DS::kCodeRunning;

    updateRobotCodeStatus (status);
}

void DS_Config::setEnabled (const bool& enabled)
{
    DS::EnableStatus status = DS::kDisabled;
    if (enabled) status = DS::kEnabled;

    updateEnabled (status);
}

void DS_Config::updateCpuUsage (const int& usage)
{
    emit cpuUsageChanged (usage);
}

void DS_Config::setBrownout (const bool& brownout)
{
    DS::VoltageStatus status = DS::kVoltageNormal;
    if (brownout) status = DS::kVoltageBrownout;

    updateVoltageStatus (status);
}

void DS_Config::setEmergencyStop (const bool& estop)
{
    DS::OperationStatus status = DS::kOperationNormal;
    if (estop) status = DS::kOperationEmergencyStop;

    updateOperationStatus (status);
}

void DS_Config::updateVoltage (const float& voltage)
{
    if (m_voltage != voltage) {
        m_voltage = roundf (voltage * 100) / 100;

        int integer = (int) voltage;
        int decimal = (float) (voltage - integer) * 10;

        QString integer_str = QString::number (integer);
        QString decimal_str = QString::number (decimal);

        if (decimal < 10)
            decimal_str.prepend ("0");

        emit voltageChanged (m_voltage);
        emit voltageChanged (integer_str + "." + decimal_str + " V");
    }
}

void DS_Config::updateAlliance (const Alliance& alliance)
{
    if (m_alliance != alliance) {
        m_alliance = alliance;
        emit allianceChanged (m_alliance);

        qDebug() << "Alliance set to" << alliance;
    }
}

void DS_Config::updatePosition (const Position& position)
{
    if (m_position != position) {
        m_position = position;
        emit positionChanged (m_position);

        qDebug() << "Position set to" << position;
    }
}

void DS_Config::updateLibVersion (const QString& version)
{
    if (m_libVersion != version) {
        m_libVersion = version;
        emit libVersionChanged (m_libVersion);

        qDebug() << "LIB version set to" << version;
    }
}

void DS_Config::updatePcmVersion (const QString& version)
{
    if (m_pcmVersion != version) {
        m_pcmVersion = version;
        emit pcmVersionChanged (m_pcmVersion);

        qDebug() << "PCM version set to" << version;
    }
}

void DS_Config::updatePdpVersion (const QString& version)
{
    if (m_pdpVersion != version) {
        m_pdpVersion = version;
        emit pdpVersionChanged (m_pdpVersion);

        qDebug() << "PDP version set to" << version;
    }
}

void DS_Config::updateRobotCodeStatus (const CodeStatus& status)
{
    if (m_codeStatus != status) {
        m_codeStatus = status;
        emit codeStatusChanged (status);
        emit statusChanged (DriverStation::getInstance()->generalStatus());

        qDebug() << "Code status set to" << status;
    }
}

void DS_Config::updateControlMode (const ControlMode& mode)
{
    if (m_controlMode != mode) {
        m_controlMode = mode;
        emit controlModeChanged (m_controlMode);
        emit statusChanged (DriverStation::getInstance()->generalStatus());

        qDebug() << "Control mode set to" << mode;
    }
}

void DS_Config::updateEnabled (const EnableStatus& status)
{
    if (m_enableStatus != status) {
        m_enableStatus = status;

        if (status == DS::kEnabled) {
            m_timer.restart();
            m_timerEnabled = true;
        }

        else
            m_timerEnabled = false;

        emit enabledChanged (m_enableStatus);
        emit statusChanged (DriverStation::getInstance()->generalStatus());

        qDebug() << "Enable status set to" << status;
    }
}

void DS_Config::updateRamUsage (const int& usage, const int& total)
{
    emit ramUsageChanged (usage, total);
}

void DS_Config::updateDiskUsage (const int& usage, const int& total)
{
    emit diskUsageChanged (usage, total);
}

void DS_Config::updateFMSCommStatus (const CommStatus& status)
{
    if (m_fmsCommStatus != status) {
        m_fmsCommStatus = status;
        emit fmsCommStatusChanged (m_fmsCommStatus);
        emit statusChanged (DriverStation::getInstance()->generalStatus());

        qDebug() << "FMS comm. status set to" << status;
    }
}

void DS_Config::updateRadioCommStatus (const CommStatus& status)
{
    if (m_radioCommStatus != status) {
        m_radioCommStatus = status;
        emit radioCommStatusChanged (m_radioCommStatus);

        qDebug() << "Radio comm. status set to" << status;
    }
}

void DS_Config::updateRobotCommStatus (const CommStatus& status)
{
    if (m_robotCommStatus != status) {
        m_robotCommStatus = status;
        emit robotCommStatusChanged (m_robotCommStatus);
        emit statusChanged (DriverStation::getInstance()->generalStatus());

        qDebug() << "Robot comm. status set to" << status;
    }
}

void DS_Config::updateVoltageStatus (const VoltageStatus& status)
{
    if (m_voltageStatus != status) {
        m_voltageStatus = status;
        emit voltageStatusChanged (m_voltageStatus);
        emit statusChanged (DriverStation::getInstance()->generalStatus());

        qDebug() << "Voltage status set to" << status;
    }
}

void DS_Config::updateOperationStatus (const OperationStatus& status)
{
    if (m_operationStatus != status) {
        m_operationStatus = status;
        emit operationStatusChanged (m_operationStatus);
        emit statusChanged (DriverStation::getInstance()->generalStatus());

        qDebug() << "Operation status set to" << status;
    }
}

void DS_Config::updateElapsedTime()
{
    if (m_timerEnabled && isConnectedToRobot() && !isEmergencyStopped()) {
        quint32 msec = m_timer.elapsed();
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

    QTimer::singleShot (100, Qt::PreciseTimer,
                        this, SLOT (updateElapsedTime()));
}
