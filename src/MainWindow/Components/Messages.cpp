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

#include <QClipboard>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QApplication>
#include <QPlainTextEdit>

//=============================================================================
// Application includes
//=============================================================================

#include "Messages.h"
#include "Global/Global.h"
#include "Global/Languages.h"

//=============================================================================
// Console::Console
//=============================================================================

Messages::Messages (QWidget* parent) : QWidget (parent)
{
    createWidgets();
    createLayouts();

    DS_LogMessage (kInfoLevel, "MainWindow: Messages widget created");
}

//=============================================================================
// Messages::~Messages
//=============================================================================

Messages::~Messages()
{
    DS_LogMessage (kInfoLevel, "MainWindow: Messages widget destroyed");
}

//=============================================================================
// Console::createWidgets
//=============================================================================

void Messages::createWidgets()
{
    m_buttonsWidget  = new QWidget        (this);
    m_console        = new QPlainTextEdit (this);
    m_copyButton     = new QPushButton    (QChar (fa::copy),   this);
    m_clearButton    = new QPushButton    (QChar (fa::trash),  this);

    m_console->setReadOnly  (true);
    m_console->setFont      (Languages::monoFont());
    m_copyButton->setFont   (AWESOME()->font (DPI_SCALE (12)));
    m_clearButton->setFont  (AWESOME()->font (DPI_SCALE (12)));

    connect (m_copyButton,  &QPushButton::clicked, this, &Messages::copy);
    connect (m_clearButton, &QPushButton::clicked, this, &Messages::clear);
    connect (DS(),          &DriverStation::newMessage,
             this,          &Messages::registerMessage);
}

//=============================================================================
// Console::createLayouts
//=============================================================================

void Messages::createLayouts()
{
    /* Configure buttons layout */
    m_buttonsLayout = new QHBoxLayout   (m_buttonsWidget);
    m_buttonsLayout->setSpacing         (DPI_SCALE (5));
    m_buttonsLayout->setContentsMargins (NULL_MARGINS());
    m_buttonsLayout->addWidget          (m_copyButton);
    m_buttonsLayout->addWidget          (m_clearButton);
    m_buttonsLayout->addSpacerItem      (SPACER());

    /* Configure main layout */
    m_mainLayout = new QVBoxLayout      (this);
    m_mainLayout->setSpacing            (DPI_SCALE (10));
    m_mainLayout->setContentsMargins    (MAIN_MARGINS());
    m_mainLayout->addWidget             (m_buttonsWidget);
    m_mainLayout->addWidget             (m_console);
    m_mainLayout->setStretch            (0, 0);
    m_mainLayout->setStretch            (1, 1);

    /* Set widget sizes */
    m_console->setMinimumSize           (DPI_SCALE (360), DPI_SCALE (96));
    m_copyButton->setFixedSize          (DPI_SCALE  (74), DPI_SCALE (24));
    m_clearButton->setFixedSize         (DPI_SCALE  (74), DPI_SCALE (24));
}

//=============================================================================
// Console::copy
//=============================================================================

void Messages::copy()
{
    qApp->clipboard()->setText (m_console->toPlainText());
    registerMessage (QString   ("<font color=\"#aaa\"><p>")
                     + tr      ("INFO: Console output copied to clipboard")
                     + QString ("</p></font>"));
}

//=============================================================================
// Console::clear
//=============================================================================

void Messages::clear()
{
    m_console->clear();
}

//=============================================================================
// Console::registerMessage
//=============================================================================

void Messages::registerMessage (QString text)
{
    m_console->appendHtml (text);
    m_console->ensureCursorVisible();
}
