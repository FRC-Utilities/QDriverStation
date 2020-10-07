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
#include <QMessageBox>
#include <QApplication>
#include <QDesktopServices>
#include <QQmlApplicationEngine>

//------------------------------------------------------------------------------
// Library includes
//------------------------------------------------------------------------------

#include <stdio.h>
#include <iostream>

#include <QJoysticks.h>
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

static void WelcomeMessages()
{
    QSettings settings (APP_COMPANY, APP_DSPNAME);
    if (settings.value ("FirstLaunch", true).toBool()) {
        // Warn user that he/she is using the legacy version
        QMessageBox box;
        box.setIcon (QMessageBox::Information);
        box.setStandardButtons (QMessageBox::Ok);
        box.setWindowTitle (QObject::tr ("Legacy Version"));
        box.setText (QObject::tr ("You are using the legacy version of "
                                  "QDriverStation"));
        box.setInformativeText (QObject::tr ("This version only supports FRC "
                                             "2014-2016 robots and is no longer "
                                             "actively maintained!"));
        box.exec();

        // Download Xbox drivers on Mac
#ifdef Q_OS_MAC
        QMessageBox xboxDrivers;
        xboxDrivers.setIcon (QMessageBox::Question);
        xboxDrivers.setStandardButtons (QMessageBox::Yes | QMessageBox::No);
        xboxDrivers.setDefaultButton (QMessageBox::Yes);
        xboxDrivers.setWindowTitle (QObject::tr ("Download Joystick Drivers"));
        xboxDrivers.setText (QObject::tr (
                                 "Do you want to install a driver "
                                 "for Xbox joysticks?"));

        xboxDrivers.setInformativeText (QObject::tr (
                                            "Clicking \"Yes\" will open a web "
                                            "browser to download the drivers"));

        if (xboxDrivers.exec() == QMessageBox::Yes)
            QDesktopServices::openUrl (QUrl (
                                           "https://github.com/360Controller/"
                                           "360Controller/releases/latest"));
#endif

        settings.setValue ("FirstLaunch", false);
    }
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
    QElapsedTimer timer;
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
    DSEventLogger* CppDSLogger = DSEventLogger::getInstance();
    qInstallMessageHandler (CppDSLogger->messageHandler);

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
    engine.rootContext()->setContextProperty ("CppIsMac",       isMac);
    engine.rootContext()->setContextProperty ("CppIsUnix",      isUnx);
    engine.rootContext()->setContextProperty ("CppIsWindows",   isWin);
    engine.rootContext()->setContextProperty ("CppBeeper",      &beeper);
    engine.rootContext()->setContextProperty ("QJoysticks",     qjoysticks);
    engine.rootContext()->setContextProperty ("CppUtilities",   &utilities);
    engine.rootContext()->setContextProperty ("CppDashboard",   &dashboards);
    engine.rootContext()->setContextProperty ("CppAppDspName",  APP_DSPNAME);
    engine.rootContext()->setContextProperty ("CppAppVersion",  APP_VERSION);
    engine.rootContext()->setContextProperty ("CppAppWebsite",  APP_WEBSITE);
    engine.rootContext()->setContextProperty ("CppAppRepBugs",  APP_REPBUGS);
    engine.rootContext()->setContextProperty ("CppDSLogger",    CppDSLogger);
    engine.rootContext()->setContextProperty ("CppDS",          driverstation);
    engine.load (QUrl (QStringLiteral ("qrc:/qml/main.qml")));

    /* QML loading failed, exit the application */
    if (engine.rootObjects().isEmpty())
        return EXIT_FAILURE;

    /* Tell user how much time was needed to initialize the app */
    qDebug() << "Initialized in " << timer.elapsed() << "milliseconds";

    /* Warn first-timers that they are running the legacy version and
     * to download the xbox drivers on macOS */
    WelcomeMessages();

    /* Run normally */
    return app.exec();
}
