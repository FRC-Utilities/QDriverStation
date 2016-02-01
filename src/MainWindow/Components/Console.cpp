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

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

//=============================================================================
// Application includes
//=============================================================================

#include "Console.h"
#include "Utilities/Global.h"
#include "Utilities/Languages.h"
#include "MessagesWindow/ConsoleWidget.h"
#include "MessagesWindow/MessagesWindow.h"

//=============================================================================
// Console::Console
//=============================================================================

Console::Console (QWidget* parent) : QWidget (parent)
{
    createWidgets();
    createLayouts();
}

//=============================================================================
// Console::createWidgets
//=============================================================================

void Console::createWidgets()
{
    m_messagesWindow = new MessagesWindow();

    m_buttonsWidget  = new QWidget        (this);
    m_consoleWidget  = new ConsoleWidget  (this);
    m_copyButton     = new QPushButton    (QChar (fa::copy),   this);
    m_clearButton    = new QPushButton    (QChar (fa::trash),  this);
    m_expandButton   = new QPushButton    (QChar (fa::expand), this);

    m_copyButton->setFont   (AWESOME()->font (DPI_SCALE (12)));
    m_clearButton->setFont  (AWESOME()->font (DPI_SCALE (12)));
    m_expandButton->setFont (AWESOME()->font (DPI_SCALE (12)));

    connect (m_copyButton,   SIGNAL (clicked()), m_consoleWidget,  SLOT (copy()));
    connect (m_clearButton,  SIGNAL (clicked()), m_consoleWidget,  SLOT (clear()));
    connect (m_expandButton, SIGNAL (clicked()), m_messagesWindow, SLOT (show()));
}

//=============================================================================
// Console::createLayouts
//=============================================================================

void Console::createLayouts()
{
    QSpacerItem* spacer = new QSpacerItem (0, 0,
                                           QSizePolicy::MinimumExpanding,
                                           QSizePolicy::MinimumExpanding);
    /* Configure buttons layout */
    m_buttonsLayout = new QHBoxLayout   (m_buttonsWidget);
    m_buttonsLayout->setSpacing         (DPI_SCALE (5));
    m_buttonsLayout->setContentsMargins (NULL_MARGINS());
    m_buttonsLayout->addWidget          (m_copyButton);
    m_buttonsLayout->addWidget          (m_clearButton);
    m_buttonsLayout->addSpacerItem      (spacer);
    m_buttonsLayout->addWidget          (m_expandButton);

    /* Configure main layout */
    m_mainLayout = new QVBoxLayout      (this);
    m_mainLayout->setSpacing            (DPI_SCALE (10));
    m_mainLayout->setContentsMargins    (MAIN_MARGINS());
    m_mainLayout->addWidget             (m_buttonsWidget);
    m_mainLayout->addWidget             (m_consoleWidget);
    m_mainLayout->setStretch            (0, 0);
    m_mainLayout->setStretch            (1, 1);

    /* Set button sizes */
    m_copyButton->setFixedSize          (DPI_SCALE (48), DPI_SCALE (24));
    m_clearButton->setFixedSize         (DPI_SCALE (48), DPI_SCALE (24));
    m_expandButton->setFixedSize        (DPI_SCALE (48), DPI_SCALE (24));
}
