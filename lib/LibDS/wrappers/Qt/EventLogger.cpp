/*
 * Copyright (c) 2015-2017 Alex Spataru <alex_spataru@outlook>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "EventLogger.h"

#include <stdio.h>

#include <QUrl>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QTimer>
#include <QSysInfo>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>
#include <QApplication>
#include <QJsonDocument>
#include <QDesktopServices>

#define LOG qDebug() << "DS Events:"

/* Used for the custom message handler */
#define PRINT_FMT "%-14s %-13s %-12s\n"
#define PRINT(string) QString(string).toLocal8Bit().constData()
#define GET_DATE_TIME(format) QDateTime::currentDateTime().toString(format)

/**
 * Repeats the \a input string \a n times and returns the obtained string
 */
static QString REPEAT (QString input, int n)
{
    QString string;

    for (int i = 0; i < n; ++i)
        string.append (input);

    return string;
}

/**
 * Connects the signals/slots between the \c DriverStation and the logger
 */
DSEventLogger::DSEventLogger()
{
    m_init = 0;
    m_dump = NULL;
    m_currentLog = "";

    init();

    saveDataLoop();
    connectSlots();
}

/**
 * Closes the log file
 */
DSEventLogger::~DSEventLogger()
{
    saveData();
}

/**
 * Returns a pointer to the only instance of this class
 */
DSEventLogger* DSEventLogger::getInstance()
{
    static DSEventLogger instance;
    return &instance;
}

/**
 * Returns the path in which the log files are saved
 */
QString DSEventLogger::logsPath() const
{
    return QString ("%1/.%2/%3/Logs/")
           .arg (QDir::homePath(),
                 qApp->applicationName().toLower(),
                 qApp->applicationVersion().toLower());
}

/**
 * Calls the appropiate functions to display the \a data on the console
 * and write it on the log file
 */
void DSEventLogger::messageHandler (QtMsgType type,
                                    const QMessageLogContext& context,
                                    const QString& data)
{
    (void) type;
    (void) context;

    getInstance()->handleMessage (type, data);
}

/**
 * Writes the message output to the console and to the dump file (which is
 * dumped on the DS log file when the application exits).
 */
void DSEventLogger::handleMessage (const QtMsgType type, const QString& data)
{
    /* Data is empty */
    if (data.isEmpty())
        return;

    /* Logger not initialized */
    if (!m_init)
        init();

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

    /* Get elapsed time */
    qint64 msec = m_timer.elapsed();
    qint64 secs = (msec / 1000);
    qint64 mins = (secs / 60) % 60;

    /* Get the remaining seconds and milliseconds */
    secs = secs % 60;
    msec = msec % 1000;

    /* Get current time */
    QString time = (QString ("%1:%2.%3")
                    .arg (mins, 2, 10, QLatin1Char ('0'))
                    .arg (secs, 2, 10, QLatin1Char ('0'))
                    .arg (QString::number (msec).at (0)));

    /* Write logs to dump file and console */
    fprintf (m_dump, PRINT_FMT, PRINT (time), PRINT (level), PRINT (data));
    fprintf (stderr, PRINT_FMT, PRINT (time), PRINT (level), PRINT (data));

    /* Flush to write "instantly" */
    fflush (m_dump);
}

/**
 * Prepares the log file for writting
 */
void DSEventLogger::init()
{
    if (!m_init) {
        /* Initialize the timer */
        m_init = true;
        m_timer.restart();

        /* Get app info */
        QString appN = qApp->applicationName();
        QString appV = qApp->applicationVersion();
        QString ldsV = DriverStation::libDSVersion();
        QString time = GET_DATE_TIME ("MMM dd yyyy - HH:mm:ss AP");

        /* Get dump directoru */
        QString path = QString ("%1/%2/%3/%4/")
                       .arg (logsPath())
                       .arg (GET_DATE_TIME ("yyyy"))
                       .arg (GET_DATE_TIME ("MMMM"))
                       .arg (GET_DATE_TIME ("ddd dd"));

        /* Create logs directory (if necessesary) */
        QDir dir (path);
        if (!dir.exists())
            dir.mkpath (".");

        /* Get dump file path */
        m_currentLog = QString ("%1/%2.log")
                       .arg (path)
                       .arg (GET_DATE_TIME ("HH_mm_ss AP"));

        /* Open dump file */
        m_dump = fopen (m_currentLog.toStdString().c_str(), "w");
        m_dump = !m_dump ? stderr : m_dump;

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
        ldsV.prepend ("LibDS version:       ");
        sysV.prepend ("Operating System:    ");
        appN.prepend ("Application name:    ");
        appV.prepend ("Application version: ");

        /* Append app info */
        fprintf (m_dump, "%s\n",   PRINT (time));
        fprintf (m_dump, "%s\n",   PRINT (ldsV));
        fprintf (m_dump, "%s\n",   PRINT (sysV));
        fprintf (m_dump, "%s\n",   PRINT (appN));
        fprintf (m_dump, "%s\n\n", PRINT (appV));

        /* Start the table header */
        fprintf (m_dump, "%s\n", PRINT (REPEAT ("-", 72)));
        fprintf (m_dump, PRINT_FMT, "ELAPSED TIME", "ERROR LEVEL", "MESSAGE");
        fprintf (m_dump, "%s\n", PRINT (REPEAT ("-", 72)));
    }
}

/**
 * Opens the application logs directory with a file explorer
 */
void DSEventLogger::openLogsPath()
{
    if (!QDir (logsPath()).exists())
        QDir (logsPath()).mkpath (".");

    QDesktopServices::openUrl (QUrl::fromLocalFile (logsPath()));
}

/**
 * Opens the current log file using a system process
 */
void DSEventLogger::openCurrentLog()
{
    if (!m_currentLog.isEmpty())
        QDesktopServices::openUrl (QUrl::fromLocalFile (m_currentLog));
}

/**
 * Saves the log data to the disk and schedules another call in the future
 */
void DSEventLogger::saveDataLoop()
{
    saveData();
    QTimer::singleShot (500, Qt::PreciseTimer, this, SLOT (saveDataLoop()));
}

/**
 * Called when the DS reports a change of the robot's CAN utilization
 */
void DSEventLogger::onCANUsageChanged (int usage)
{
    Q_UNUSED (usage);
    m_canUsageLog.append (qMakePair<qint64, int> (currentTime(), usage));
}

/**
 * Called when the DS reports a change of the robot's CPU usage
 */
void DSEventLogger::onCPUUsageChanged (int usage)
{
    Q_UNUSED (usage);
    m_cpuUsageLog.append (qMakePair<qint64, int> (currentTime(), usage));
}

/**
 * Called when the DS reports a change of the robot's RAM usage
 */
void DSEventLogger::onRAMUsageChanged (int usage)
{
    Q_UNUSED (usage);
    m_ramUsageLog.append (qMakePair<qint64, int> (currentTime(), usage));
}

/**
 * Called when the DS reports a new NetConsole message
 */
void DSEventLogger::onNewMessage (QString message)
{
    Q_UNUSED (message);
    m_messagesLog.append (qMakePair<qint64, QString> (currentTime(), message));
}

/**
 * Called when the DS reports a change of the robot's disk usage
 */
void DSEventLogger::onDiskUsageChanged (int usage)
{
    Q_UNUSED (usage);
    m_diskUsageLog.append (qMakePair<qint64, int> (currentTime(), usage));
}

/**
 * Called when the DS reports a change of the enabled status
 */
void DSEventLogger::onEnabledChanged (bool enabled)
{
    LOG << "Robot enabled state set to" << enabled;
    m_enabledLog.append (qMakePair<qint64, bool> (currentTime(), enabled));
}

/**
 * Called when the DS reports a change of the team number
 */
void DSEventLogger::onTeamNumberChanged (int number)
{
    LOG << "Team number set to" << number;
}

/**
 * Called when the DS reports a change in the robot voltage
 */
void DSEventLogger::onVoltageChanged (float voltage)
{
    Q_UNUSED (voltage);
    m_voltageLog.append (qMakePair<qint64, float> (currentTime(), voltage));
}

/**
 * Called when the DS reports a change in the robot code status
 */
void DSEventLogger::onRobotCodeChanged (bool robotCode)
{
    LOG << "Robot code status set to" << robotCode;
    m_robotCodeLog.append (qMakePair<qint64, bool> (currentTime(), robotCode));
}

/**
 * Called when the DS reports a change in the FMS communications status
 */
void DSEventLogger::onFMSCommunicationsChanged (bool connected)
{
    LOG << "FMS communications set to" << connected;
    m_fmsCommsLog.append (qMakePair<qint64, bool> (currentTime(), connected));
}

/**
 * Called when the DS reports a change in the radio communications status
 */
void DSEventLogger::onRadioCommunicationsChanged (bool connected)
{
    LOG << "Radio communications set to" << connected;
    m_radioCommsLog.append (qMakePair<qint64, bool> (currentTime(), connected));
}

/**
 * Called when the DS reports a change in the robot communications status
 */
void DSEventLogger::onRobotCommunicationsChanged (bool connected)
{
    LOG << "Robot communications set to" << connected;
    m_robotCommsLog.append (qMakePair<qint64, bool> (currentTime(), connected));
}

/**
 * Called when the DS reports a change in the emergency stop status
 */
void DSEventLogger::onEmergencyStoppedChanged (bool emergencyStopped)
{
    LOG << "ESTOP set to" << emergencyStopped;
    m_emergencyStopLog.append (qMakePair<qint64, bool> (currentTime(),
                               emergencyStopped));
}

/**
 * Called when the DS reports a change in the robot control mode
 */
void DSEventLogger::onControlModeChanged (DriverStation::Control mode)
{
    LOG << "Robot control mode set to" << mode;
    m_controlModeLog.append (qMakePair<qint64, int> (currentTime(),
                                                     (int) mode));
}

/**
 * Called when the DS reports a change in the team alliance
 */
void DSEventLogger::onAllianceChanged (DriverStation::Alliance alliance)
{
    LOG << "Team alliance set to" << alliance;
}

/**
 * Called when the DS reports a change in the team position
 */
void DSEventLogger::onPositionChanged (DriverStation::Position position)
{
    LOG << "Team position set to" << position;
}

/**
 * Saves the current log information to a local file using the JSON format
 */
void DSEventLogger::saveData()
{
}

/**
 * Allows the logger class to react when the DriverStation receives a
 * new event from the LibDS library
 */
void DSEventLogger::connectSlots()
{
    DriverStation* ds = DriverStation::getInstance();

    connect (ds,   &DriverStation::canUsageChanged,
             this, &DSEventLogger::onCANUsageChanged);
    connect (ds,   &DriverStation::cpuUsageChanged,
             this, &DSEventLogger::onCPUUsageChanged);
    connect (ds,   &DriverStation::ramUsageChanged,
             this, &DSEventLogger::onRAMUsageChanged);
    connect (ds,   &DriverStation::newMessage,
             this, &DSEventLogger::onNewMessage);
    connect (ds,   &DriverStation::diskUsageChanged,
             this, &DSEventLogger::onDiskUsageChanged);
    connect (ds,   &DriverStation::enabledChanged,
             this, &DSEventLogger::onEnabledChanged);
    connect (ds,   &DriverStation::teamNumberChanged,
             this, &DSEventLogger::onTeamNumberChanged);
    connect (ds,   &DriverStation::voltageChanged,
             this, &DSEventLogger::onVoltageChanged);
    connect (ds,   &DriverStation::robotCodeChanged,
             this, &DSEventLogger::onRobotCodeChanged);
    connect (ds,   &DriverStation::fmsCommunicationsChanged,
             this, &DSEventLogger::onFMSCommunicationsChanged);
    connect (ds,   &DriverStation::radioCommunicationsChanged,
             this, &DSEventLogger::onRadioCommunicationsChanged);
    connect (ds,   &DriverStation::robotCommunicationsChanged,
             this, &DSEventLogger::onRobotCommunicationsChanged);
    connect (ds,   &DriverStation::emergencyStoppedChanged,
             this, &DSEventLogger::onEmergencyStoppedChanged);
    connect (ds,   &DriverStation::controlModeChanged,
             this, &DSEventLogger::onControlModeChanged);
    connect (ds,   &DriverStation::allianceChanged,
             this, &DSEventLogger::onAllianceChanged);
    connect (ds,   &DriverStation::positionChanged,
             this, &DSEventLogger::onPositionChanged);
}

/**
 * Returns the current time signature
 */
qint64 DSEventLogger::currentTime()
{
    QDateTime time = QDateTime::currentDateTime();
    return time.toMSecsSinceEpoch();
}
