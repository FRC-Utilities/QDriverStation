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
// Application includes
//=============================================================================

#include "InfoWindow.h"
#include "InfoWidget.h"
#include "Global/Global.h"
#include "Global/Settings.h"

//=============================================================================
// InfoWindow::InfoWindow
//=============================================================================

InfoWindow::InfoWindow()
{
    /* Create widgets */
    m_infoWidget = new InfoWidget (this);

    /* Configure layout */
    setLayout (m_infoWidget->layout());
    setContentsMargins (MAIN_MARGINS());

    /* Set window geometry */
    setWindowTitle (tr ("Dashboard"));
    setMinimumHeight (DPI_SCALE (400));
    move (Settings::get ("InfoWindow X", 100).toInt(),
          Settings::get ("InfoWindow Y", 100).toInt());
}

//=============================================================================
// InfoWindow::moveEvent
//=============================================================================

void InfoWindow::moveEvent (QMoveEvent* event)
{
    event->accept();
    Settings::set ("InfoWindow X", x());
    Settings::set ("InfoWindow Y", y());
}

//=============================================================================
// InfoWindow::console
//=============================================================================

Console* InfoWindow::console() const
{
    return m_infoWidget->console();
}
