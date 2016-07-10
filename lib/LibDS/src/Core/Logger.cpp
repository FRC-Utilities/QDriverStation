/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#include "DS_Common.h"

#include <QDir>
#include <QFile>
#include <QSysInfo>
#include <QApplication>
#include <QElapsedTimer>

//------------------------------------------------------------------------------
// Ugly hacks to make the code more readable
//------------------------------------------------------------------------------

#if defined Q_OS_WIN
    #define COUT stdout
#else
    #define COUT stderr
#endif

#define PRINT_FMT             "%-14s %-13s %-12s\n"
#define PRINT(string)         QString(string).toLocal8Bit().constData()
#define GET_DATE_TIME(format) QDateTime::currentDateTime().toString(format)

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------

static FILE* DUMP;
static QElapsedTimer TIMER;
static bool INITIALIZED = false;

static QString appLogsFile;
static QString robotLogsFile;

/**
 * Repeats the \a input string \a n times and returns the resultant string
 */
static QString REPEAT (QString input, int n) {
    QString string;

    for (int i = 0; i < n; ++i)
        string.append (input);

    return string;
}

/**
 * Adds a "section" to the log to make it easier to read
 */
static void ADD_HEADER (const QString& title) {
    fprintf (DUMP, "%s\n", PRINT (REPEAT ("-", 72)));
    fprintf (DUMP, "%s\n", PRINT (title.toUpper()));
    fprintf (DUMP, "%s\n", PRINT (REPEAT ("-", 72)));
    fprintf (DUMP, "%s\n", "");
}

/**
 * Creates the log dump file
 */
static void INIT_LOGGER() {
    TIMER.start();

    /* Set file names */
    appLogsFile = DS_APP_LOGGER_PATH()
                  + "/"
                  + GET_DATE_TIME ("MMM dd yyyy - HH_mm_ss")
                  + ".log";
    robotLogsFile = DS_ROBOT_LOGGER_PATH()
                    + "/"
                    + GET_DATE_TIME ("MMM dd yyyy - HH_mm_ss")
                    + ".json";

    /* Open the dump file */
    DUMP = fopen (DS_APP_LOGS_FILE().toStdString().c_str(), "a+");
    DUMP = !DUMP ? COUT : DUMP;

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

    /* Add log header */
    ADD_HEADER ("Start of log");

    /* Format app info */
    time.prepend ("Log created on:      ");
    sysV.prepend ("Operating System:    ");
    appN.prepend ("Application name:    ");
    appV.prepend ("Application version: ");

    /* Append app info */
    fprintf (DUMP, "%s\n",   PRINT (time));
    fprintf (DUMP, "%s\n",   PRINT (sysV));
    fprintf (DUMP, "%s\n",   PRINT (appN));
    fprintf (DUMP, "%s\n\n", PRINT (appV));

    /* Start the table header */
    fprintf (DUMP, "%s\n", PRINT (REPEAT ("-", 72)));
    fprintf (DUMP, PRINT_FMT, "ELAPSED TIME", "ERROR LEVEL", "MESSAGE");
    fprintf (DUMP, "%s\n", PRINT (REPEAT ("-", 72)));

    INITIALIZED = true;
}

/**
 * Returns the path in which several application files are stored (e.g. logs)
 */
QString DS_FILES_PATH() {
    QDir dir (QString ("%1/.%2/").arg (QDir::homePath(),
                                       qApp->applicationName().toLower()));

    if (!dir.exists())
        dir.mkpath (".");

    return dir.absolutePath();
}

/**
 * Returns the file in which the application logs are written
 */
QString DS_APP_LOGS_FILE() {
    return appLogsFile;
}

/**
 * Returns the file in which the robot logs are written
 */
QString DS_ROBOT_LOGS_FILE() {
    return robotLogsFile;
}

/**
 * Figures out where to place the application logs
 */
QString DS_APP_LOGGER_PATH() {
    QDir dir = QDir (DS_FILES_PATH() + "/Logs/Client/");

    if (!dir.exists())
        dir.mkpath (".");

    return dir.absolutePath();
}

/**
 * Figures out where to place robot log files (e.g. containing voltage & other
 * stuff)
 */
QString DS_ROBOT_LOGGER_PATH() {
    QDir dir = QDir (DS_FILES_PATH() + "/Logs/Robot/");
    if (!dir.exists())
        dir.mkpath (".");

    return dir.absolutePath();
}

/**
 * Ensures that the logger is closed properly when the application quits
 */
void DS_CLOSE_LOGS() {
    if (DUMP && INITIALIZED) {
        qDebug() << "Log buffer closed";

        INITIALIZED = false;
        ADD_HEADER ("End of log, have a nice day!");
        fclose (DUMP);
    }
}

/**
 * Logs the given data to both the dump file and the console
 *
 * @param type the message level (error, debug, critical or fatal)
 * @param context information regarding who and where sent the message
 * @param message the actual message/data
 */
void DS_MESSAGE_HANDLER (QtMsgType type, const QMessageLogContext& context,
                         const QString& data) {
    Q_UNUSED (context);

    /* First call, create log file */
    if (!INITIALIZED)
        INIT_LOGGER();

    /* Get elapsed time */
    quint32 msec = TIMER.elapsed();
    quint32 secs = (msec / 1000);
    quint32 mins = (secs / 60) % 60;
    secs = secs % 60;
    msec = msec % 1000;
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
    fprintf (DUMP, PRINT_FMT, PRINT (time), PRINT (level), PRINT (data));
    fprintf (COUT, PRINT_FMT, PRINT (time), PRINT (level), PRINT (data));

    /* Flush to write "instantly" */
    fflush (DUMP);
}
