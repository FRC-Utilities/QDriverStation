/*
 * Copyright (c) 2015 WinT 3794 <http://wint3794.org>
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

#include <QIcon>
#include <QFontDatabase>

#include "Global/AssemblyInfo.h"
#include "MainWindow/MainWindow.h"

/* Custom appearance options for each operating system */
#if defined Q_OS_MAC
const int POINT_SIZE = 12;
const QIcon APP_ICON = QIcon ("");
#elif defined Q_OS_LINUX
const int POINT_SIZE = 12;
const QIcon APP_ICON = QIcon (":/icon.ico");
#else
const int POINT_SIZE = 8;
const QIcon APP_ICON = QIcon (":/icon.ico");
#endif

/**
 * Configures and runs the application
 */
int main (int argc, char* argv[]) {
    QApplication app (argc, argv);

    /* Load the application fonts */
    QFontDatabase::addApplicationFont (":/fonts/FontAwesome.otf");
    QFontDatabase::addApplicationFont (":/fonts/Inconsolata.otf");
    QFontDatabase::addApplicationFont (":/fonts/Quicksand-Bold.ttf");
    QFontDatabase::addApplicationFont (":/fonts/Quicksand-Regular.ttf");

    /* Configure application options */
    app.setWindowIcon (APP_ICON);
    app.setStyle      (QStyleFactory::create ("fusion"));
    app.setFont       (QFont ("Quicksand", POINT_SIZE, -1, false));

    /* Configure application information */
    app.setApplicationName    (AssemblyInfo::name());
    app.setApplicationVersion (AssemblyInfo::version());
    app.setOrganizationName   (AssemblyInfo::organization());
    app.setOrganizationDomain (AssemblyInfo::organizationDomain());

    /* The window will show itself when initialized */
    MainWindow window;
    Q_UNUSED (window);

    return app.exec();
}
