/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#include <QElapsedTimer>

#include "RobotLogger.h"
#include "DriverStation.h"

RobotLogger::RobotLogger()
{
    m_timer = new QElapsedTimer;
    m_timer->start();
}

RobotLogger::~RobotLogger()
{
    delete m_timer;
}

void RobotLogger::registerWatchdogTimeout()
{
}

void RobotLogger::registerVoltage (float voltage)
{
    Q_UNUSED (voltage);
}

void RobotLogger::registerRobotRAMUsage (int usage)
{
    Q_UNUSED (usage);
}

void RobotLogger::registerRobotCPUUsage (int usage)
{
    Q_UNUSED (usage);
}

void RobotLogger::registerAlliance (DS::Alliance alliance)
{
    qDebug() << "Robot alliance set to" << alliance;
}

void RobotLogger::registerPosition (DS::Position position)
{
    qDebug() << "Robot possition set to" << position;
}

void RobotLogger::registerControlMode (DS::ControlMode mode)
{
    qDebug() << "Robot control mode set to" << mode;
}

void RobotLogger::registerCodeStatus (DS::CodeStatus status)
{
    qDebug() << "Robot code status set to" << status;
}

void RobotLogger::registerEnableStatus (DS::EnableStatus status)
{
    qDebug() << "Robot enabled status set to" << status;
}

void RobotLogger::registerRadioCommStatus (DS::CommStatus status)
{
    qDebug() << "Radio communication status set to" << status;
}

void RobotLogger::registerRobotCommStatus (DS::CommStatus status)
{
    qDebug() << "Robot communication status set to" << status;
}

void RobotLogger::registerVoltageStatus (DS::VoltageStatus status)
{
    qDebug() << "Robot voltage status set to" << status;
}

void RobotLogger::registerOperationStatus (DS::OperationStatus status)
{
    qDebug() << "Robot operation status set to" << status;
}
