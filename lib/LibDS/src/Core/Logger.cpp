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
#include <QElapsedTimer>

#include "Logger.h"

#define PRINT_FMT "%-14s %-13s %-12s\n"
#define PRINT(string) QString(string).toLocal8Bit().constData()
#define GET_DATE_TIME(format) QDateTime::currentDateTime().toString(format)

/**
 * Repeats the \a input string \a n times and returns the obtained string
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

    m_closed = false;
    m_initialized = false;
    m_eventsRegistered = false;

    m_timer->start();
    m_logFilePath = logsPath() + "/"
            + GET_DATE_TIME ("yyyy_MM_dd hh_mm_ss ddd")
            + "." + extension();
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
 * Opens the given log \a file and parses its JSON data
 */
QJsonDocument Logger::openLog (const QString& name) const {
    QFile file (name);
    QJsonDocument document;

    if (file.open (QFile::ReadOnly)) {
        document = QJsonDocument::fromJson (file.readAll());
        file.close();
    }

    return document;
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
 * Saves the robot events and the application logs into a compact JSON file.
 * This file can later be used by teams to diagnostic their robots or by the
 * LibDS developers to fix an issue.
 */
void Logger::saveLogs() {
    /* Register voltage values */
    QVariantList voltageList;
    for (int i = 0; i < m_voltage.count(); ++i) {
        QVariantMap map;
        map.insert ("data", m_voltage.at (i));
        map.insert ("time", m_voltageTimings.at (i));
        voltageList.append (map);
    }

    /* Register CPU usages */
    QVariantList cpuList;
    for (int i = 0; i < m_cpuUsage.count(); ++i) {
        QVariantMap map;
        map.insert ("data", m_cpuUsage.at (i));
        map.insert ("time", m_cpuTimings.at (i));
        cpuList.append (map);
    }

    /* Register RAM usages */
    QVariantList ramList;
    for (int i = 0; i < m_ramUsage.count(); ++i) {
        QVariantMap map;
        map.insert ("data", m_ramUsage.at (i));
        map.insert ("time", m_ramTimings.at (i));
        ramList.append (map);
    }

    /* Register packet loss */
    QVariantList pktList;
    for (int i = 0; i < m_pktLoss.count(); ++i) {
        QVariantMap map;
        map.insert ("data", m_pktLoss.at (i));
        map.insert ("time", m_pktTimings.at (i));
        pktList.append (map);
    }

    /* Register code status */
    QVariantList codeStatusList;
    for (int i = 0; i < m_codeStatus.count(); ++i) {
        QVariantMap map;
        map.insert ("data", m_codeStatus.at (i));
        map.insert ("time", m_codeStatusTimings.at (i));
        codeStatusList.append (map);
    }

    /* Register control modes */
    QVariantList controlModeList;
    for (int i = 0; i < m_controlMode.count(); ++i) {
        QVariantMap map;
        map.insert ("data", m_controlMode.at (i));
        map.insert ("time", m_controlModeTimings.at (i));
        controlModeList.append (map);
    }

    /* Register voltage status */
    QVariantList voltageStatusList;
    for (int i = 0; i < m_voltageStatus.count(); ++i) {
        QVariantMap map;
        map.insert ("data", m_voltageStatus.at (i));
        map.insert ("time", m_voltageStatusTimings.at (i));
        voltageStatusList.append (map);
    }

    /* Register enabled status */
    QVariantList enabledStatusList;
    for (int i = 0; i < m_enabledStatus.count(); ++i) {
        QVariantMap map;
        map.insert ("data", m_enabledStatus.at (i));
        map.insert ("time", m_enabledStatusTimings.at (i));
        enabledStatusList.append (map);
    }

    /* Register operation status */
    QVariantList operationStatusList;
    for (int i = 0; i < m_operationStatus.count(); ++i) {
        QVariantMap map;
        map.insert ("data", m_operationStatus.at (i));
        map.insert ("time", m_operationStatusTimings.at (i));
        operationStatusList.append (map);
    }

    /* Register radio comm. status */
    QVariantList radioCommStatusList;
    for (int i = 0; i < m_radioCommStatus.count(); ++i) {
        QVariantMap map;
        map.insert ("data", m_radioCommStatus.at (i));
        map.insert ("time", m_radioCommStatusTimings.at (i));
        radioCommStatusList.append (map);
    }

    /* Register robot comm. status */
    QVariantList robotCommStatusList;
    for (int i = 0; i < m_robotCommStatus.count(); ++i) {
        QVariantMap map;
        map.insert ("data", m_robotCommStatus.at (i));
        map.insert ("time", m_robotCommStatusTimings.at (i));
        robotCommStatusList.append (map);
    }

    /* Serialize event data */
    QJsonArray array;
    QJsonDocument document;
    array.append (QJsonValue::fromVariant (cpuList));
    array.append (QJsonValue::fromVariant (ramList));
    array.append (QJsonValue::fromVariant (pktList));
    array.append (QJsonValue::fromVariant (voltageList));
    array.append (QJsonValue::fromVariant (codeStatusList));
    array.append (QJsonValue::fromVariant (controlModeList));
    array.append (QJsonValue::fromVariant (voltageStatusList));
    array.append (QJsonValue::fromVariant (enabledStatusList));
    array.append (QJsonValue::fromVariant (operationStatusList));
    array.append (QJsonValue::fromVariant (radioCommStatusList));
    array.append (QJsonValue::fromVariant (robotCommStatusList));

    /* Add application logs to JSON */
    QFile logs (m_dumpFilePath);
    if (logs.open (QFile::ReadOnly)) {
        array.append (QJsonValue::fromVariant (logs.readAll()));
        logs.close();
    }

    /* Add NetConsole input to JSON */
    array.append (QJsonValue::fromVariant (m_netConsole));

    /* Save JSON document to disk */
    document.setArray (array);
    QFile file (m_logFilePath);
    if (file.open (QFile::WriteOnly)) {
        file.write (document.toJson (QJsonDocument::Compact));
        file.close();
    }

    /* Overwrite log in two seconds */
    if (!m_closed)
        DS_Schedule (2000, this, SLOT (saveLogs()));
}

/**
 * Closes the console log dump file
 */
void Logger::closeLogs() {
    if (m_dump && m_initialized && !m_closed) {
        qDebug() << "Log buffer closed";

        saveLogs();
        fclose (m_dump);

        m_closed = true;
        m_initialized = false;
    }
}

/**
 * Registers the inital robot events in the event lists
 */
void Logger::registerInitialEvents() {
    if (!m_eventsRegistered) {
        m_eventsRegistered = true;

        registerVoltage (0);
        registerPacketLoss (0);
        registerRobotRAMUsage (0);
        registerRobotCPUUsage (0);
        registerAlliance (DS::kAllianceRed);
        registerEnableStatus (DS::kDisabled);
        registerOperationStatus (DS::kNormal);
        registerCodeStatus (DS::kCodeFailing);
        registerVoltageStatus (DS::kVoltageNormal);
        registerRadioCommStatus (DS::kCommsFailing);
        registerRobotCommStatus (DS::kCommsFailing);
        registerControlMode (DS::kControlTeleoperated);
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
        m_voltage.append (voltage);
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
        m_pktLoss.append (pktLoss);
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
        m_ramUsage.append (usage);
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
        m_cpuUsage.append (usage);
        m_cpuTimings.append (m_timer->elapsed());
    }
}

/**
 * Logs the given team \a alliance to the console output.
 */
void Logger::registerAlliance (DS::Alliance alliance) {
    qDebug() << "Robot alliance set to" << alliance;
}

/**
 * Logs the given team \a position to the console output.
 */
void Logger::registerPosition (DS::Position position) {
    qDebug() << "Robot possition set to" << position;
}

/**
 * Registers the given control \a mode to the event lists.
 * \note This value will only be registered if the given data is different
 *       from the data registered earlier (to avoid creating huge log files)
 */
void Logger::registerControlMode (DS::ControlMode mode) {
    if (m_previousControlMode != mode) {
        m_previousControlMode = mode;
        m_controlMode.append (mode);
        m_controlModeTimings.append (m_timer->elapsed());
    }
}

/**
 * Registers the given code \a status to the event lists.
 * \note This value will only be registered if the given data is different
 *       from the data registered earlier (to avoid creating huge log files)
 */
void Logger::registerCodeStatus (DS::CodeStatus status) {
    if (m_previousCodeStatus != status) {
        m_previousCodeStatus = status;
        m_codeStatus.append (status);
        m_codeStatusTimings.append (m_timer->elapsed());
    }
}

/**
 * Registers the given enable \a status to the event lists.
 * \note This value will only be registered if the given data is different
 *       from the data registered earlier (to avoid creating huge log files)
 */
void Logger::registerEnableStatus (DS::EnableStatus status) {
    if (m_previousEnabledStatus != status) {
        m_previousEnabledStatus = status;
        m_enabledStatus.append (status);
        m_enabledStatusTimings.append (m_timer->elapsed());
    }
}

/**
 * Registers the given radio communication \a status to the event lists.
 * \note This value will only be registered if the given data is different
 *       from the data registered earlier (to avoid creating huge log files)
 */
void Logger::registerRadioCommStatus (DS::CommStatus status) {
    if (m_previousRadioCommStatus != status) {
        m_previousRadioCommStatus = status;
        m_radioCommStatus.append (status);
        m_radioCommStatusTimings.append (m_timer->elapsed());
    }
}

/**
 * Registers the given robot communication \a status to the event lists.
 * \note This value will only be registered if the given data is different
 *       from the data registered earlier (to avoid creating huge log files)
 */
void Logger::registerRobotCommStatus (DS::CommStatus status) {
    if (m_previousRobotCommStatus != status) {
        m_previousRobotCommStatus = status;
        m_robotCommStatus.append (status);
        m_robotCommStatusTimings.append (m_timer->elapsed());
    }
}

/**
 * Registers the given voltage \a status to the event lists.
 * \note This value will only be registered if the given data is different
 *       from the data registered earlier (to avoid creating huge log files)
 */
void Logger::registerVoltageStatus (DS::VoltageStatus status) {
    if (m_previousVoltageStatus != status) {
        m_previousVoltageStatus = status;
        m_voltageStatus.append (status);
        m_voltageStatusTimings.append (m_timer->elapsed());
    }
}

/**
 * Appends the given \a message to the NetConsole log
 */
void Logger::registerNetConsoleMessage (const QString& message) {
    m_netConsole.append (message);
}

/**
 * Registers the given operation \a status to the event lists.
 * \note This value will only be registered if the given data is different
 *       from the data registered earlier (to avoid creating huge log files)
 */
void Logger::registerOperationStatus (DS::OperationStatus status) {
    if (m_previousOperationStatus != status) {
        m_previousOperationStatus = status;
        m_operationStatus.append (status);
        m_operationStatusTimings.append (m_timer->elapsed());
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

    /* Open the dump file */
    m_dumpFilePath = QDir::tempPath() + "/QDriverStation.log";
    m_dump = fopen (m_dumpFilePath.toStdString().c_str(), "w");
    m_dump = !m_dump ? stderr : m_dump;

    /* Save initial dump to DS log */
    saveLogs();

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
