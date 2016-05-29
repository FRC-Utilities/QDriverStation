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
 * Figures out where to place the application logs
 */
QString DS_LOGGER_PATH()
{
#ifdef Q_OS_WIN
    QString baseFolder = "/%1/Logs";
#else
    QString baseFolder = "/.%1/Logs";
#endif

    QDir path (QDir::homePath() + baseFolder.arg (qApp->applicationName()));

    if (!path.exists())
        path.mkpath (".");

    return path.absolutePath();
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
                    + GET_DATE_TIME ("ddd MMM dd yyyy - HH_mm_ss")
                    + ".log";

    /* Open the dump file */
    DUMP = fopen (fpath.toStdString().c_str(), "a");
    if (!DUMP)
        DUMP = stdout;

    /* Get app info */
    QString appN = qApp->applicationName();
    QString appV = qApp->applicationVersion();
    QString time = GET_DATE_TIME ("MMM dd yyyy - HH:mm:ss");

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
                         const QString& message)
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
        level = "ERROR_FATAL";
        break;
    default:
        level = "SYSTEM";
        break;
    }

    /* Write log message to file and console */
    fprintf (DUMP,   PRINT_FMT, PRINT (time), PRINT (level), PRINT (message));
    fprintf (stderr, PRINT_FMT, PRINT (time), PRINT (level), PRINT (message));
}
