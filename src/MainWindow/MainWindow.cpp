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

#include <SDL.h>
#include <QTimer>
#include <QScreen>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QApplication>

//=============================================================================
// Application includes
//=============================================================================

#include "MainWindow.h"
#include "Utilities/Global.h"

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
    m_docked   = false;
    m_closing  = false;

    m_central  = new QWidget     (this);
    m_status   = new Status      (m_central);
    m_buttons  = new Buttons     (m_central);
    m_leftTab  = new LeftTab     (m_central);
    m_rightTab = new RightTab    (m_central);
    m_layout   = new QHBoxLayout (m_central);

    m_layout->addWidget          (m_leftTab);
    m_layout->addWidget          (m_status);
    m_layout->addWidget          (m_rightTab);
    m_layout->addWidget          (m_buttons);
    m_layout->setStretch         (0, 0);
    m_layout->setStretch         (1, 0);
    m_layout->setStretch         (2, 1);
    m_layout->setStretch         (3, 0);
    m_layout->setSpacing         (DPI_SCALE (5));

    connect (DS(),      SIGNAL   (initialized()),
             this,        SLOT   (displayWindow()));
    connect (m_leftTab, SIGNAL   (showDocked()),
             this,        SLOT   (showDocked()));
    connect (m_leftTab, SIGNAL   (showNormal()),
             this,        SLOT   (showUnDocked()));
    connect (m_leftTab, SIGNAL   (requestErrorAnimation()),
             m_status,    SLOT   (doErrorAnimation()));
    connect (qApp,      SIGNAL   (aboutToQuit()),
             this,        SLOT   (quitSound()));

    setUseFixedSize              (true);
    setCentralWidget             (m_central);
    setWindowTitle               (qApp->applicationName()
                                  + QString (" ")
                                  + qApp->applicationVersion());
}

//=============================================================================
// MainWindow::~MainWindow
//=============================================================================

MainWindow::~MainWindow()
{
    foreach (QWidget* widget, findChildren<QWidget*>())
        widget->deleteLater();
}

//=============================================================================
// MainWindow::closeEvent
//=============================================================================

void MainWindow::closeEvent (QCloseEvent* event)
{
    if (!m_closing && Settings::get ("Prompt on Quit", true).toBool())
        {
            int ans = QMessageBox::question (Q_NULLPTR,
                                             tr ("QDriverStation"),
                                             tr ("Are you sure you want to "
                                                 "exit the QDriverStation?"));

            if (ans == QMessageBox::No)
                {
                    event->ignore();
                    return;
                }

            else
                m_closing = true;
        }

    event->accept();
}

//=============================================================================
// MainWindow::showNormal
//=============================================================================

void MainWindow::showUnDocked()
{
    setWindowMode (kNormal);
    Settings::set ("Docked", false);
}

//=============================================================================
// MainWindow::quitSound
//=============================================================================

void MainWindow::quitSound()
{
    SDL_Delay (500);
    BEEPER()->beep (220, 100);
    BEEPER()->beep (440, 100);;
}

//=============================================================================
// MainWindow::showDocked
//=============================================================================

void MainWindow::showDocked()
{
    setWindowMode (kDocked);
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
    bool docked = Settings::get ("Docked", false).toBool();
    setWindowMode (docked ? kDocked : kNormal);
    QTimer::singleShot (100, Qt::CoarseTimer, this, SLOT (startUpSound()));
}
