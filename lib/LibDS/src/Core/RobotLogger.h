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

class RobotLogger : public QObject {
    Q_OBJECT

  public:
    RobotLogger();
    ~RobotLogger();

  public slots:
    void registerWatchdogTimeout();
    void registerVoltage (qreal voltage);
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

  private:
    QElapsedTimer* m_timer;
};

#endif
