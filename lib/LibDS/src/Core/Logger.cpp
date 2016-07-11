/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#include <QDir>
#include <QJsonArray>
#include <QJsonObject>
#include <QApplication>
#include <QJsonDocument>
#include <QElapsedTimer>

#include "Logger.h"

#define PRINT_FMT "%-14s %-13s %-12s\n"
#define PRINT(string) QString(string).toLocal8Bit().constData()
#define GET_DATE_TIME(format) QDateTime::currentDateTime().toString(format)

/**
 * Repeats the \a input string \a n times and returns the resultant string
 */
static QString REPEAT (QString input, int n) {
    QString string;

    for (int i = 0; i < n; ++i)
        string.append (input);

    return string;
}

Logger::Logger() {
    m_dump = Q_NULLPTR;
    m_timer = new QElapsedTimer;

    m_closed = 0;
    m_initialized = 0;
    m_previousRAM = 0;
    m_previousCPU = 0;
    m_previousLoss = 0;
    m_previousVoltage = 0;

    m_timer->start();
}

/**
 * Returns the path in which log files are saved
 */
QString Logger::logsPath() const {
    QDir dir (QString ("%1/.%2/Logs/").arg (
                  QDir::homePath(), qApp->applicationName().toLower()));

    if (!dir.exists())
        dir.mkpath (".");

    return dir.absolutePath();
}

/**
 * Returns the extension appended to the log files
 */
QString Logger::extension() const {
    return "qdslog";
}

/**
 * Returns a list with all the logs that have been created locally
 */
QStringList Logger::availableLogs() const {
    QString filter = "*." + extension();
    return QDir (logsPath()).entryList (QStringList (filter));
}

/**
 * Writes the message output to the console and to the dump file (which is
 * dumped on the DS log file when the application exits).
 */
void Logger::messageHandler (QtMsgType type,
                             const QMessageLogContext& context,
                             const QString& data) {
    Q_UNUSED (context);

    /* If logger is closed, abort. If logger is not initialized, initialize! */
    if (m_closed) return;
    if (!m_initialized) initializeLogger();

    /* Get elapsed time */
    quint32 msec = m_timer->elapsed();
    quint32 secs = (msec / 1000);
    quint32 mins = (secs / 60) % 60;

    /* Get the remaining seconds and milliseconds */
    secs = secs % 60;
    msec = msec % 1000;

    /* Get current time */
    QString time = (QString ("%1:%2.%3")
                    .arg (mins, 2, 10, QLatin1Char ('0'))
                    .arg (secs, 2, 10, QLatin1Char ('0'))
                    .arg (QString::number (msec).at (0)));

    /* Get warning level */
    QString level;
    switch (type) {
    case QtDebugMsg:
        level = "DEBUG";
        break;
    case QtWarningMsg:
        level = "WARNING";
        break;
    case QtCriticalMsg:
        level = "CRITICAL";
        break;
    case QtFatalMsg:
        level = "FATAL";
        break;
    default:
        level = "SYSTEM";
        break;
    }

    /* Write all logs to the dump file */
    fprintf (m_dump, PRINT_FMT, PRINT (time), PRINT (level), PRINT (data));
    fprintf (stderr, PRINT_FMT, PRINT (time), PRINT (level), PRINT (data));

    /* Flush to write "instantly" */
    fflush (m_dump);
}

/**
 * Saves the robot events and the application logs into a binary JSON file.
 * This file can later be used by teams to diagnostic their robots or by the
 * LibDS developers to fix an issue.
 */
void Logger::saveLogs() {
    /* Close the console dump file */
    closeLogs();

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
    QFile logs (m_dumpFilePath);
    if (logs.open (QFile::ReadOnly)) {
        QVariantList list;
        list.append ("Application Logs");
        list.append (logs.readAll());
        array.append (QJsonValue::fromVariant (list));
    }

    /* Save JSON document to disk */
    document.setArray (array);
    QFile file (m_logFilePath);
    if (file.open (QFile::WriteOnly)) {
        file.write (document.toBinaryData());
        file.close();
    }
}

/**
 * Registers the given \a voltage to the robot events log.
 * \note This value will only be registered if the given data is different
 *       from the data registered earlier (to avoid creating huge log files)
 */
void Logger::registerVoltage (qreal voltage) {
    if (m_previousVoltage != voltage) {
        m_previousVoltage = voltage;
        m_voltages.append (voltage);
        m_voltageTimings.append (m_timer->elapsed());
    }
}

/**
 * Registers the given packet \a loss to the robot events log.
 * \note This value will only be registered if the given data is different
 *       from the data registered earlier (to avoid creating huge log files)
 */
void Logger::registerPacketLoss (int pktLoss) {
    if (pktLoss != m_previousLoss) {
        m_previousLoss = pktLoss;
        m_pktLosses.append (pktLoss);
        m_pktTimings.append (m_timer->elapsed());
    }
}

/**
 * Registers the given RAM \a usage to the robot events log.
 * \note This value will only be registered if the given data is different
 *       from the data registered earlier (to avoid creating huge log files)
 */
void Logger::registerRobotRAMUsage (int usage) {
    if (m_previousRAM != usage) {
        m_previousRAM = usage;
        m_ramUsages.append (usage);
        m_ramTimings.append (m_timer->elapsed());
    }
}

/**
 * Registers the given CPU \a usage to the robot events log.
 * \note This value will only be registered if the given data is different
 *       from the data registered earlier (to avoid creating huge log files)
 */
void Logger::registerRobotCPUUsage (int usage) {
    if (m_previousCPU != usage) {
        m_previousCPU = usage;
        m_cpuUsages.append (usage);
        m_cpuTimings.append (m_timer->elapsed());
    }
}

/**
 * Logs the given team \a alliance to the console output.
 * \todo Register this event on the robot events log
 */
void Logger::registerAlliance (DS::Alliance alliance) {
    qDebug() << "Robot alliance set to" << alliance;
}

/**
 * Logs the given team \a position to the console output.
 * \todo Register this event on the robot events log
 */
void Logger::registerPosition (DS::Position position) {
    qDebug() << "Robot possition set to" << position;
}

/**
 * Logs the given control \a mode to the console output.
 * \todo Register this event on the robot events log
 */
void Logger::registerControlMode (DS::ControlMode mode) {
    qDebug() << "Robot control mode set to" << mode;
}

/**
 * Logs the given code \a status to the console output.
 * \todo Register this event on the robot events log
 */
void Logger::registerCodeStatus (DS::CodeStatus status) {
    qDebug() << "Robot code status set to" << status;
}

/**
 * Logs the given enabled \a status to the console output.
 * \todo Register this event on the robot events log
 */
void Logger::registerEnableStatus (DS::EnableStatus status) {
    qDebug() << "Robot enabled status set to" << status;
}

/**
 * Logs the given radio communication \a status to the console output.
 * \todo Register this event on the robot events log
 */
void Logger::registerRadioCommStatus (DS::CommStatus status) {
    qDebug() << "Radio communication status set to" << status;
}

/**
 * Logs the given robot communication \a status to the console output.
 * \todo Register this event on the robot events log
 */
void Logger::registerRobotCommStatus (DS::CommStatus status) {
    qDebug() << "Robot communication status set to" << status;
}

/**
 * Logs the given voltage \a status to the console output.
 * \todo Register this event on the robot events log
 */
void Logger::registerVoltageStatus (DS::VoltageStatus status) {
    qDebug() << "Robot voltage status set to" << status;
}

/**
 * Logs the given operation \a status to the console output.
 * \todo Register this event on the robot events log
 */
void Logger::registerOperationStatus (DS::OperationStatus status) {
    qDebug() << "Robot operation status set to" << status;
}

void Logger::closeLogs() {
    if (m_dump && m_initialized && !m_closed) {
        qDebug() << "Log buffer closed";

        m_closed = true;
        m_initialized = false;

        fclose (m_dump);
    }
}

/**
 * Creates the temporary console dump file and sets the file name of the final
 * DS log file.
 *
 * \note The OS name, version and date/time will be registered in the log
 * \note The application name and version are also registered in the log
 */
void Logger::initializeLogger() {
    /* This function will only run once */
    if (m_initialized) return;
    else m_initialized = true;

    /* Get log number string (to ensure correct order up to 9999) */
    QString logNumber = QString::number (availableLogs().count());
    if (availableLogs().count() < 10)
        logNumber.prepend ("000");
    else if (availableLogs().count() < 100)
        logNumber.prepend ("00");
    else if (availableLogs().count() < 1000)
        logNumber.prepend ("0");

    /* Get the names of the log files */
    m_dumpFilePath = QDir::tempPath() + "/QDriverStation.log";
    m_logFilePath = logsPath() + "/"
                    + "Log " + logNumber + " "
                    + GET_DATE_TIME ("(MMM dd yyyy - HH_mm_ss)")
                    + "." + extension();

    /* Open the dump file */
    m_dump = fopen (m_dumpFilePath.toStdString().c_str(), "w");
    m_dump = !m_dump ? stderr : m_dump;

    /* Get app info */
    QString appN = qApp->applicationName();
    QString appV = qApp->applicationVersion();
    QString time = GET_DATE_TIME ("MMM dd yyyy - HH:mm:ss AP");

    /* Get OS information */
    QString sysV;
#if QT_VERSION >= QT_VERSION_CHECK (5, 4, 0)
    sysV = QSysInfo::prettyProductName();
#else
#if defined Q_OS_WIN
    sysV = "Windows";
#elif defined Q_OS_MAC
    sysV = "Mac OSX";
#elif defined Q_OS_LINUX
    sysV = "GNU/Linux";
#else
    sysV = "Unknown";
#endif
#endif

    /* Format app info */
    time.prepend ("Log created on:      ");
    sysV.prepend ("Operating System:    ");
    appN.prepend ("Application name:    ");
    appV.prepend ("Application version: ");

    /* Append app info */
    fprintf (m_dump, "%s\n",   PRINT (time));
    fprintf (m_dump, "%s\n",   PRINT (sysV));
    fprintf (m_dump, "%s\n",   PRINT (appN));
    fprintf (m_dump, "%s\n\n", PRINT (appV));

    /* Start the table header */
    fprintf (m_dump, "%s\n", PRINT (REPEAT ("-", 72)));
    fprintf (m_dump, PRINT_FMT, "ELAPSED TIME", "ERROR LEVEL", "MESSAGE");
    fprintf (m_dump, "%s\n", PRINT (REPEAT ("-", 72)));
}
