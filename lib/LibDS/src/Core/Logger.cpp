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

#include "Logger.h"
#include "DriverStation.h"

Logger::Logger() {
    m_timer = new QElapsedTimer;
    m_timer->start();

    m_previousRAM = 0;
    m_previousCPU = 0;
    m_previousLoss = 0;
    m_previousVoltage = 0;
}

Logger::~Logger() {
    delete m_timer;
}

void Logger::saveLogs() {
    /* Register voltage values */
    QVariantList volList;
    volList.append ("Robot Voltage");
    for (int i = 0; i < m_voltages.count(); ++i) {
        QVariantMap map;
        map.insert ("voltage", m_voltages.at (i));
        map.insert ("time", m_voltageTimings.at (i));
        volList.append (map);
    }

    /* Register CPU usages */
    QVariantList cpuList;
    cpuList.append ("CPU Usage");
    for (int i = 0; i < m_cpuUsages.count(); ++i) {
        QVariantMap map;
        map.insert ("CPU", m_cpuUsages.at (i));
        map.insert ("time", m_cpuTimings.at (i));
        cpuList.append (map);
    }

    /* Register RAM usages */
    QVariantList ramList;
    ramList.append ("RAM Usage");
    for (int i = 0; i < m_ramUsages.count(); ++i) {
        QVariantMap map;
        map.insert ("RAM", m_ramUsages.at (i));
        map.insert ("time", m_ramTimings.at (i));
        ramList.append (map);
    }

    /* Register packet loss */
    QVariantList pktList;
    pktList.append ("Packet Loss");
    for (int i = 0; i < m_pktLosses.count(); ++i) {
        QVariantMap map;
        map.insert ("loss", m_pktLosses.at (i));
        map.insert ("time", m_pktTimings.at (i));
        pktList.append (map);
    }

    /* Serialize registered data */
    QJsonArray array;
    QJsonDocument document;
    array.append (QJsonValue::fromVariant (cpuList));
    array.append (QJsonValue::fromVariant (ramList));
    array.append (QJsonValue::fromVariant (pktList));
    array.append (QJsonValue::fromVariant (volList));

    /* Add application logs to JSON */
    QFile logs (DS_CONSOLE_LOG());
    if (logs.open (QFile::ReadOnly)) {
        QVariantList list;
        list.append ("Application Logs");
        list.append (logs.readAll());
        array.append (QJsonValue::fromVariant (list));
    }

    /* Save JSON document to disk */
    document.setArray (array);
    QFile file (DS_LOG_FILE());
    if (file.open (QFile::WriteOnly)) {
        file.write (document.toBinaryData());
        file.close();
    }

    /* Close the console dump file */
    DS_CLOSE_CONSOLE_LOG();
}

void Logger::registerWatchdogTimeout() {
}

void Logger::registerVoltage (qreal voltage) {
    if (m_previousVoltage != voltage) {
        m_previousVoltage = voltage;
        m_voltages.append (voltage);
        m_voltageTimings.append (m_timer->elapsed());
    }
}

void Logger::registerPacketLoss (int pktLoss) {
    if (pktLoss != m_previousLoss) {
        m_previousLoss = pktLoss;
        m_pktLosses.append (pktLoss);
        m_pktTimings.append (m_timer->elapsed());
    }
}

void Logger::registerRobotRAMUsage (int usage) {
    if (m_previousRAM != usage) {
        m_previousRAM = usage;
        m_ramUsages.append (usage);
        m_ramTimings.append (m_timer->elapsed());
    }
}

void Logger::registerRobotCPUUsage (int usage) {
    if (m_previousCPU != usage) {
        m_previousCPU = usage;
        m_cpuUsages.append (usage);
        m_cpuTimings.append (m_timer->elapsed());
    }
}

void Logger::registerAlliance (DS::Alliance alliance) {
    qDebug() << "Robot alliance set to" << alliance;
}

void Logger::registerPosition (DS::Position position) {
    qDebug() << "Robot possition set to" << position;
}

void Logger::registerControlMode (DS::ControlMode mode) {
    qDebug() << "Robot control mode set to" << mode;
}

void Logger::registerCodeStatus (DS::CodeStatus status) {
    qDebug() << "Robot code status set to" << status;
}

void Logger::registerEnableStatus (DS::EnableStatus status) {
    qDebug() << "Robot enabled status set to" << status;
}

void Logger::registerRadioCommStatus (DS::CommStatus status) {
    qDebug() << "Radio communication status set to" << status;
}

void Logger::registerRobotCommStatus (DS::CommStatus status) {
    qDebug() << "Robot communication status set to" << status;
}

void Logger::registerVoltageStatus (DS::VoltageStatus status) {
    qDebug() << "Robot voltage status set to" << status;
}

void Logger::registerOperationStatus (DS::OperationStatus status) {
    qDebug() << "Robot operation status set to" << status;
}
