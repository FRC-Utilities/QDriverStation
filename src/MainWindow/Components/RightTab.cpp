/*
 * Copyright (c) 2015-2016 WinT 3794 <http://wint3794.org>
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

#include "RightTab.h"

#include "About.h"
#include "Messages.h"
#include "CAN_Metrics.h"

#include "../Hacks/CustomTabStyle.h"

//=============================================================================
// RightTab::RightTab
//=============================================================================

RightTab::RightTab (QWidget* parent) : QTabWidget (parent) {
    m_about    = new About       (this);
    m_messages = new Messages    (this);
    m_metrics  = new CAN_Metrics (this);

    setTabPosition              (East);
    addTab                      (m_messages, QIcon(), QChar (fa::envelope));
    addTab                      (m_metrics,  QIcon(), QChar (fa::random));
    addTab                      (m_about,    QIcon(), QChar (fa::infocircle));

    tabBar()->setStyle          (new CustomTabStyle);
    tabBar()->setFont           (AWESOME()->font (DPI_SCALE (14)));

    DS::Log (DS::kInfoLevel, "MainWindow: Right tab created");
}

//=============================================================================
// RightTab::~RightTab
//=============================================================================

RightTab::~RightTab() {
    DS::Log (DS::kInfoLevel, "MainWindow: RightTab widget destroyed");
}
