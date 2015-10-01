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

#include <QUrl>
#include <QObject>
#include <QMessageBox>
#include <QInputDialog>
#include <QDesktopServices>

#include "Settings.h"
#include "CpuUsage.h"
#include "InitTasks.h"

#define URL_XBOX_DRIVER  "https://github.com/d235j/360Controller/releases"
#define URL_BONJOUR_MDNS "https://support.apple.com/kb/dl999?locale=en_US"

int InitTasks::getTeamNumber() {
    /* First launch, ask for team number */
    if (Settings::get ("First Launch", true).toBool())
        return QInputDialog::getInt (0, 0,
                                     QObject::tr ("Input team number:"), 0, 0,
                                     9999);

    /* Retrieve team number from settings */
    else
        return Settings::get ("Team ID", 0).toInt();
}

void InitTasks::executeFirstRunTasks() {
    CpuUsage::init();

    /* Not first run, nothing to do here *flies away* */
    if (!Settings::get ("First Launch", true).toBool())
        return;

    /* Download Xbox 360 controller drivers for Mac OS X*/
#if defined __APPLE__
    QMessageBox box;
    box.setIcon (QMessageBox::Information);
    box.setStandardButtons (QMessageBox::Yes | QMessageBox::No);
    box.setDefaultButton (QMessageBox::Yes);
    box.setText ("<h3>" + QObject::tr ("Download extra drivers?") + "</h3>");
    box.setInformativeText (QObject::tr ("Do you want to download a driver "
                                         "for Xbox 360 joysticks?"));

    if (box.exec() == QMessageBox::Yes)
        QDesktopServices::openUrl (QUrl (URL_XBOX_DRIVER));
#endif

    /* Download Bonjour for Windows to get mDNS working */
#if defined __WIN32 || defined __WIN64
    QMessageBox box;
    box.setIcon (QMessageBox::Information);
    box.setStandardButtons (QMessageBox::Yes | QMessageBox::No);
    box.setDefaultButton (QMessageBox::Yes);
    box.setText ("<h3>" + QObject::tr ("Install mDNS service for Windows?") +
                 "</h3>");
    box.setInformativeText (QObject::tr ("Do you want to download Bonjour to "
                                         "be able to communicate with the robot?"
                                         "\n\nIf you have the official "
                                         "FRC DriverStation installed, this "
                                         "step is not required."));

    if (box.exec() == QMessageBox::Yes)
        QDesktopServices::openUrl (QUrl (_BONJOUR_URL));
#endif

    /* Ensure that this code does not run again */
    Settings::set ("First Launch", false);
}
