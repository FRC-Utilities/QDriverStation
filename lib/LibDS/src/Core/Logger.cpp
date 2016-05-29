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

FILE* DUMP;
QElapsedTimer TIMER;
bool INITIALIZED = false;
const char* PRINT_FMT  = "%-14s %-13s %-12s\n";
const QString TIME_FMT = "dd-MMM-yyyy HH:mm:ss";
const QString DUMP_FMT = "yyyy-MMM-dd HH-mm-ss ddd";

QString REPEAT (QString input, int reps)
{
    QString string;

    for (int i = 0; i < reps; ++i)
        string.append (input);

    return string;
}

QString LOGGER_PATH()
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

void INIT_LOGGER()
{
    TIMER.start();

    /* Construct file name */
    QString name = QDateTime::currentDateTime().toString (DUMP_FMT);
    QString fpath = LOGGER_PATH() + "/" + name + ".log";

    /* Open the dump file */
    DUMP = fopen (fpath.toStdString().c_str(), "a");
    if (!DUMP) DUMP = stdout;

    /* Get app info */
    QString appN = qApp->applicationName();
    QString appV = qApp->applicationVersion();
    QString time = QDateTime::currentDateTime().toString (TIME_FMT);

    /* Format app info */
    time.prepend ("Log created on:      ");
    appN.prepend ("Application name:    ");
    appV.prepend ("Application version: ");

    /* Append app info */
    fprintf (DUMP, "%s\n",   time.toStdString().c_str());
    fprintf (DUMP, "%s\n",   appN.toStdString().c_str());
    fprintf (DUMP, "%s\n\n", appV.toStdString().c_str());

    /* Start the table header */
    fprintf (DUMP, PRINT_FMT, "ELAPSED TIME", "ERROR LEVEL", "MESSAGE");
    fprintf (DUMP, PRINT_FMT,
             REPEAT ("-", 14).toStdString().c_str(),
             REPEAT ("-", 13).toStdString().c_str(),
             REPEAT ("-", 32).toStdString().c_str());

    INITIALIZED = true;
}

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
    fprintf (DUMP, PRINT_FMT,
             time.toStdString().c_str(),
             level.toStdString().c_str(),
             message.toStdString().c_str());
    fprintf (stderr, PRINT_FMT,
             time.toStdString().c_str(),
             level.toStdString().c_str(),
             message.toStdString().c_str());
}
