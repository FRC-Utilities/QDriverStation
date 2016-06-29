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
#include <QScreen>
#include <QApplication>
#include <QJoysticks.h>
#include <DriverStation.h>
#include <QSimpleUpdater.h>
#include <QQmlApplicationEngine>

//------------------------------------------------------------------------------
// Application includes
//------------------------------------------------------------------------------

#include "beeper.h"
#include "versions.h"
#include "shortcuts.h"
#include "utilities.h"
#include "dashboards.h"

//------------------------------------------------------------------------------
// Fix compilation issues in Windows
//------------------------------------------------------------------------------

#ifdef Q_OS_WIN
#ifdef main
#undef main
#endif
#endif

//------------------------------------------------------------------------------
// Mac-specific initialization code
//------------------------------------------------------------------------------

#ifdef Q_OS_MAC
#include <QObject>
#include <QSettings>
#include <QMessageBox>
#include <QDesktopServices>

void DownloadXboxDrivers() {
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
}
#endif

//------------------------------------------------------------------------------
// Main function
//------------------------------------------------------------------------------

int main (int argc, char* argv[]) {
    QTime time;
    time.start();

    /* Avoid UI scaling issues with Qt 5.6 */
#if QT_VERSION >= QT_VERSION_CHECK (5, 6, 0)
#if defined Q_OS_MAC
    QApplication::setAttribute (Qt::AA_EnableHighDpiScaling);
#else
    QApplication::setAttribute (Qt::AA_DisableHighDpiScaling);
#endif
#endif

    /* Start the application and register its information */
    QApplication app (argc, argv);
    app.setApplicationName    (APP_DSPNAME);
    app.setOrganizationName   (APP_COMPANY);
    app.setApplicationVersion (APP_VERSION);
    app.setOrganizationDomain (APP_WEBSITE);

    /* Configure the logging system */
    qInstallMessageHandler (DS_MESSAGE_HANDLER);
    qDebug() << "Initializing application...";

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

    /* Calculate the scale factor of the screen */
    qreal ratio = (app.primaryScreen()->physicalDotsPerInch() / 100) * 0.9;

    /* Scale factor is too small or OS is Mac */
    if (ratio < 1.2 || isMac)
        ratio = 1;

    /* Report the scale factor */
    qDebug() << "Scale factor set to:" << ratio;

    /* Initialize application modules */
    Beeper beeper;
    Utilities utilities;
    Shortcuts shortcuts;
    Dashboards dashboards;
    QJoysticks* qjoysticks = QJoysticks::getInstance();
    QSimpleUpdater* updater = QSimpleUpdater::getInstance();
    DriverStation* driverstation = DriverStation::getInstance();

    /* Shortcuts gets events before virtual joystick */
    qApp->installEventFilter (&shortcuts);

    /* Load the QML interface */
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty ("cRatio",        ratio);
    engine.rootContext()->setContextProperty ("cIsMac",        isMac);
    engine.rootContext()->setContextProperty ("cIsUnix",       isUnx);
    engine.rootContext()->setContextProperty ("cIsWindows",    isWin);
    engine.rootContext()->setContextProperty ("cBeeper",       &beeper);
    engine.rootContext()->setContextProperty ("cUpdater",      updater);
    engine.rootContext()->setContextProperty ("QJoysticks",    qjoysticks);
    engine.rootContext()->setContextProperty ("cUtilities",    &utilities);
    engine.rootContext()->setContextProperty ("cDashboard",    &dashboards);
    engine.rootContext()->setContextProperty ("appDspName",    APP_DSPNAME);
    engine.rootContext()->setContextProperty ("appVersion",    APP_VERSION);
    engine.rootContext()->setContextProperty ("cUpdaterUrl",   URL_UPDATER);
    engine.rootContext()->setContextProperty ("DriverStation", driverstation);
    engine.load (QUrl (QStringLiteral ("qrc:/qml/main.qml")));

    /* QML loading failed, exit the application */
    if (engine.rootObjects().isEmpty())
        return EXIT_FAILURE;

    /* Ask OS X users to download Xbox drivers */
#ifdef Q_OS_MAC
    DownloadXboxDrivers();
#endif

    /* Start the application event loop */
    qDebug() << "Initialized in " << time.elapsed() << "milliseconds";
    return app.exec();
}
