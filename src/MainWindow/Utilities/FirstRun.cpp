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
#include <QMessageBox>
#include <QInputDialog>
#include <QDesktopServices>

#include "CPU.h"
#include "FirstRun.h"
#include "Global/Settings.h"

const QString XBOX_URL =
    "http://files.tattiebogle.net/360/360ControllerInstall.dmg";

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
    /* Initialize the CPU querying process */
    CPU::init();

    /* Not first run, nothing to do here *flies away* */
    if (!Settings::get ("First Launch", true).toBool())
        return;

    /* Download Xbox 360 controller drivers for Mac OS X*/
#if defined Q_OS_MAC
    QMessageBox box;
    box.setIcon (QMessageBox::Information);
    box.setStandardButtons (QMessageBox::Yes | QMessageBox::No);
    box.setDefaultButton (QMessageBox::Yes);
    box.setText ("<h3>" + QObject::tr ("Download extra drivers?") + "</h3>");
    box.setInformativeText (QObject::tr ("Do you want to download a driver "
                                         "for Xbox 360 joysticks?"));

    if (box.exec() == QMessageBox::Yes)
        QDesktopServices::openUrl (QUrl (XBOX_URL));
#endif

    /* Ensure that this code does not run again */
    Settings::set ("First Launch", false);
}
