/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QApplication>
#include <QJsonDocument>
#include <QElapsedTimer>

#include "RobotLogger.h"
#include "DriverStation.h"

RobotLogger::RobotLogger() {
    m_timer = new QElapsedTimer;
    m_timer->start();

    m_previousRAM = 0;
    m_previousCPU = 0;
    m_previousLoss = 0;
    m_previousVoltage = 0;

    connect (qApp, SIGNAL (aboutToQuit()), this, SLOT (serializeLogs()));
}

RobotLogger::~RobotLogger() {
    delete m_timer;
}

void RobotLogger::registerWatchdogTimeout() {
}

void RobotLogger::registerVoltage (qreal voltage) {
    if (m_previousVoltage != voltage) {
        m_previousVoltage = voltage;
        m_voltages.append (voltage);
        m_voltageTimings.append (m_timer->elapsed());
    }
}

void RobotLogger::registerPacketLoss (int pktLoss) {
    if (pktLoss != m_previousLoss) {
        m_previousLoss = pktLoss;
        m_pktLosses.append (pktLoss);
        m_pktTimings.append (m_timer->elapsed());
    }
}

void RobotLogger::registerRobotRAMUsage (int usage) {
    if (m_previousRAM != usage) {
        m_previousRAM = usage;
        m_ramUsages.append (usage);
        m_ramTimings.append (m_timer->elapsed());
    }
}

void RobotLogger::registerRobotCPUUsage (int usage) {
    if (m_previousCPU != usage) {
        m_previousCPU = usage;
        m_cpuUsages.append (usage);
        m_cpuTimings.append (m_timer->elapsed());
    }
}

void RobotLogger::registerAlliance (DS::Alliance alliance) {
    qDebug() << "Robot alliance set to" << alliance;
}

void RobotLogger::registerPosition (DS::Position position) {
    qDebug() << "Robot possition set to" << position;
}

void RobotLogger::registerControlMode (DS::ControlMode mode) {
    qDebug() << "Robot control mode set to" << mode;
}

void RobotLogger::registerCodeStatus (DS::CodeStatus status) {
    qDebug() << "Robot code status set to" << status;
}

void RobotLogger::registerEnableStatus (DS::EnableStatus status) {
    qDebug() << "Robot enabled status set to" << status;
}

void RobotLogger::registerRadioCommStatus (DS::CommStatus status) {
    qDebug() << "Radio communication status set to" << status;
}

void RobotLogger::registerRobotCommStatus (DS::CommStatus status) {
    qDebug() << "Robot communication status set to" << status;
}

void RobotLogger::registerVoltageStatus (DS::VoltageStatus status) {
    qDebug() << "Robot voltage status set to" << status;
}

void RobotLogger::registerOperationStatus (DS::OperationStatus status) {
    qDebug() << "Robot operation status set to" << status;
}

void RobotLogger::serializeLogs() {
    qDebug() << "Begin serialization of robot logs...";

    /* Register voltage values */
    QVariantList volList;
    for (int i = 0; i < m_voltages.count(); ++i) {
        QVariantMap map;
        map.insert ("voltage", m_voltages.at (i));
        map.insert ("time", m_voltageTimings.at (i));
        volList.append (map);
    }

    /* Register CPU usages */
    QVariantList cpuList;
    for (int i = 0; i < m_cpuUsages.count(); ++i) {
        QVariantMap map;
        map.insert ("CPU", m_cpuUsages.at (i));
        map.insert ("time", m_cpuTimings.at (i));
        cpuList.append (map);
    }

    /* Register RAM usages */
    QVariantList ramList;
    for (int i = 0; i < m_ramUsages.count(); ++i) {
        QVariantMap map;
        map.insert ("RAM", m_ramUsages.at (i));
        map.insert ("time", m_ramTimings.at (i));
        ramList.append (map);
    }

    /* Register packet loss */
    QVariantList pktList;
    for (int i = 0; i < m_pktLosses.count(); ++i) {
        QVariantMap map;
        map.insert ("loss", m_pktLosses.at (i));
        map.insert ("time", m_pktTimings.at (i));
        pktList.append (map);
    }

    /* Serialize registered data */
    QJsonArray array;
    QJsonDocument document;
    array << "CPU Usage"     << QJsonValue::fromVariant (cpuList);
    array << "RAM Usage"     << QJsonValue::fromVariant (ramList);
    array << "Packet Loss"   << QJsonValue::fromVariant (pktList);
    array << "Robot Voltage" << QJsonValue::fromVariant (volList);
    document.setArray (array);

    /* Save JSON document to disk */
    QFile file (DS_ROBOT_LOGS_FILE());
    if (file.open (QFile::WriteOnly)) {
        file.write (document.toJson (QJsonDocument::Compact));
        file.close();
    }

    qDebug() << "Serialization of robot logs finished";
}
