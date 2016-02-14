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

#include "Global/Global.h"
#include "Global/Beeper.h"
#include "Global/AppTheme.h"
#include "Global/Settings.h"
#include "Global/Languages.h"

#include "Updater/Updater.h"
#include "MainWindow/MainWindow.h"

//=============================================================================
// Called when a widget is clicked
//=============================================================================

void BEEP()
{
    BEEPER()->beep (440, 100);
}

//=============================================================================
// Avoid logging Qt warnings about our UI hacks
//=============================================================================

void messageHandler (QtMsgType type,
                     const QMessageLogContext& context,
                     const QString& msg)
{
    QByteArray localMsg = msg.toLocal8Bit();

    switch (type)
        {
        case QtDebugMsg:
            fprintf (stderr,
                     "DEBUG: %s (%s:%u, %s)\n",
                     localMsg.constData(),
                     context.file,
                     context.line,
                     context.function);
            break;
        case QtInfoMsg:
            fprintf (stderr,
                     "INFO: %s (%s:%u, %s)\n",
                     localMsg.constData(),
                     context.file,
                     context.line,
                     context.function);
            break;
        case QtWarningMsg:
            break;
        case QtCriticalMsg:
            fprintf (stderr,
                     "CRITICAL: %s (%s:%u, %s)\n",
                     localMsg.constData(),
                     context.file,
                     context.line,
                     context.function);
        case QtFatalMsg:
            fprintf (stderr,
                     "FATAL: %s (%s:%u, %s)\n",
                     localMsg.constData(),
                     context.file,
                     context.line,
                     context.function);
            break;
        }
}

//=============================================================================
// Main entry-point of the application
//=============================================================================

int main (int argc, char* argv[])
{
    qInstallMessageHandler (messageHandler);
    DS_LogMessage (kInfoLevel, "Starting application....");

    /* Configure application information */
    QApplication app (argc, argv);
    app.setApplicationVersion ("0.15");
    app.setOrganizationName   ("WinT 3794");
    app.setApplicationName    ("QDriverStation");
    app.setOrganizationDomain ("www.wint3794.org");
    app.installTranslator     (Languages::translator());

    /* Create the main window and check for updates */
    app.setFont (Languages::appFont());
    Updater updater;
    MainWindow mainwindow;

    /* Repeat this line to be sure that font is applied on everything */
    GLOBAL_INIT();
    AppTheme::init();
    app.setFont (Languages::appFont());

    /* Avoid compilation warnings */
    Q_UNUSED (updater);
    Q_UNUSED (mainwindow);

    /* Beep whenever a button or checkbox is clicked */
    foreach (QWidget* widget, app.allWidgets())
        {
            /* Do the conversions */
            QSpinBox*    spin   = qobject_cast<QSpinBox*> (widget);
            QCheckBox*   check  = qobject_cast<QCheckBox*> (widget);
            QComboBox*   combo  = qobject_cast<QComboBox*> (widget);
            QTabWidget*  tabwid = qobject_cast<QTabWidget*> (widget);
            QPushButton* button = qobject_cast<QPushButton*> (widget);

            /* The widget is a spin box */
            if (spin != Q_NULLPTR)
                QObject::connect (spin,
                                  static_cast<void (QSpinBox::*) (int)
                                  > (&QSpinBox::valueChanged),
                                  BEEP);

            /* The widget is a combo box */
            else if (combo != Q_NULLPTR)
                QObject::connect (combo,
                                  static_cast<void (QComboBox::*) (int)
                                  > (&QComboBox::currentIndexChanged),
                                  BEEP);

            /* The widget is a check box */
            else if (check != Q_NULLPTR)
                QObject::connect (check, &QCheckBox::clicked, BEEP);

            /* The widget is a tab widget */
            else if (tabwid != Q_NULLPTR)
                QObject::connect (tabwid, &QTabWidget::currentChanged, BEEP);

            /* The widget is a button */
            else if (button != Q_NULLPTR)
                QObject::connect (button, &QPushButton::clicked, BEEP);
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

    return app.exec();
}
