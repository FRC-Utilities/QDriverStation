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

#include "LeftTab.h"
#include "Operator.h"
#include "Joysticks.h"
#include "Preferences.h"
#include "Diagnostics.h"

#include "../Hacks/CustomTabStyle.h"

//=============================================================================
// LeftTab::LeftTab
//=============================================================================

LeftTab::LeftTab (QWidget* parent) : QTabWidget (parent)
{
    m_operator    = new Operator    (this);
    m_joysticks   = new Joysticks   (this);
    m_preferences = new Preferences (this);
    m_diagnostics = new Diagnostics (this);

    setTabPosition     (West);
    addTab             (m_operator,    QIcon(), QChar (fa::dashboard));
    addTab             (m_diagnostics, QIcon(), QChar (fa::stethoscope));
    addTab             (m_preferences, QIcon(), QChar (fa::wrench));
    addTab             (m_joysticks,   QIcon(), QChar (fa::usb));

    tabBar()->setStyle (new CustomTabStyle);
    tabBar()->setFont  (AWESOME()->font (DPI_SCALE (16)));

    connect (m_operator, SIGNAL (showDocked()),
             this,       SIGNAL (showDocked()));
    connect (m_operator, SIGNAL (showNormally()),
             this,       SIGNAL (showNormally()));
    connect (m_operator, SIGNAL (requestErrorAnimation()),
             this,       SIGNAL (requestErrorAnimation()));
}
