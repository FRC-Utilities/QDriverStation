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

#include <QLayout>
#include <QMessageBox>
#include <QApplication>
#include <QDesktopWidget>
#include <LibDS/Core/Timers.h>

#include "SmartWindow.h"
#include "Global/Settings.h"

//=============================================================================
// SmartWindow::SmartWindow
//=============================================================================

SmartWindow::SmartWindow() {
    m_closingDown = false;
    m_useFixedSize = true;
    m_promptOnQuit = true;

    ResizeToFit();
    connect (DS_Timers::GetInstance(), SIGNAL (Timeout20()),
             this,                     SLOT   (ResizeToFit()));
}

//=============================================================================
// SmartWindow::IsDocked
//=============================================================================

bool SmartWindow::IsDocked() {
    return m_windowMode == kDocked;
}

//=============================================================================
// SmartWindow::UsesFixedSize
//=============================================================================

bool SmartWindow::UsesFixedSize() {
    return m_useFixedSize;
}

//=============================================================================
// SmartWindow::PromptOnQuit
//=============================================================================

bool SmartWindow::PromptOnQuit() {
    return m_promptOnQuit;
}

//=============================================================================
// SmartWindow::moveEvent
//=============================================================================

void SmartWindow::moveEvent (QMoveEvent* e) {
    e->accept();

    if (!IsDocked()) {
        Settings::Set ("x", x());
        Settings::Set ("y", y());
    }
}

//=============================================================================
// SmartWindow::closeEvent
//=============================================================================

void SmartWindow::closeEvent (QCloseEvent* e) {
    /* User already confirmed his/her choice */
    if (m_closingDown)
        return;

    /* Ask user for confirmation on quit */
    if (m_promptOnQuit) {
        QString name = qApp->applicationName();

        QMessageBox box;
        box.setIcon (QMessageBox::Question);
        box.setStandardButtons (QMessageBox::Yes | QMessageBox::No);
        box.setDefaultButton (QMessageBox::Yes);
        box.setText (tr ("Are you sure you want to quit the %1?").arg (name));

        /* User changed his/her mind */
        if (box.exec() != QMessageBox::Yes) {
            e->ignore();
            return;
        }

        /* Ensure that this message is not shown twice in a row */
        else
            m_closingDown = true;
    }

    /* Close MainWindow and any other window that is open */
    e->accept();
    qApp->quit();
}

//=============================================================================
// SmartWindow::UseFixedSize
//=============================================================================

void SmartWindow::UseFixedSize (bool fixed) {
    m_useFixedSize = fixed;
}

//=============================================================================
// SmartWindow::EnablePromptOnQuit
//=============================================================================

void SmartWindow::EnablePromptOnQuit (bool prompt) {
    m_promptOnQuit = prompt;
}

//=============================================================================
// SmartWindow::SetWindowMode
//=============================================================================

void SmartWindow::SetWindowMode (const WindowMode& mode) {
    hide();
    m_windowMode = mode;
    Settings::Set ("Docked", IsDocked());

    /* Enable window borders and resize to minimum size */
    if (mode == kNormal) {
        QDesktopWidget w;
        int dx = w.width() / 9;
        int dy = w.height() / 2;

        move (Settings::Get ("x", dx).toInt(),
              Settings::Get ("y", dy).toInt());

        setMinimumSize (0, 0);
        setMaximumSize (0, 0);

        setWindowFlags (Qt::WindowTitleHint
                        | Qt::WindowCloseButtonHint
                        | Qt::WindowMinimizeButtonHint);
    }

    /* Disable window borders */
    else if (mode == kDocked)
        setWindowFlags (Qt::FramelessWindowHint);

    showNormal();
    ResizeToFit();
}

//=============================================================================
// SmartWindow::ResizeToFit
//=============================================================================

void SmartWindow::ResizeToFit() {
    /* 'Dock' the window at the bottom and extend it to the sides */
    if (IsDocked()) {
        QDesktopWidget w;
        setFixedHeight (size().height());
        setFixedWidth (w.availableGeometry().width());
        move (0, w.availableGeometry().height() - height());
        layout()->setSizeConstraint (QLayout::SetNoConstraint);
    }

    /* Show the window normally */
    else {
        resize (0, 0);
        setFixedSize (size());
        layout()->setSizeConstraint (QLayout::SetMinimumSize);
    }
}
