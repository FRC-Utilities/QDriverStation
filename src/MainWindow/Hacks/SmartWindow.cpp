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
#include <QLayout>
#include <QMessageBox>
#include <QApplication>
#include <QDesktopWidget>

#include "SmartWindow.h"
#include "Global/Settings.h"

//=============================================================================
// SmartWindow::SmartWindow
//=============================================================================

SmartWindow::SmartWindow()
{
    resizeToFit();
    m_useFixedSize = true;
}

//=============================================================================
// SmartWindow::IsDocked
//=============================================================================

bool SmartWindow::isDocked()
{
    return m_windowMode == kDocked;
}

//=============================================================================
// SmartWindow::UsesFixedSize
//=============================================================================

bool SmartWindow::usesFixedSize()
{
    return m_useFixedSize;
}

//=============================================================================
// SmartWindow::moveEvent
//=============================================================================

void SmartWindow::moveEvent (QMoveEvent* e)
{
    e->accept();

    if (!isDocked())
        {
            Settings::set ("x", x());
            Settings::set ("y", y());
        }
}

//=============================================================================
// SmartWindow::UseFixedSize
//=============================================================================

void SmartWindow::setUseFixedSize (bool fixed)
{
    m_useFixedSize = fixed;
}

//=============================================================================
// SmartWindow::SetWindowMode
//=============================================================================

void SmartWindow::setWindowMode (const WindowMode& mode)
{
    hide();
    m_windowMode = mode;

    /* Enable window borders and resize to minimum size */
    if (mode == kNormal)
        {
            QDesktopWidget w;
            int dx = w.width() / 9;
            int dy = w.height() / 2;

            move (Settings::get ("x", dx).toInt(),
                  Settings::get ("y", dy).toInt());

            resize (minimumSizeHint());
            setFixedSize (minimumSizeHint());
            setWindowFlags (Qt::WindowTitleHint
                            | Qt::WindowCloseButtonHint
                            | Qt::WindowMinimizeButtonHint);
        }

    /* Disable window borders */
    else if (mode == kDocked)
        setWindowFlags (Qt::FramelessWindowHint);

    show();
}

//=============================================================================
// SmartWindow::ResizeToFit
//=============================================================================

void SmartWindow::resizeToFit()
{
    /* 'Dock' the window at the bottom and extend it to the sides */
    if (isDocked())
        {
            QDesktopWidget w;
            setFixedHeight (size().height());
            setFixedWidth (w.availableGeometry().width());
            move (0, w.availableGeometry().height() - height());
            layout()->setSizeConstraint (QLayout::SetNoConstraint);
        }

    /* Show the window normally */
    else
        {
            resize (minimumSizeHint());
            setFixedSize (minimumSizeHint());
            layout()->setSizeConstraint (QLayout::SetMinimumSize);
        }

    QTimer::singleShot (100, Qt::CoarseTimer, this, SLOT (resizeToFit()));
}
