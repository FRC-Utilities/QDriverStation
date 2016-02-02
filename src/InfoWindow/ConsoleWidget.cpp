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
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>
#include <QPlainTextEdit>

//=============================================================================
// Application includes
//=============================================================================

#include "ConsoleWidget.h"
#include "Global/Global.h"
#include "Global/Languages.h"

//=============================================================================
// ConsoleWidget::ConsoleWidget
//=============================================================================

ConsoleWidget::ConsoleWidget (QWidget* parent) : QWidget (parent)
{
    createWidgets();
    createLayouts();
    connectSlots();
    onProtocolChanged();
}

//=============================================================================
// ConsoleWidget::~ConsoleWidget
//=============================================================================

ConsoleWidget::~ConsoleWidget()
{
    delete m_sendButton;
    delete m_consoleEdit;
    delete m_commandEdit;
    delete m_commandWidget;
}

//=============================================================================
// ConsoleWidget::copy
//=============================================================================

void ConsoleWidget::copy()
{
    QApplication::clipboard()->setText (m_consoleEdit->toPlainText());

    registerMessage ("<font color=\"#AAA\"><p>"
                     + tr ("INFO: NetConsole output copied to clipboard")
                     + "</p></font>");
}

//=============================================================================
// ConsoleWidget::clear
//=============================================================================

void ConsoleWidget::clear()
{
    m_consoleEdit->clear();
}

//=============================================================================
// ConsoleWidget::createWidgets
//=============================================================================

void ConsoleWidget::createWidgets()
{
    m_commandWidget = new QWidget        (this);
    m_commandEdit   = new QLineEdit      (this);
    m_consoleEdit   = new QPlainTextEdit (this);
    m_sendButton    = new QPushButton    (tr ("Send"), this);

    m_consoleEdit->setReadOnly           (true);
    m_sendButton->setFixedHeight         (DPI_SCALE (24));
    m_commandEdit->setFixedHeight        (DPI_SCALE (24));
    m_consoleEdit->setFont               (Languages::monoFont());
    m_commandEdit->setPlaceholderText    (tr ("Type a command") + "...");
    m_consoleEdit->setMinimumSize        (DPI_SCALE (288), DPI_SCALE (96));
}

//=============================================================================
// ConsoleWidget::createLayouts
//=============================================================================

void ConsoleWidget::createLayouts()
{
    m_commandLayout = new QHBoxLayout   (m_commandWidget);
    m_commandLayout->setContentsMargins (NULL_MARGINS());
    m_commandLayout->addWidget          (m_commandEdit);
    m_commandLayout->addWidget          (m_sendButton);
    m_commandLayout->setStretch         (0, 1);
    m_commandLayout->setStretch         (1, 0);

    m_mainLayout = new QVBoxLayout      (this);
    m_mainLayout->setContentsMargins    (NULL_MARGINS());
    m_mainLayout->addWidget             (m_consoleEdit);
    m_mainLayout->addWidget             (m_commandWidget);
    m_mainLayout->setStretch            (0, 1);
    m_mainLayout->setStretch            (1, 0);
}

//=============================================================================
// ConsoleWidget::connectSlots
//=============================================================================

void ConsoleWidget::connectSlots()
{
    connect (m_commandEdit, SIGNAL (returnPressed()),
             this,            SLOT (sendCommand()));
    connect (m_sendButton,  SIGNAL (clicked()),
             this,            SLOT (sendCommand()));
    connect (DS(),          SIGNAL (newMessage        (QString)),
             this,            SLOT (registerMessage   (QString)));
    connect (DS(),          SIGNAL (protocolChanged   (void)),
             this,            SLOT (onProtocolChanged (void)));
}

//=============================================================================
// ConsoleWidget::sendCommand
//=============================================================================

void ConsoleWidget::ConsoleWidget::sendCommand()
{
    QString format = QString ("<font color=#0F0>DS->RIO:</font>&nbsp;"
                              "<font color=#0A0>%1</font>");

    if (!m_commandEdit->text().isEmpty())
        {
            registerMessage (format.arg (m_commandEdit->text()));
            DS()->sendCommand (m_commandEdit->text());
            m_commandEdit->clear();
        }
}

//=============================================================================
// ConsoleWidget::onProtocolChanged
//=============================================================================

void ConsoleWidget::onProtocolChanged()
{
    m_commandWidget->setEnabled (DS()->acceptsConsoleCommands());
    m_commandWidget->setVisible (DS()->acceptsConsoleCommands());
}

//=============================================================================
// ConsoleWidget::registerMessage
//=============================================================================

void ConsoleWidget::registerMessage (QString text)
{
    m_consoleEdit->appendHtml (text);
    m_consoleEdit->ensureCursorVisible();

    emit messageRegistered (text);
}
