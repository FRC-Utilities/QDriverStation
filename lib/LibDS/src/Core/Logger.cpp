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
#include <QApplication>
#include <QElapsedTimer>

/* Ugly hacks to make code more readable */
#define PRINT_FMT             "%-14s %-13s %-12s\n"
#define PRINT(string)         QString(string).toLocal8Bit().constData()
#define GET_DATE_TIME(format) QDateTime::currentDateTime().toString(format)

/* Global variables */
static FILE* DUMP;
static QElapsedTimer TIMER;
static bool INITIALIZED = false;

/**
 * Repeats the \a input string \a n times and returns the resultant string
 */
static QString REPEAT (QString input, int n)
{
    QString string;

    for (int i = 0; i < n; ++i)
        string.append (input);

    return string;
}

/**
 * Returns the path in which several application files are stored (e.g. logs)
 */
QString DS_FILES_PATH()
{
    QDir dir (QString ("%1/.%2/").arg (QDir::homePath(),
                                       qApp->applicationName().toLower()));

    if (!dir.exists())
        dir.mkpath (".");

    return dir.absolutePath();
}

/**
 * Figures out where to place the application logs
 */
QString DS_LOGGER_PATH()
{
    QDir dir = QDir (DS_FILES_PATH() + "/Application Logs/");

    if (!dir.exists())
        dir.mkpath (".");

    return dir.absolutePath();
}

/**
 * Figures out where to place robot log files (e.g. containing voltage & other
 * stuff)
 */
QString DS_ROBOT_LOGGER_PATH()
{
    QDir dir = QDir (DS_FILES_PATH() + "/Robot Logs/");
    if (!dir.exists())
        dir.mkpath (".");

    return dir.absolutePath();
}

/**
 * Creates the log dump file
 */
static void INIT_LOGGER()
{
    TIMER.start();

    /* Construct file name */
    QString fpath = DS_LOGGER_PATH()
                    + "/"
                    + GET_DATE_TIME ("ddd MMM dd yyyy - HH_mm_ss AP")
                    + ".log";

    /* Open the dump file */
    DUMP = fopen (fpath.toStdString().c_str(), "a");
    if (!DUMP)
        DUMP = stdout;

    /* Get app info */
    QString appN = qApp->applicationName();
    QString appV = qApp->applicationVersion();
    QString time = GET_DATE_TIME ("MMM dd yyyy - HH:mm:ss AP");

    /* Format app info */
    time.prepend ("Log created on:      ");
    appN.prepend ("Application name:    ");
    appV.prepend ("Application version: ");

    /* Append app info */
    fprintf (DUMP, "%s\n",   PRINT (time));
    fprintf (DUMP, "%s\n",   PRINT (appN));
    fprintf (DUMP, "%s\n\n", PRINT (appV));

    /* Start the table header */
    fprintf (DUMP, PRINT_FMT, "ELAPSED TIME", "ERROR LEVEL", "MESSAGE");
    fprintf (DUMP, PRINT_FMT,
             PRINT (REPEAT ("-", 14)),
             PRINT (REPEAT ("-", 13)),
             PRINT (REPEAT ("-", 32)));

    INITIALIZED = true;
}

/**
 * Logs the given data to both the dump file and the console
 *
 * @param type the message level (error, debug, critical or fatal)
 * @param context information regarding who and where sent the message
 * @param message the actual message/data
 */
void DS_MESSAGE_HANDLER (QtMsgType type, const QMessageLogContext& context,
                         const QString& data)
{
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
    fprintf (DUMP,   PRINT_FMT, PRINT (time), PRINT (level), PRINT (data));
    fprintf (stderr, PRINT_FMT, PRINT (time), PRINT (level), PRINT (data));
}
