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

#include <QLineEdit>
#include <QClipboard>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QApplication>
#include <QPlainTextEdit>

//=============================================================================
// Application includes
//=============================================================================

#include "Console.h"
#include "Utilities/Global.h"
#include "Utilities/Languages.h"

//=============================================================================
// Console::Console
//=============================================================================

Console::Console (QWidget* parent) : QWidget (parent)
{
    createWidgets();
    createLayouts();
    configureStyles();

    connect (m_sendCommand, SIGNAL (clicked()),   this, SLOT (sendCommand()));
    connect (m_copyButton,  SIGNAL (clicked()),   this, SLOT (copy()));
    connect (m_clearButton, SIGNAL (clicked()),   this, SLOT (clear()));
    connect (m_commands,    SIGNAL (returnPressed()),
             this,            SLOT (sendCommand()));
    connect (DS(),          SIGNAL (newMessage (QString)),
             this,            SLOT (log (QString)));
    connect (DS(),          SIGNAL (protocolChanged()),
             this,            SLOT (onProtocolChanged()));

    onProtocolChanged();
}

//=============================================================================
// Console::createWidgets
//=============================================================================

void Console::createWidgets()
{
    m_buttonsWidget  = new QWidget        (this);
    m_console        = new QPlainTextEdit (this);
    m_commands       = new QLineEdit      (this);
    m_commandsWidget = new QWidget        (this);
    m_sendCommand    = new QPushButton    (tr    ("Send"),    this);
    m_copyButton     = new QPushButton    (QChar (fa::copy),  this);
    m_clearButton    = new QPushButton    (QChar (fa::trash), this);
}

//=============================================================================
// Console::createLayouts
//=============================================================================

void Console::createLayouts()
{
    /* Configure buttons layout */
    m_buttonsLayout = new QHBoxLayout   (m_buttonsWidget);
    m_buttonsLayout->addWidget          (m_copyButton);
    m_buttonsLayout->addWidget          (m_clearButton);

    /* Configure the commands layout */
    m_commandsLayout = new QHBoxLayout (m_commandsWidget);
    m_commandsLayout->addWidget        (m_commands);
    m_commandsLayout->addWidget        (m_sendCommand);
    m_commandsLayout->setStretch       (0, 1);
    m_commandsLayout->setStretch       (1, 0);

    /* Configure main layout */
    m_mainLayout = new QVBoxLayout      (this);
    m_mainLayout->addWidget             (m_buttonsWidget);
    m_mainLayout->addWidget             (m_console);
    m_mainLayout->addWidget             (m_commandsWidget);

    /* Change spacing & resize to fit */
    m_mainLayout->setSpacing            (DPI_SCALE (10));
    m_buttonsLayout->setSpacing         (DPI_SCALE (5));
    m_buttonsLayout->setSizeConstraint  (QLayout::SetFixedSize);

    /* Set margins */
    m_mainLayout->setContentsMargins     (MAIN_MARGINS());
    m_buttonsLayout->setContentsMargins  (NULL_MARGINS());
    m_commandsLayout->setContentsMargins (NULL_MARGINS());

    /* Configure console output widget */
    m_console->setReadOnly              (true);
    m_console->setMaximumHeight         (DPI_SCALE (140));
    m_console->setMinimumWidth          (DPI_SCALE (320));;

    /* Set button sizes */
    m_copyButton->setFixedWidth         (DPI_SCALE (72));
    m_clearButton->setFixedWidth        (DPI_SCALE (72));
    m_copyButton->setFixedHeight        (DPI_SCALE (24));
    m_clearButton->setFixedHeight       (DPI_SCALE (24));

    /* Configure the commands widget */
    m_commands->setFixedHeight          (DPI_SCALE (24));
    m_sendCommand->setFixedSize         (DPI_SCALE (72), DPI_SCALE (24));
    m_commands->setPlaceholderText      (tr ("Write a command") + "...");
}

//=============================================================================
// Console::configureStyles
//=============================================================================

void Console::configureStyles()
{
    /* Apply monospace font to the console */
    m_console->setFont        (Languages::monoFont());

    /* Change the fonts of the buttons */
    m_copyButton->setFont     (AWESOME()->font (DPI_SCALE (12)));
    m_clearButton->setFont    (AWESOME()->font (DPI_SCALE (12)));

    /* Set tooltips */
    m_console->setToolTip     (tr ("Displays robot messages and logs"));
    m_copyButton->setToolTip  (tr ("Copy the console log"));
    m_clearButton->setToolTip (tr ("Clear the console window"));
}

//=============================================================================
// Console::copy
//=============================================================================

void Console::copy()
{
    QApplication::clipboard()->setText (m_console->toPlainText());
    log ("<font color=\"#AAA\"><p>"
         + tr ("INFO: NetConsole output copied to clipboard")
         + "</p></font>");
}

//=============================================================================
// Console::clear
//=============================================================================

void Console::clear()
{
    m_console->clear();
}

//=============================================================================
// Console::log
//=============================================================================

void Console::log (QString message)
{
    m_console->appendHtml (message);
    m_console->ensureCursorVisible();
}

//=============================================================================
// Console::sendCommand
//=============================================================================

void Console::sendCommand()
{
    QString format = QString ("<font color=#0F0>DS->RIO:</font>&nbsp;"
                              "<font color=#0A0>%1</font>");

    if (!m_commands->text().isEmpty())
        {
            log (format.arg (m_commands->text()));
            DS()->sendCommand (m_commands->text());
            m_commands->clear();
        }
}

//=============================================================================
// Console::onProtocolChanged
//=============================================================================

void Console::onProtocolChanged()
{
    m_commandsWidget->setEnabled (DS()->acceptsConsoleCommands());
    m_commandsWidget->setVisible (DS()->acceptsConsoleCommands());
}
