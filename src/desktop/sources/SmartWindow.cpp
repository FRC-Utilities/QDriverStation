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

#include <QTimer>
#include <QMessageBox>
#include <QApplication>
#include <QDesktopWidget>

#include "Settings.h"
#include "SmartWindow.h"

#define _UPDATE_INTERVAL 100

//------------------------------------------------------------------------------
// Class initialization functions
//------------------------------------------------------------------------------

SmartWindow::SmartWindow()
{
    m_closingDown = false;
    m_useFixedSize = true;
    m_promptOnQuit = true;
    m_windowMode = Invalid;

    resizeToFit();
    setWindowMode (Settings::get ("Docked", false).toBool() ? Docked : Normal);
}

//------------------------------------------------------------------------------
// Functions that give information about the window
//------------------------------------------------------------------------------

bool SmartWindow::isDocked()
{
    return m_windowMode == Docked;
}

bool SmartWindow::isFixedSize()
{
    return m_useFixedSize;
}

bool SmartWindow::isPromptOnQuit()
{
    return m_promptOnQuit;
}

//------------------------------------------------------------------------------
// Functions that react to user events
//------------------------------------------------------------------------------

void SmartWindow::moveEvent (QMoveEvent* e)
{
    e->accept();

    if (!isDocked()) {
        Settings::set ("x", x());
        Settings::set ("y", y());
    }
}

void SmartWindow::closeEvent (QCloseEvent* e)
{
    if (m_promptOnQuit && !m_closingDown) {
        QString name = qApp->applicationName();

        QMessageBox box;
        box.setIcon (QMessageBox::Question);
        box.setStandardButtons (QMessageBox::Yes | QMessageBox::No);
        box.setDefaultButton (QMessageBox::Yes);
        box.setText (tr ("Are you sure you want to quit the %1?").arg (name));

        if (box.exec() != QMessageBox::Yes) {
            e->ignore();
            return;
        }

        else
            m_closingDown = true;
    }

    e->accept();
}

//------------------------------------------------------------------------------
// Functions that change the appearance and behaviour of the window
//------------------------------------------------------------------------------

void SmartWindow::setUseFixedSize (bool fixed)
{
    m_useFixedSize = fixed;
}

void SmartWindow::setPromptOnQuit (bool prompt)
{
    m_promptOnQuit = prompt;
}

void SmartWindow::setWindowMode (WindowMode mode)
{
    if (m_windowMode == mode)
        return;

    hide();
    m_windowMode = mode;
    Settings::set ("Docked", isDocked());

    if (mode == Normal) {
        QDesktopWidget w;
        int dx = w.width() / 9;
        int dy = w.height() / 2;

        move (Settings::get ("x", dx).toInt(),
              Settings::get ("y", dy).toInt());

        setMinimumSize (0, 0);
        setMaximumSize (0, 0);

        setWindowFlags (Qt::WindowTitleHint
                        | Qt::WindowCloseButtonHint
                        | Qt::WindowMinimizeButtonHint);
    }

    else if (mode == Docked)
        setWindowFlags (Qt::FramelessWindowHint);

    showNormal();
}

//------------------------------------------------------------------------------
// Functions that are called constantly with a timer
//------------------------------------------------------------------------------

void SmartWindow::resizeToFit()
{
    if (!isDocked() && m_useFixedSize) {
        resize (0, 0);
        setMinimumSize (size());
        setMaximumSize (size());
    }

    else if (isDocked()) {
        QDesktopWidget w;
        setMinimumWidth (w.width());
        setMaximumWidth (w.width());
        setMinimumHeight (size().height());
        setMaximumHeight (size().height());
        move (0, w.availableGeometry().height() - height());
    }

    QTimer::singleShot (_UPDATE_INTERVAL, this, SLOT (resizeToFit()));
}
