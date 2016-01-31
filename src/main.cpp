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

//=============================================================================
// System includes
//=============================================================================

#include <QObject>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QTabWidget>
#include <QPushButton>
#include <QApplication>
#include <QInputDialog>

//=============================================================================
// Application includes
//=============================================================================

#include "Updater/Updater.h"
#include "Utilities/Global.h"
#include "Utilities/AppTheme.h"
#include "Utilities/Languages.h"
#include "Utilities/SoundPlayer.h"
#include "MainWindow/MainWindow.h"

//=============================================================================
// Main entry-point of the application
//=============================================================================

int main (int argc, char* argv[])
{
    /* Configure application information */
    QApplication app (argc, argv);
    app.setApplicationVersion ("0.14");
    app.setOrganizationName   ("WinT 3794");
    app.setApplicationName    ("QDriverStation");
    app.setOrganizationDomain ("www.wint3794.org");
    app.installTranslator     (Languages::translator());
    app.setFont               (Languages::appFont());

    /* */
    AppTheme::init();
    GLOBAL_INIT();

    /* Create the main window and check for updates */
    Updater updater;
    MainWindow mainwindow;

    /* Avoid compilation warnings */
    Q_UNUSED (updater);
    Q_UNUSED (mainwindow);

    /* Beep whenever a button or checkbox is clicked */
    SoundPlayer soundPlayer;
    QString family = Languages::appFont().family();
    foreach (QWidget* widget, app.allWidgets())
        {
            /* Apply our custom font [hack] */
            if (widget->font().family() != "FontAwesome")
                {
                    QFont font = widget->font();
                    font.setFamily (family);
                    widget->setFont (font);
                }

            /* Do the conversions */
            QSpinBox*    spin   = qobject_cast<QSpinBox*> (widget);
            QCheckBox*   check  = qobject_cast<QCheckBox*> (widget);
            QComboBox*   combo  = qobject_cast<QComboBox*> (widget);
            QTabWidget*  tabwid = qobject_cast<QTabWidget*> (widget);
            QPushButton* button = qobject_cast<QPushButton*> (widget);

            /* The widget is a spin box */
            if (spin != Q_NULLPTR)
                {
                    QObject::connect (spin, SIGNAL (valueChanged (int)),
                                      &soundPlayer, SLOT (actionBeep (int)));
                }

            /* The widget is a combo box */
            else if (combo != Q_NULLPTR)
                {
                    QObject::connect (combo, SIGNAL (currentIndexChanged (int)),
                                      &soundPlayer, SLOT (actionBeep (int)));
                }

            /* The widget is a check box */
            else if (check != Q_NULLPTR)
                {
                    QObject::connect (check, SIGNAL (clicked (bool)),
                                      &soundPlayer, SLOT (actionBeep (bool)));
                }

            /* The widget is a tab widget */
            else if (tabwid != Q_NULLPTR)
                {
                    QObject::connect (tabwid, SIGNAL (currentChanged (int)),
                                      &soundPlayer, SLOT (actionBeep (int)));
                }

            /* The widget is a button */
            else if (button != Q_NULLPTR)
                {
                    QObject::connect (button, SIGNAL (clicked (bool)),
                                      &soundPlayer, SLOT (actionBeep (bool)));
                }
        }

    /* Ask for team number on first launch */
    if (Settings::get ("First launch", true).toBool())
        {
            DS()->setTeamNumber (QInputDialog::getInt (Q_NULLPTR,
                                 QObject::tr ("QDriverStation"),
                                 QObject::tr ("Please input your team number:"),
                                 0, 0, 9999, 1, 0,
                                 Qt::WindowStaysOnTopHint));
            Settings::set ("First launch", false);
        }

    /* May the force be with you */
    return app.exec();
}
