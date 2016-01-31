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

#include <QUrl>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QApplication>
#include <QDesktopServices>

//=============================================================================
// Application includes
//=============================================================================

#include "About.h"
#include "Utilities/Global.h"

//=============================================================================
// About::About
//=============================================================================

About::About (QWidget* parent) : QWidget (parent)
{
    m_buttonContainer   = new QWidget     (this);
    m_appIcon           = new QLabel      (this);
    m_site              = new QPushButton (tr ("Visit Website"), this);
    m_reportBug         = new QPushButton (tr ("Report bug"), this);
    m_version           = new QLabel      (tr ("Version %1")
                                           .arg (QApplication::applicationVersion()),
                                           this);

    QSpacerItem* spacer = new QSpacerItem (0, 0,
                                           QSizePolicy::MinimumExpanding,
                                           QSizePolicy::MinimumExpanding);

    m_mainLayout = new QVBoxLayout   (this);
    m_mainLayout->setSpacing         (DPI_SCALE (5));
    m_mainLayout->addSpacerItem      (spacer);
    m_mainLayout->addWidget          (m_appIcon);
    m_mainLayout->addWidget          (m_version);
    m_mainLayout->addSpacerItem      (spacer);
    m_mainLayout->addWidget          (m_buttonContainer);
    m_mainLayout->addSpacerItem      (spacer);

    m_buttonLayout = new QHBoxLayout (m_buttonContainer);
    m_buttonLayout->setSpacing       (DPI_SCALE (5));
    m_buttonLayout->addSpacerItem    (spacer);
    m_buttonLayout->addWidget        (m_site);
    m_buttonLayout->addWidget        (m_reportBug);
    m_buttonLayout->addSpacerItem    (spacer);

    m_appIcon->setAlignment          (Qt::AlignCenter);
    m_version->setAlignment          (Qt::AlignCenter);
    m_appIcon->setPixmap             (QPixmap (":/headers/QDriverStation.png")
                                      .scaledToHeight (DPI_SCALE (72),
                                              Qt::SmoothTransformation));

    connect (m_site,      SIGNAL (clicked()), this, SLOT (openWebsite()));
    connect (m_reportBug, SIGNAL (clicked()), this, SLOT (reportBug()));
}

//=============================================================================
// About::reportBug
//=============================================================================

void About::reportBug()
{
    QDesktopServices::openUrl (
        QUrl ("https://github.com/wint-3794/qdriverstation/issues"));
}

//=============================================================================
// About::openWebsite
//=============================================================================

void About::openWebsite()
{
    QDesktopServices::openUrl (QUrl ("http://qdriverstation.sf.net"));
}
