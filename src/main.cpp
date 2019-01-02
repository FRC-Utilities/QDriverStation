/*
 * Copyright (c) 2015-2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

//------------------------------------------------------------------------------
// Qt includes
//------------------------------------------------------------------------------

#include <QTime>
#include <QtQml>
#include <iostream>
#include <QApplication>
#include <QJoysticks.h>
#include <QDesktopServices>
#include <QQmlApplicationEngine>

//------------------------------------------------------------------------------
// Library includes
//------------------------------------------------------------------------------

#include <stdio.h>
#include <EventLogger.h>
#include <DriverStation.h>

//------------------------------------------------------------------------------
// Application includes
//------------------------------------------------------------------------------

#include "beeper.h"
#include "versions.h"
#include "shortcuts.h"
#include "utilities.h"
#include "dashboards.h"

//------------------------------------------------------------------------------
// Mac-specific initialization code
//------------------------------------------------------------------------------

#ifdef Q_OS_MAC
    #include <QObject>
    #include <QSettings>
    #include <QMessageBox>
    #include <QDesktopServices>
#endif

//------------------------------------------------------------------------------
// CLI messages
//------------------------------------------------------------------------------

const QString WEBS = "We instructed your web browser to navigate to:        \n"
                     "    %1                                                \n"
                     "If nothing happens, please navigate to that URL.        ";

const QString HELP = "Usage: qdriverstation [ options ... ]                 \n"
                     "                                                      \n"
                     "Options include:                                      \n"
                     "    -b, --bug       Report a bug                      \n"
                     "    -h, --help      Show this message                 \n"
                     "    -r, --reset     Reset/clear the settings          \n"
                     "    -c, --contact   Contact the lead developer        \n"
                     "    -v, --version   Display the application version   \n"
                     "    -w, --website   Open a web site of this project   \n";

//------------------------------------------------------------------------------
// Download joystick drivers if needed
//------------------------------------------------------------------------------

static void DownloadXboxDrivers()
{
#ifdef Q_OS_MAC
    QSettings settings (APP_COMPANY, APP_DSPNAME);

    if (settings.value ("FirstLaunch", true).toBool()) {
        QMessageBox box;
        box.setIcon (QMessageBox::Question);
        box.setStandardButtons (QMessageBox::Yes | QMessageBox::No);
        box.setDefaultButton (QMessageBox::Yes);

        box.setWindowTitle (QObject::tr ("Download Joystick Drivers"));

        box.setText (QObject::tr (
                         "Do you want to install a driver "
                         "for Xbox joysticks?"));

        box.setInformativeText (QObject::tr (
                                    "Clicking \"Yes\" will open a web "
                                    "browser to download the drivers"));

        settings.setValue ("FirstLaunch", false);

        if (box.exec() == QMessageBox::Yes)
            QDesktopServices::openUrl (QUrl (
                                           "https://github.com/360Controller/"
                                           "360Controller/releases/latest"));
    }
#endif
}

//------------------------------------------------------------------------------
// Utility functions
//------------------------------------------------------------------------------

static void showHelp()
{
    qDebug() << HELP.toStdString().c_str();
}

static void resetSettings()
{
    QSettings (APP_COMPANY, APP_DSPNAME).clear();
    qDebug() << "QDriverStation settings cleared!";
}

static void contact()
{
    QString url = "mailto:" + CONTACT_URL;
    QDesktopServices::openUrl (QUrl (url));
    qDebug() << WEBS.arg (url).toStdString().c_str();
}

static void reportBug()
{
    QDesktopServices::openUrl (QUrl (APP_REPBUGS));
    qDebug() << WEBS.arg (APP_REPBUGS).toStdString().c_str();
}

static void openWebsite()
{
    QDesktopServices::openUrl (QUrl (APP_WEBSITE));
    qDebug() << WEBS.arg (APP_WEBSITE).toStdString().c_str();
}

static void showVersion()
{
    QString appver = APP_DSPNAME + " version " + APP_VERSION;
    QString author = "Written by Alex Spataru <" + CONTACT_URL + ">";

    qDebug() << appver.toStdString().c_str();
    qDebug() << author.toStdString().c_str();
}

//------------------------------------------------------------------------------
// Application init
//------------------------------------------------------------------------------

int main (int argc, char* argv[])
{
    /* Fix scalling issues on Windows */
#ifndef Q_OS_WIN
    QApplication::setAttribute (Qt::AA_EnableHighDpiScaling);
#else
    QApplication::setAttribute (Qt::AA_DisableHighDpiScaling);
#endif

    /* Set application info */
    QApplication::setApplicationName    (APP_DSPNAME);
    QApplication::setOrganizationName   (APP_COMPANY);
    QApplication::setApplicationVersion (APP_VERSION);
    QApplication::setOrganizationDomain (APP_WEBSITE);

    /* Initialize application */
    QString arguments;
    QApplication app (argc, argv);

    /* Read command line arguments */
    if (app.arguments().count() >= 2)
        arguments = app.arguments().at (1);

    /* We have some arguments, read them */
    if (!arguments.isEmpty() && arguments.startsWith ("-")) {
        if (arguments == "-b" || arguments == "--bug")
            reportBug();

        else if (arguments == "-r" || arguments == "--reset")
            resetSettings();

        else if (arguments == "-c" || arguments == "--contact")
            contact();

        else if (arguments == "-v" || arguments == "--version")
            showVersion();

        else if (arguments == "-w" || arguments == "--website")
            openWebsite();

        else
            showHelp();

        return EXIT_SUCCESS;
    }

    /* Start the initialization time clock */
    QTime timer;
    timer.start();

    /* Initialize OS variables */
    bool isMac = false;
    bool isUnx = false;
    bool isWin = false;

    /* Let QML know the operating system */
#if defined Q_OS_MAC
    isMac = true;
#elif defined Q_OS_WIN
    isWin = true;
#else
    isUnx = true;
#endif

    /* Install the LibDS event logger */
    DSEventLogger* dslogger = DSEventLogger::getInstance();
    qInstallMessageHandler (dslogger->messageHandler);

    /* Initialize application modules */
    Beeper beeper;
    Utilities utilities;
    Shortcuts shortcuts;
    Dashboards dashboards;
    QJoysticks* qjoysticks = QJoysticks::getInstance();
    DriverStation* driverstation = DriverStation::getInstance();

    /* Configure the shortcuts handler and start the DS */
    app.installEventFilter (&shortcuts);
    driverstation->declareQML();
    driverstation->start();

    /* Load the QML interface */
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty ("cIsMac",        isMac);
    engine.rootContext()->setContextProperty ("cIsUnix",       isUnx);
    engine.rootContext()->setContextProperty ("cIsWindows",    isWin);
    engine.rootContext()->setContextProperty ("Beeper",        &beeper);
    engine.rootContext()->setContextProperty ("QJoysticks",    qjoysticks);
    engine.rootContext()->setContextProperty ("Utilities",     &utilities);
    engine.rootContext()->setContextProperty ("cDashboard",    &dashboards);
    engine.rootContext()->setContextProperty ("appDspName",    APP_DSPNAME);
    engine.rootContext()->setContextProperty ("appVersion",    APP_VERSION);
    engine.rootContext()->setContextProperty ("appWebsite",    APP_WEBSITE);
    engine.rootContext()->setContextProperty ("appRepBugs",    APP_REPBUGS);
    engine.rootContext()->setContextProperty ("DSLogger",      dslogger);
    engine.rootContext()->setContextProperty ("DS",            driverstation);
    engine.load (QUrl (QStringLiteral ("qrc:/qml/main.qml")));

    /* QML loading failed, exit the application */
    if (engine.rootObjects().isEmpty())
        return EXIT_FAILURE;

    /* Tell user how much time was needed to initialize the app */
    qDebug() << "Initialized in " << timer.elapsed() << "milliseconds";

    /* Ask first-timers to download the xbox drivers */
    DownloadXboxDrivers();

    /* Run normally */
    return app.exec();
}
