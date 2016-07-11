/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#include "DS_Config.h"
#include "DriverStation.h"

#include <QThread>
#include <QElapsedTimer>
#include <Core/Logger.h>

DS_Config::DS_Config() {
    m_timer = new QElapsedTimer;
    m_logger = new Logger;

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

    /* Move the robot logger to another thread */
    QThread* thread = new QThread (this);
    m_logger->moveToThread (thread);
    thread->start (QThread::NormalPriority);

    /* Begin elapsed time loop */
    updateElapsedTime();
}

DS_Config::~DS_Config() {
    delete m_timer;
    delete m_logger;
}

/**
 * Returns the instance of the robot logger object
 */
Logger* DS_Config::logger() {
    return m_logger;
}

/**
 * Returns the one and only instance of
 */
DS_Config* DS_Config::getInstance() {
    static DS_Config instance;
    return &instance;
}

/**
 * Returns the current team number
 */
int DS_Config::team() const {
    return m_team;
}

/**
 * Returns the current CPU usage of the robot (0 - 100)
 */
int DS_Config::cpuUsage() const {
    return m_cpuUsage;
}

/**
 * Returns the current RAM usage of the robot
 */
int DS_Config::ramUsage() const {
    return m_ramUsage;
}

/**
 * Returns the current disk usage of the robot
 */
int DS_Config::diskUsage() const {
    return m_diskUsage;
}

/**
 * Returns the current voltage of the robot
 */
qreal DS_Config::voltage() const {
    if (isConnectedToRobot())
        return m_voltage;

    return 0;
}

/**
 * Returns \c true if the robot is enabled. Otherwise, it returns \c false
 */
bool DS_Config::isEnabled() const {
    return enableStatus() == DS::kEnabled;
}

/**
 * Returns \c true if the robot is simulated. Otherwise, it returns \c false
 */
bool DS_Config::isSimulated() const {
    return m_simulated;
}

/**
 * Returns the current alliance of the robot (be it user-set or FMS-set)
 */
DS::Alliance DS_Config::alliance() const {
    return m_alliance;
}

/**
 * Returns the current position of the robot (be it user-set or FMS-set)
 */
DS::Position DS_Config::position() const {
    return m_position;
}

/**
 * Returns \c true if the Driver Station is connected to the Field Management
 * System (FMS).
 */
bool DS_Config::isFMSAttached() const {
    return fmsCommStatus() == DS::kCommsWorking;
}

/**
 * Returns the library version of the robot
 */
QString DS_Config::libVersion() const {
    return m_libVersion;
}

/**
 * Returns the PCM version of the robot
 */
QString DS_Config::pcmVersion() const {
    return m_pcmVersion;
}

/**
 * Returns the PDP version of the robot
 */
QString DS_Config::pdpVersion() const {
    return m_pdpVersion;
}

/**
 * Returns \c true if the robot is emergency stopped. Otherwise, it returns
 * \c false.
 */
bool DS_Config::isEmergencyStopped() const {
    return operationStatus() == DS::kEmergencyStop;
}

/**
 * Returns \c true if the robot code is running. Otherwise, it returns \c false
 */
bool DS_Config::isRobotCodeRunning() const {
    return robotCodeStatus() == DS::kCodeRunning;
}

/**
 * Returns \c true if the Driver Station is connected to the radio. Otherwise,
 * it returns \c false
 */
bool DS_Config::isConnectedToRadio() const {
    return radioCommStatus() == DS::kCommsWorking;
}

/**
 * Returns \c true if the Driver Station is connected to the robot. Otherwise,
 * it returns \c false
 */
bool DS_Config::isConnectedToRobot() const {
    return robotCommStatus() == DS::kCommsWorking;
}

/**
 * Returns the current control mode of the robot
 */
DS::ControlMode DS_Config::controlMode() const {
    return m_controlMode;
}

/**
 * Returns the current status of the FMS communications.
 * \note You can also use the \c isConnectedToFMS() function
 */
DS::CommStatus DS_Config::fmsCommStatus() const {
    return m_fmsCommStatus;
}

/**
 * Returns the current enabled status of the robot
 * \note You can also use the \c isEnabled() function
 */
DS::EnableStatus DS_Config::enableStatus() const {
    return m_enableStatus;
}

/**
 * Returns the current status of the radio communications
 * \note You can also use the \c isConnectedToRadio() function
 */
DS::CommStatus DS_Config::radioCommStatus() const {
    return m_radioCommStatus;
}

/**
 * Returns the current status of the robot communications
 * \note You can also use the \c isConnectedToRobot() function
 */
DS::CommStatus DS_Config::robotCommStatus() const {
    return m_robotCommStatus;
}

/**
 * Returns the current status of the robot code
 * \note You can also use the \c isRobotCodeRunning() function
 */
DS::CodeStatus DS_Config::robotCodeStatus() const {
    return m_codeStatus;
}

/**
 * Returns the current voltage brownout status of the robot.
 */
DS::VoltageStatus DS_Config::voltageStatus() const {
    return m_voltageStatus;
}

/**
 * Returns the current operation status of the robot.
 * \note You can also use the \c isEmergencyStopped() function
 */
DS::OperationStatus DS_Config::operationStatus() const {
    return m_operationStatus;
}

/**
 * Changes the \a team number and fires the appropriate signals if required
 */
void DS_Config::updateTeam (int team) {
    if (m_team != team) {
        m_team = team;
        emit teamChanged (m_team);

        qDebug() << "Team number set to" << team;
    }
}

/**
 * Changes the robot \a code status and fires the appropriate signals if
 * required
 */
void DS_Config::setRobotCode (bool code) {
    DS::CodeStatus status = DS::kCodeFailing;
    if (code) status = DS::kCodeRunning;

    updateRobotCodeStatus (status);
}

/**
 * Changes the \a enabled status and fires the appropriate signals if required
 */
void DS_Config::setEnabled (bool enabled) {
    DS::EnableStatus status = DS::kDisabled;
    if (enabled) status = DS::kEnabled;

    updateEnabled (status);
}

/**
 * Changes the CPU \a usage and fires the appropriate signals if required
 */
void DS_Config::updateCpuUsage (int usage) {
    m_cpuUsage = 0;
    emit cpuUsageChanged (usage);
}

/**
 * Changes the RAM \a usage and fires the appropriate signals if required
 */
void DS_Config::updateRamUsage (int usage) {
    m_ramUsage = 0;
    emit ramUsageChanged (usage);
}

/**
 * Changes the disk \a usage and fires the appropriate signals if required
 */
void DS_Config::updateDiskUsage (int usage) {
    m_diskUsage = 0;
    emit diskUsageChanged (usage);
}

/**
 * Changes the voltage \a brownout status and fires the appropriate signals
 * if required.
 */
void DS_Config::setBrownout (bool brownout) {
    DS::VoltageStatus status = DS::kVoltageNormal;
    if (brownout) status = DS::kVoltageBrownout;

    updateVoltageStatus (status);
}

/**
 * Changes the \a estop status and fires the appropriate signals if required
 */
void DS_Config::setEmergencyStop (bool estop) {
    DS::OperationStatus status = DS::kNormal;
    if (estop) status = DS::kEmergencyStop;

    updateOperationStatus (status);
}

/**
 * Changes the robot \a voltage and fires the appropriate signals if required
 */
void DS_Config::updateVoltage (qreal voltage) {
    /* Round voltage to two decimal places */
    m_voltage = roundf (voltage * 100) / 100;

    /* Avoid this: http://i.imgur.com/iAAi1bX.png */
    if (m_voltage > DriverStation::getInstance()->maxBatteryVoltage())
        m_voltage = DriverStation::getInstance()->maxBatteryVoltage();

    /* Separate voltage into natural and decimal numbers */
    int integer = static_cast<int> (m_voltage);
    int decimal = static_cast<qreal> (m_voltage - integer) * 100;

    /* Convert the obtained numbers into strings */
    QString integer_str = QString::number (integer);
    QString decimal_str = QString::number (decimal);

    /* Prepend a 0 to the decimal numbers if required */
    if (decimal < 10)
        decimal_str.prepend ("0");

    /* Emit signals */
    emit voltageChanged (m_voltage);
    emit voltageChanged (integer_str + "." + decimal_str + " V");

    /* Log robot voltage */
    m_logger->registerVoltage (m_voltage);
}

/**
 * Changes the \a simulated status and fires the appropriate signals if
 * required
 */
void DS_Config::updateSimulated (bool simulated) {
    m_simulated = simulated;
    emit simulatedChanged (simulated);
}

/**
 * Changes the \a alliance and fires the appropriate signals if required
 */
void DS_Config::updateAlliance (Alliance alliance) {
    if (m_alliance != alliance) {
        m_alliance = alliance;
        m_logger->registerAlliance (alliance);
    }

    emit allianceChanged (m_alliance);
}

/**
 * Changes the \a position and fires the appropriate signals if required
 */
void DS_Config::updatePosition (Position position) {
    if (m_position != position) {
        m_position = position;
        m_logger->registerPosition (position);
    }

    emit positionChanged (m_position);
}

/**
 * Changes the robot code \a status and fires the appropriate signals if
 * required
 */
void DS_Config::updateRobotCodeStatus (CodeStatus status) {
    if (m_codeStatus != status) {
        m_codeStatus = status;
        m_logger->registerCodeStatus (status);
    }

    emit codeStatusChanged (m_codeStatus);
    emit statusChanged (DriverStation::getInstance()->generalStatus());
}

/**
 * Changes the robot control \a mode and fires the appropriate signals if
 * required
 */
void DS_Config::updateControlMode (ControlMode mode) {
    if (m_controlMode != mode) {
        m_controlMode = mode;
        m_logger->registerControlMode (mode);
    }

    emit controlModeChanged (m_controlMode);
    emit statusChanged (DriverStation::getInstance()->generalStatus());
}

/**
 * Changes the robot library \a version and fires the appropriate signals if
 * required
 */
void DS_Config::updateLibVersion (const QString& version) {
    if (m_libVersion != version) {
        m_libVersion = version;
        qDebug() << "LIB version set to" << version;
    }

    emit libVersionChanged (m_libVersion);
}

/**
 * Changes the PCM \a version and fires the appropriate signals if required
 */
void DS_Config::updatePcmVersion (const QString& version) {
    if (m_pcmVersion != version) {
        m_pcmVersion = version;
        qDebug() << "PCM version set to" << version;
    }

    emit pcmVersionChanged (m_pcmVersion);
}

/**
 * Changes the PDP/PDB \a version and fires the appropriate signals if required
 */
void DS_Config::updatePdpVersion (const QString& version) {
    if (m_pdpVersion != version) {
        m_pdpVersion = version;
        qDebug() << "PDP version set to" << version;
    }

    emit pdpVersionChanged (m_pdpVersion);
}

/**
 * Changes the enabled \a status and fires the appropriate signals if required
 */
void DS_Config::updateEnabled (EnableStatus status) {
    if (m_enableStatus != status) {
        m_enableStatus = status;

        if (status == DS::kEnabled) {
            m_timer->restart();
            m_timerEnabled = true;
        }

        else
            m_timerEnabled = false;

        m_logger->registerEnableStatus (status);
    }

    emit enabledChanged (m_enableStatus);
    emit statusChanged (DriverStation::getInstance()->generalStatus());
}

/**
 * Changes the FMS communication \a status and fires the appropriate signals
 * if required
 */
void DS_Config::updateFMSCommStatus (CommStatus status) {
    if (m_fmsCommStatus != status) {
        m_fmsCommStatus = status;
        qDebug() << "FMS comm. status set to" << status;
    }

    emit fmsCommStatusChanged (m_fmsCommStatus);
    emit statusChanged (DriverStation::getInstance()->generalStatus());
}

/**
 * Changes the radio communication \a status and fires the appropriate signals
 * if required
 */
void DS_Config::updateRadioCommStatus (CommStatus status) {
    if (m_radioCommStatus != status) {
        m_radioCommStatus = status;
        m_logger->registerRadioCommStatus (status);
    }

    emit radioCommStatusChanged (m_radioCommStatus);
}

/**
 * Changes the robot communication \a status and fires the appropriate signals
 * if required
 */
void DS_Config::updateRobotCommStatus (CommStatus status) {
    if (m_robotCommStatus != status) {
        m_robotCommStatus = status;
        m_logger->registerRobotCommStatus (status);
    }

    emit robotCommStatusChanged (m_robotCommStatus);
    emit statusChanged (DriverStation::getInstance()->generalStatus());
}

/**
 * Changes the voltage brownout \a status and fires the appropriate signals
 * if required
 */
void DS_Config::updateVoltageStatus (VoltageStatus status) {
    if (m_voltageStatus != status) {
        m_voltageStatus = status;
        m_logger->registerVoltageStatus (status);
    }

    emit voltageStatusChanged (m_voltageStatus);
    emit statusChanged (DriverStation::getInstance()->generalStatus());

}

/**
 * Changes the robot operation \a status and fires the appropriate signals
 * if required
 */
void DS_Config::updateOperationStatus (OperationStatus status) {
    if (m_operationStatus != status) {
        m_operationStatus = status;
        m_logger->registerOperationStatus (status);
    }

    emit operationStatusChanged (m_operationStatus);
    emit statusChanged (DriverStation::getInstance()->generalStatus());
}

/**
 * Calculates the elapsed time since the robot has been enabled (regardless of
 * the operation mode).
 *
 * This function is called every 100 milliseconds.
 *
 * \note This function will not run if there is no communication status with
 *       the robot or if the robot is emergency stopped
 * \note The elapsed time will be resetted when the user changes the control
 *       mode of the robot
 */
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

    DS_Schedule (100, this, SLOT (updateElapsedTime()));
}
