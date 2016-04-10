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

//==================================================================================================
// System includes
//==================================================================================================

#include <QObject>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QTabWidget>
#include <QPushButton>
#include <QApplication>
#include <QInputDialog>

//==================================================================================================
// Application includes
//==================================================================================================

#include "Global/Global.h"
#include "Global/Beeper.h"
#include "Global/AppTheme.h"
#include "Global/Settings.h"
#include "Global/Languages.h"

#include "Updater/Updater.h"
#include "MainWindow/MainWindow.h"

#undef main

//==================================================================================================
// Called when a widget is clicked
//==================================================================================================

void BEEP() {
    BEEPER()->beep (440, 100);
}

//==================================================================================================
// Loops through all the widgets and configures them to call the BEEP() function when clicked
//==================================================================================================

void ENABLE_SOUND_EFFECTS() {
    foreach (QWidget* widget, QApplication::allWidgets()) {
        QSpinBox* spin      = qobject_cast<QSpinBox*> (widget);
        QCheckBox* check    = qobject_cast<QCheckBox*> (widget);
        QComboBox* combo    = qobject_cast<QComboBox*> (widget);
        QTabWidget* tabwid  = qobject_cast<QTabWidget*> (widget);
        QPushButton* button = qobject_cast<QPushButton*> (widget);

        if (spin != Q_NULLPTR)
            QObject::connect (spin,
                              static_cast<void (QSpinBox::*) (int)
                              > (&QSpinBox::valueChanged),
                              BEEP);

        else if (combo != Q_NULLPTR)
            QObject::connect (combo,
                              static_cast<void (QComboBox::*) (int)
                              > (&QComboBox::currentIndexChanged),
                              BEEP);

        else if (check != Q_NULLPTR)
            QObject::connect (check, &QCheckBox::clicked, BEEP);

        else if (tabwid != Q_NULLPTR)
            QObject::connect (tabwid, &QTabWidget::currentChanged, BEEP);

        else if (button != Q_NULLPTR)
            QObject::connect (button, &QPushButton::clicked, BEEP);
    }

}

//==================================================================================================
// Main entry-point of the application
//==================================================================================================

int main (int argc, char* argv[]) {
    DS::log (DS::kInfoLevel, "Starting application....");

    /* Configure application information */
    QApplication app (argc, argv);
    QApplication::setApplicationVersion ("1.0.0");
    QApplication::setOrganizationName   ("WinT 3794");
    QApplication::setApplicationName    ("QDriverStation");
    QApplication::setOrganizationDomain ("www.wint3794.org");
    QApplication::installTranslator     (Languages::translator());
    QApplication::setFont               (Languages::appFont());

    /* Create the main window and check for updates */
    Updater updater;
    MainWindow mainwindow;

    /* Do not allow the compiler to bitch about unused variables */
    (void) app;
    (void) updater;
    (void) mainwindow;

    /* Initialize the application modules and configure the SDL-sound effects */
    GLOBAL_INIT();
    ENABLE_SOUND_EFFECTS();

    /* Load the application theme and the font (which is based on the language) */
    AppTheme::init();
    QApplication::setFont (Languages::appFont());

    /* Ask for team number on first launch */
    if (Settings::get ("First launch", true).toBool()) {
        QDS()->setTeamNumber (QInputDialog::getInt (Q_NULLPTR,
                              QObject::tr ("QDriverStation"),
                              QObject::tr ("Please input your team number:"),
                              0, 0, 9999, 1, 0,
                              Qt::WindowStaysOnTopHint));
        Settings::set ("First launch", false);
    }

    /* Start the application loop */
    return QApplication::exec();
}
