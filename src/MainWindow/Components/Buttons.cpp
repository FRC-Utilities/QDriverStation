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

#include <QPushButton>
#include <QVBoxLayout>
#include <QApplication>

//=============================================================================
// Application includes
//=============================================================================

#include "Buttons.h"
#include "Utilities/Global.h"

//=============================================================================
// Buttons::Buttons
//=============================================================================

Buttons::Buttons (QWidget* parent) : QWidget (parent)
{
    /* Create the buttons */
    m_close             = new QPushButton (QChar (fa::close),     this);
    m_settings          = new QPushButton (QChar (fa::wrench),    this);
    m_virtualJ          = new QPushButton (QChar (fa::keyboardo), this);
    QSpacerItem* spacer = new QSpacerItem (0, 0,
                                           QSizePolicy::MinimumExpanding,
                                           QSizePolicy::MinimumExpanding);

    /* Set tooltips */
    m_close->setToolTip          (tr ("Quit"));
    m_settings->setToolTip       (tr ("Preferences"));
    m_virtualJ->setToolTip       (tr ("Virtual/Keyboard Joystick"));

    /* Configure the layout */
    m_layout = new QVBoxLayout   (this);
    m_layout->setSpacing         (DPI_SCALE (3));
    m_layout->addWidget          (m_close);
    m_layout->addWidget          (m_settings);
    m_layout->addWidget          (m_virtualJ);
    m_layout->addSpacerItem      (spacer);

    /* Change the font of the buttons to display font awesome */;
    m_close->setFont             (AWESOME()->font (DPI_SCALE (16)));
    m_settings->setFont          (AWESOME()->font (DPI_SCALE (16)));
    m_virtualJ->setFont          (AWESOME()->font (DPI_SCALE (16)));

    /* Change size of buttons */
    m_close->setFixedSize        (DPI_SCALE (36), DPI_SCALE (36));
    m_settings->setFixedSize     (DPI_SCALE (36), DPI_SCALE (36));
    m_virtualJ->setFixedSize     (DPI_SCALE (36), DPI_SCALE (36));

    /* Change layout margins */
    m_layout->setContentsMargins (DPI_SCALE (5), 0, 0, 0);

    /* Make the close button red */
    QPalette closePalette;
    QBrush brush2                (QColor (121, 0, 0, 255));
    brush2.setStyle              (Qt::SolidPattern);
    closePalette.setBrush        (QPalette::Active, QPalette::Button, brush2);
    QBrush brush3                (QColor (255, 255, 255, 255));
    brush3.setStyle              (Qt::SolidPattern);
    closePalette.setBrush        (QPalette::Active,   QPalette::ButtonText, brush3);
    closePalette.setBrush        (QPalette::Inactive, QPalette::Button,     brush2);
    closePalette.setBrush        (QPalette::Inactive, QPalette::ButtonText, brush3);
    closePalette.setBrush        (QPalette::Disabled, QPalette::Button,     brush2);
    QBrush brush4                (QColor (148, 148, 148, 255));
    brush4.setStyle              (Qt::SolidPattern);
    closePalette.setBrush        (QPalette::Disabled, QPalette::ButtonText, brush4);
    m_close->setPalette          (closePalette);

    /* Configure button actions */
    connect (m_close,    SIGNAL (clicked()), qApp, SLOT (closeAllWindows()));
    connect (m_settings, SIGNAL (clicked()), this, SLOT (showSettingsWindow()));
    connect (m_virtualJ, SIGNAL (clicked()), this, SLOT (showVirtualJoysticks()));
}

//=============================================================================
// Buttons::showSettingsWindow
//=============================================================================

void Buttons::showSettingsWindow()
{
    SHOW_SETTINGS_WINDOW();
}

//=============================================================================
// Buttons::showVirtualJoysticks
//=============================================================================

void Buttons::showVirtualJoysticks()
{
    SHOW_VIRTUAL_JOYSTICKS();
}
