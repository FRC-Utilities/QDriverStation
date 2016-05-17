/*
 * Copyright (c) 2015-2016 WinT 3794 <http://wint3794.org>
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

//-----------------------------------------------------------------------------
// Qt includes
//-----------------------------------------------------------------------------

#include <QtQml>
#include <QScreen>
#include <QApplication>
#include <QQmlApplicationEngine>

//-----------------------------------------------------------------------------
// LibDS and joystick libraries
//-----------------------------------------------------------------------------

#include <LibDS.h>
#include <QJoysticks.h>

//-----------------------------------------------------------------------------
// Application includes
//-----------------------------------------------------------------------------

#include "beeper.h"
#include "updater.h"
#include "utilities.h"
#include "dashboards.h"

//-----------------------------------------------------------------------------
// Avoid issues with SDL
//-----------------------------------------------------------------------------

#if defined (QT_NEEDS_QMAIN)
#define QDS_MAIN qMain
#else
#define QDS_MAIN main
#endif

//-----------------------------------------------------------------------------
// Main entry point of the application
//-----------------------------------------------------------------------------

int QDS_MAIN (int argc, char* argv[]) {
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
    app.setApplicationVersion ("16.05");
    app.setApplicationDisplayName ("QDriverStation");
    app.setOrganizationName ("QDriverStation Developers");
    app.setOrganizationDomain ("http://qdriverstation.sf.net");

    /* Calculate the scale factor of the screen */
    qreal scaleRatio = (app.primaryScreen()->physicalDotsPerInch() / 100) * 0.9;

    /* Scale factor is too small */
    if (scaleRatio < 1.2)
        scaleRatio = 1;

    /* Mac already scales things */
#if defined Q_OS_MAC
    scaleRatio = 1;
#endif

    /* Initialize application systems */
    Beeper beeper;
    Updater updater;
    Utilities utilities;
    Dashboards dashboards;

    /* Load the QML interface */
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty ("cRatio", scaleRatio);
    engine.rootContext()->setContextProperty ("cBeeper", &beeper);
    engine.rootContext()->setContextProperty ("cUpdater", &updater);
    engine.rootContext()->setContextProperty ("cUtilities", &utilities);
    engine.rootContext()->setContextProperty ("cDashboards", &dashboards);
    engine.rootContext()->setContextProperty ("appName", app.applicationName());
    engine.rootContext()->setContextProperty ("appVersion", app.applicationVersion());
    engine.rootContext()->setContextProperty ("QJoysticks", QJoysticks::getInstance());
    engine.rootContext()->setContextProperty ("DriverStation", DriverStation::getInstance());
    engine.load (QUrl (QStringLiteral ("qrc:/qml/main.qml")));

    /* QML loading failed, exit the application */
    if (engine.rootObjects().isEmpty())
        return EXIT_FAILURE;

    /* Start the application event loop */
    return app.exec();
}
