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

//=============================================================================
// System includes
//=============================================================================

#include <QTimer>
#include <QScreen>
#include <QMoveEvent>
#include <QCloseEvent>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QApplication>

//=============================================================================
// Application includes
//=============================================================================

#include "MainWindow.h"
#include "Global/Global.h"
#include "Global/Beeper.h"
#include "Global/Settings.h"
#include "Dashboards/Dashboards.h"

#include "Components/Status.h"
#include "Components/Buttons.h"
#include "Components/LeftTab.h"
#include "Components/RightTab.h"
#include "Components/Operator.h"

//=============================================================================
// MainWindow::MainWindow
//=============================================================================

MainWindow::MainWindow()
{
    DS_LogMessage (kInfoLevel, "Creating MainWindow....");

    /* Configure internal variables */
    m_docked   = false;
    m_closing  = false;

    /* Create widgets */
    m_central   = new QWidget     (this);
    m_status    = new Status      (m_central);
    m_buttons   = new Buttons     (m_central);
    m_leftTab   = new LeftTab     (m_central);
    m_rightTab  = new RightTab    (m_central);
    m_layout    = new QHBoxLayout (m_central);

    /* Add the common widgets */
    m_layout->addWidget           (m_leftTab);
    m_layout->addWidget           (m_status);
    m_layout->addWidget           (m_rightTab);
    m_layout->addWidget           (m_buttons);
    m_layout->setStretch          (0, 0);
    m_layout->setStretch          (1, 0);
    m_layout->setStretch          (2, 1);
    m_layout->setStretch          (3, 0);
    m_layout->setSpacing          (DPI_SCALE (5));
    m_layout->setContentsMargins  (MAIN_MARGINS());

    /* Configure window options */
    setCentralWidget              (m_central);
    m_central->setMinimumWidth    (m_central->minimumSizeHint().width());
    m_central->setFixedHeight     (m_central->minimumSizeHint().height());
    setWindowTitle                (QApplication::applicationName()
                                   + QString (" ")
                                   + QApplication::applicationVersion());

    /* Signals/slots */
    connect (DS(),      &DriverStation::initialized,
             this,      &MainWindow::displayWindow);
    connect (m_leftTab, &LeftTab::showDocked,
             this,      &MainWindow::showDocked);
    connect (m_leftTab, &LeftTab::showUnDocked,
             this,      &MainWindow::showUnDocked);
    connect (m_leftTab, &LeftTab::requestErrorAnimation,
             m_status,  &Status::doErrorAnimation);
    connect (qApp,      &QApplication::aboutToQuit,
             this,      &MainWindow::quitSound);

    /* Move window to saved position */
    move (Settings::get ("MainWindow X", 100).toInt(),
          Settings::get ("MainWindow Y", 100).toInt());

    DS_LogMessage (kInfoLevel, "MainWindow OK");

    /* Finally, init the Driver Station */
    DS()->init();
}

//=============================================================================
// MainWindow::moveEvent
//=============================================================================

void MainWindow::moveEvent (QMoveEvent* event)
{
    event->accept();

    if (!m_docked)
        {
            Settings::set ("MainWindow X", x());
            Settings::set ("MainWindow Y", y());
        }
}


//=============================================================================
// MainWindow::closeEvent
//=============================================================================

void MainWindow::closeEvent (QCloseEvent* event)
{
    if (!m_closing && Settings::get ("Prompt on Quit", true).toBool())
        {
            QMessageBox box;
            box.setIcon (QMessageBox::Question);
            box.setStandardButtons (QMessageBox::Yes | QMessageBox::No);
            box.setDefaultButton (QMessageBox::Yes);
            box.setText (tr ("Are you sure you want to exit the "
                             "QDriverStation?"));

            if (box.exec() == QMessageBox::No)
                {
                    event->ignore();
                    return;
                }

            else
                m_closing = true;
        }

    event->accept();
    QApplication::closeAllWindows();
}

//=============================================================================
// MainWindow::showNormal
//=============================================================================

void MainWindow::showUnDocked()
{
    m_docked = false;
    setWindowFlags (Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);

    move (Settings::get ("MainWindow X", 100).toInt(),
          Settings::get ("MainWindow Y", 100).toInt());

    setFixedSize (0, 0);
    Settings::set ("Docked", false);

    showNormal();
}

//=============================================================================
// MainWindow::quitSound
//=============================================================================

void MainWindow::quitSound()
{
    BEEPER()->beep (220, 100);
    BEEPER()->beep (440, 100);;
}

//=============================================================================
// MainWindow::showDocked
//=============================================================================

void MainWindow::showDocked()
{
    m_docked = true;

    /* Frameless windows are a headache for some window managers */
#if defined Q_OS_WIN
    setWindowFlags (Qt::FramelessWindowHint);
#endif

    showNormal();
    Settings::set ("Docked", true);
}

//=============================================================================
// MainWindow::startUpSound
//=============================================================================

void MainWindow::startUpSound()
{
    BEEPER()->beep (440, 100);
    BEEPER()->beep (220, 100);
}

//=============================================================================
// MainWindow::displayWindow
//=============================================================================

void MainWindow::displayWindow()
{
    showNormal();
    Settings::get ("Docked", false).toBool() ? showDocked() : showUnDocked();

    QTimer::singleShot (100, Qt::CoarseTimer, this, SLOT (updateSize()));
    QTimer::singleShot (100, Qt::CoarseTimer, this, SLOT (startUpSound()));
}

//=============================================================================
// MainWindow::updateSize
//=============================================================================

void MainWindow::updateSize()
{
    if (m_docked)
        {
            QSize desktop = QApplication::primaryScreen()->availableSize();

            setFixedWidth  (desktop.width());
            setFixedHeight (minimumSizeHint().height());

            move (0, desktop.height() - height());
        }

    else
        setFixedSize (minimumSizeHint());

    QTimer::singleShot (100, Qt::CoarseTimer, this, SLOT (updateSize()));
}
