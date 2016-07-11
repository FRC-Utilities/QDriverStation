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
 * \brief Logs robot data and application messages
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

    void messageHandler (QtMsgType type,
                         const QMessageLogContext& context,
                         const QString& data);

  public slots:
    void saveLogs();
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

    FILE* m_dump;
    bool m_closed;
    bool m_initialized;
    QString m_logFilePath;
    QString m_dumpFilePath;

    int m_previousRAM;
    int m_previousCPU;
    int m_previousLoss;
    qreal m_previousVoltage;

    QList<int> m_ramUsages;
    QList<int> m_cpuUsages;
    QList<int> m_pktLosses;
    QList<qreal> m_voltages;

    QList<qint64> m_ramTimings;
    QList<qint64> m_cpuTimings;
    QList<qint64> m_pktTimings;
    QList<qint64> m_voltageTimings;
};

#endif
