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

#include <QScreen>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QApplication>

//=============================================================================
// Application includes
//=============================================================================

#include "InfoWindow.h"
#include "ConsoleWidget.h"
#include "Global/Global.h"

//=============================================================================
// MessagesWindow::MessagesWindow
//=============================================================================

InfoWindow::InfoWindow()
{
    m_tabWidget     = new QTabWidget    (this);
    m_consoleWidget = new ConsoleWidget (this);
    m_layout        = new QVBoxLayout   (this);

    QMargins margins = QMargins (DPI_SCALE (10),
                                 DPI_SCALE (10),
                                 DPI_SCALE (10),
                                 DPI_SCALE  (0));

    m_consoleWidget->layout()->setContentsMargins (MAIN_MARGINS());
    m_tabWidget->addTab          (m_consoleWidget, tr ("Console"));
    m_layout->setContentsMargins (margins);
    m_layout->addWidget          (m_tabWidget);
}

//=============================================================================
// MessagesWindow::~MessagesWindow
//=============================================================================

InfoWindow::~InfoWindow()
{
    delete m_layout;
    delete m_tabWidget;
    delete m_consoleWidget;
}

//=============================================================================
// MessagesWindow::console
//=============================================================================

ConsoleWidget* InfoWindow::console() const
{
    return m_consoleWidget;
}

//=============================================================================
// MessagesWindow::setShowDocked
//=============================================================================

void InfoWindow::showDocked (int displacement)
{
    hide();
    move (0, 0);
    setWindowFlags (Qt::FramelessWindowHint);

    resize (qApp->primaryScreen()->availableSize().width(),
            qApp->primaryScreen()->availableSize().height() - displacement);

    show();
}
