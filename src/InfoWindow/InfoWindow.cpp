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
#include <QComboBox>
#include <QTabWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QApplication>

//=============================================================================
// Application includes
//=============================================================================

#include "InfoWindow.h"
#include "Global/Global.h"
#include "Widgets/Drive.h"
#include "Widgets/Camera.h"
#include "Widgets/Console.h"
#include "Widgets/Checklist.h"

//=============================================================================
// MessagesWindow::MessagesWindow
//=============================================================================

InfoWindow::InfoWindow()
{
    /* Create widgets */
    m_drive            = new Drive       (this);
    m_camera           = new Camera      (this);
    m_console          = new Console     (this);
    m_checkList        = new Checklist   (this);
    m_tabWidget        = new QTabWidget  (this);
    m_rightWidget      = new QWidget     (this);
    m_cameraOptions    = new QComboBox   (this);
    m_camControlWidget = new QWidget     (this);

    /* Create the layouts */
    m_layout           = new QHBoxLayout (this);
    m_camLayout        = new QHBoxLayout (m_camControlWidget);
    m_rightLayout      = new QVBoxLayout (m_rightWidget);

    /* Configure right widget (tabs & cam options) */
    m_rightLayout->setSpacing         (DPI_SCALE (5));
    m_rightLayout->setContentsMargins (NULL_MARGINS());
    m_rightLayout->addWidget          (m_tabWidget);
    m_rightLayout->addWidget          (m_camControlWidget);
    m_rightLayout->setStretch         (0, 1);
    m_rightLayout->setStretch         (1, 0);

    /* Create the spacer between the camera options combo and buttons */
    QSpacerItem* spacer = new QSpacerItem (0,
                                           0,
                                           QSizePolicy::MinimumExpanding,
                                           QSizePolicy::MinimumExpanding);

    /* Configure the camera control options */
    m_cameraOptions->addItems         (m_camera->getOptions());
    m_camLayout->setContentsMargins   (NULL_MARGINS());
    m_camLayout->addWidget            (m_cameraOptions);
    m_camLayout->addSpacerItem        (spacer);

    /* Configure the tabs */
    m_tabWidget->addTab (m_drive,     tr ("Drive"));
    m_tabWidget->addTab (m_console,   tr ("Console"));
    m_tabWidget->addTab (m_checkList, tr ("Checklist"));

    /* Use custom margins to look good when displayed with MW on bottom */
    QMargins margins = QMargins (DPI_SCALE (10),
                                 DPI_SCALE (10),
                                 DPI_SCALE (10),
                                 DPI_SCALE  (0));

    /* Configure the layout */
    m_layout->addWidget (m_camera);
    m_layout->addWidget (m_rightWidget);
    m_layout->setSpacing (DPI_SCALE (10));
    m_layout->setContentsMargins (margins);
    m_console->layout()->setContentsMargins (MAIN_MARGINS());

    /* Connect signals/slots */
    connect (m_cameraOptions, SIGNAL (currentIndexChanged (int)),
             m_camera,          SLOT (setCameraOption     (int)));
}

//=============================================================================
// MessagesWindow::console
//=============================================================================

Console* InfoWindow::console() const
{
    return m_console;
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
