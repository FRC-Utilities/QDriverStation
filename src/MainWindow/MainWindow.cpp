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

#include <QTimer>
#include <QScreen>
#include <QMoveEvent>
#include <QCloseEvent>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QApplication>

//==================================================================================================
// Application includes
//==================================================================================================

#include "MainWindow.h"
#include "Global/Global.h"
#include "Global/Beeper.h"
#include "Global/Settings.h"
#include "Dashboards/Dashboards.h"

#include "SettingsWindow/SettingsWindow.h"
#include "VJoystickWindow/VJoystickWindow.h"

#include "Components/Status.h"
#include "Components/Buttons.h"
#include "Components/LeftTab.h"
#include "Components/RightTab.h"
#include "Components/Operator.h"

//==================================================================================================
// Hacks to get the docked window working
//==================================================================================================

QSize screen;
QSize desktop;

//==================================================================================================
// MainWindow::MainWindow
//==================================================================================================

MainWindow::MainWindow() {
    setAttribute  (Qt::WA_QuitOnClose);
    DS::log (DS::kInfoLevel, "Creating MainWindow....");

    /* Get screen and desktop sizes */
    screen  = QApplication::primaryScreen()->size();
    desktop = QApplication::primaryScreen()->availableSize();

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
    setWindowTitle                (QString ("%1 - Version %2").arg (qApp->applicationName(),
                                   qApp->applicationVersion()));

    /* Signals/slots */
    connect (m_leftTab, &LeftTab::showDocked, this, &MainWindow::showDocked);
    connect (m_leftTab, &LeftTab::showUnDocked, this, &MainWindow::showUnDocked);
    connect (QDS(), &DriverStation::initialized, this, &MainWindow::displayWindow);
    connect (m_leftTab, &LeftTab::requestErrorAnimation, m_status, &Status::doErrorAnimation);

    /* Move window to saved position */
    move (Settings::get ("MainWindow X", 100).toInt(), Settings::get ("MainWindow Y", 100).toInt());

    DS::log (DS::kInfoLevel, "MainWindow OK");
}

//==================================================================================================
// MainWindow::~MainWindow
//==================================================================================================

MainWindow::~MainWindow() {
    DS::log (DS::kInfoLevel, "MainWindow: MW destroyed");
}

//==================================================================================================
// MainWindow::moveEvent
//==================================================================================================

void MainWindow::moveEvent (QMoveEvent* event) {
    event->accept();

    if (!m_docked) {
        Settings::set ("MainWindow X", x());
        Settings::set ("MainWindow Y", y());
    }
}


//==================================================================================================
// MainWindow::closeEvent
//==================================================================================================

void MainWindow::closeEvent (QCloseEvent* event) {
    if (!m_closing && Settings::get ("Prompt on Quit", true).toBool()) {
        QMessageBox box;
        box.setIcon (QMessageBox::Question);
        box.setStandardButtons (QMessageBox::Yes | QMessageBox::No);
        box.setDefaultButton (QMessageBox::Yes);
        box.setText (tr ("Are you sure you want to exit the "
                         "QDriverStation?"));

        if (box.exec() == QMessageBox::No) {
            event->ignore();
            return;
        }

        else
            m_closing = true;
    }

    event->accept();
}

//==================================================================================================
// MainWindow::showNormal
//==================================================================================================

void MainWindow::showUnDocked() {
    m_docked = false;

    Settings::set ("Docked", false);
    setWindowFlags (Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
    move (Settings::get ("MainWindow X", 100).toInt(), Settings::get ("MainWindow Y", 100).toInt());

    showNormal();
}

//==================================================================================================
// MainWindow::showDocked
//==================================================================================================

void MainWindow::showDocked() {
    m_docked = true;

#if defined Q_OS_WIN
    setWindowFlags (Qt::FramelessWindowHint);
#endif

    showNormal();
    Settings::set ("Docked", true);
}

//==================================================================================================
// MainWindow::startUpSound
//==================================================================================================

void MainWindow::startUpSound() {
    BEEPER()->beep (440, 100);
    BEEPER()->beep (220, 100);
}

//==================================================================================================
// MainWindow::displayWindow
//==================================================================================================

void MainWindow::displayWindow() {
    show();
    Settings::get ("Docked", false).toBool() ? showDocked() : showUnDocked();

    QTimer::singleShot (100, Qt::CoarseTimer, this, SLOT (updateSize()));
    QTimer::singleShot (100, Qt::CoarseTimer, this, SLOT (startUpSound()));
}

//==================================================================================================
// MainWindow::updateSize
//==================================================================================================

void MainWindow::updateSize() {
    if (m_docked) {
        setFixedWidth  (desktop.width());
        setFixedHeight (minimumSizeHint().height());
        move (screen.width() - desktop.width(), desktop.height() - height());
    }

    else
        setFixedSize (minimumSizeHint());

    QTimer::singleShot (100, Qt::CoarseTimer, this, SLOT (updateSize()));
}
