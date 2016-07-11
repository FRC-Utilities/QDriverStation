/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#ifndef _LIB_DS_ROBOT_LOGGER_H
#define _LIB_DS_ROBOT_LOGGER_H

#include <Core/DS_Common.h>

class QElapsedTimer;

/**
 * \brief Creates and reads files with robot events and application logs
 *
 * This can be later used to diagnostic the robot or to diagnostic the
 * QDriverStation.
 */
class Logger : public QObject {
    Q_OBJECT

  public:
    explicit Logger();

    QString logsPath() const;
    QString extension() const;
    QStringList availableLogs() const;
    QJsonDocument openLog (const QString& name) const;

    void messageHandler (QtMsgType type,
                         const QMessageLogContext& context,
                         const QString& data);

  public slots:
    void saveLogs();
    void registerInitialEvents();
    void registerVoltage (qreal voltage);
    void registerPacketLoss (int pktLoss);
    void registerRobotRAMUsage (int usage);
    void registerRobotCPUUsage (int usage);
    void registerAlliance (DS::Alliance alliance);
    void registerPosition (DS::Position position);
    void registerControlMode (DS::ControlMode mode);
    void registerCodeStatus (DS::CodeStatus status);
    void registerEnableStatus (DS::EnableStatus status);
    void registerRadioCommStatus (DS::CommStatus status);
    void registerRobotCommStatus (DS::CommStatus status);
    void registerVoltageStatus (DS::VoltageStatus status);
    void registerOperationStatus (DS::OperationStatus status);

  private slots:
    void closeLogs();
    void initializeLogger();

  private:
    QElapsedTimer* m_timer;
    bool m_eventsRegistered;

    /* Used for console output (both to stderr and a dump file) */
    FILE* m_dump;
    bool m_closed;
    bool m_initialized;
    QString m_logFilePath;
    QString m_dumpFilePath;

    /* Registers previous event data (to avoid creating huge logs) */
    int m_previousRAM;
    int m_previousCPU;
    int m_previousLoss;
    qreal m_previousVoltage;
    DS::CodeStatus m_previousCodeStatus;
    DS::ControlMode m_previousControlMode;
    DS::CommStatus m_previousRadioCommStatus;
    DS::CommStatus m_previousRobotCommStatus;
    DS::EnableStatus m_previousEnabledStatus;
    DS::VoltageStatus m_previousVoltageStatus;
    DS::OperationStatus m_previousOperationStatus;

    /* These lists hold the events */
    QList<int> m_pktLoss;
    QList<int> m_ramUsage;
    QList<int> m_cpuUsage;
    QList<qreal> m_voltage;
    QList<DS::CodeStatus> m_codeStatus;
    QList<DS::ControlMode> m_controlMode;
    QList<DS::CommStatus> m_radioCommStatus;
    QList<DS::CommStatus> m_robotCommStatus;
    QList<DS::EnableStatus> m_enabledStatus;
    QList<DS::VoltageStatus> m_voltageStatus;
    QList<DS::OperationStatus> m_operationStatus;

    /* These lists hold the timeframes of each event */
    QList<qint64> m_ramTimings;
    QList<qint64> m_cpuTimings;
    QList<qint64> m_pktTimings;
    QList<qint64> m_voltageTimings;
    QList<qint64> m_codeStatusTimings;
    QList<qint64> m_controlModeTimings;
    QList<qint64> m_enabledStatusTimings;
    QList<qint64> m_voltageStatusTimings;
    QList<qint64> m_operationStatusTimings;
    QList<qint64> m_radioCommStatusTimings;
    QList<qint64> m_robotCommStatusTimings;
};

#endif
