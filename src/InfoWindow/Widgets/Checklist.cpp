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
#include <QPlainTextEdit>

//=============================================================================
// Application includes
//=============================================================================

#include "Checklist.h"
#include "Global/Global.h"
#include "Global/Settings.h"
#include "Global/Languages.h"

//=============================================================================
// Global variables
//=============================================================================

const QString _STYLE        = "QPlainTextEdit {"
                              "color: #222;"
                              "background: url(:/textures/notes.png) repeat-y; }";
const QString _DEFAULT_LIST = QObject::tr ("NOTES: \n"
                              "1. Secure battery and connection.\n"
                              "2. Ethernet cable from radio to robot controller.\n"
                              "3. Radio switch position and LEDs.\n"
                              "4. Remove safety pins.\n"
                              "\n"
                              "Modify this text directly to customize your list.");

//=============================================================================
// CheckList::CheckList
//=============================================================================

Checklist::Checklist (QWidget* parent) : QWidget (parent)
{
    /* Create widgets */
    m_layout = new QVBoxLayout (this);
    m_edit   = new QPlainTextEdit (parent);

    /* Configure layout */
    m_layout->addWidget (m_edit);
    m_layout->setContentsMargins (MAIN_MARGINS());

    /* Load checklist into text editor */
    QFont font = Languages::monoFont();
    font.setPixelSize     (DPI_SCALE (14));
    m_edit->setFont       (font);
    m_edit->setStyleSheet (_STYLE);
    m_edit->setPlainText  (Settings::get ("Checklist", _DEFAULT_LIST).toString());

    connect (m_edit, SIGNAL (textChanged()), this, SLOT (saveChecklist()));
}

//=============================================================================
// CheckList::saveChecklist
//=============================================================================

void Checklist::saveChecklist()
{
    Settings::set ("Checklist", m_edit->toPlainText());
}
