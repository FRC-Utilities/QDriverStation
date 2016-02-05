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

#include <QLabel>
#include <QGroupBox>
#include <QCheckBox>
#include <QTabWidget>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDoubleSpinBox>

//=============================================================================
// Application includes
//=============================================================================

#include "VJoystickWindow.h"
#include "Global/Global.h"
#include "Global/Settings.h"
#include "Joysticks/JoystickManager.h"
#include "Joysticks/VirtualJoystick.h"

//=============================================================================
// VJoystickWindow::VJoystickWindow
//=============================================================================

VJoystickWindow::VJoystickWindow()
{
    createWidgets();
    createLayouts();
    connectSlots();
    readSettings();

    setFixedSize (minimumSizeHint());
}

//=============================================================================
// VJoystickWindow::~VJoystickWindow
//=============================================================================

VJoystickWindow::~VJoystickWindow()
{
    foreach (QWidget* widget, findChildren<QWidget*>())
        widget->deleteLater();
}

//=============================================================================
// VJoystickWindow::createWidgets
//=============================================================================

void VJoystickWindow::createWidgets()
{
    /* Tabs and keys group box */
    m_generalTab            = new QWidget (this);
    m_tab                   = new QTabWidget (this);
    m_keysBox               = new QGroupBox (tr ("Keys"), this);

    /* Keys and meanings list */
    m_keys                  = new QListWidget (this);
    m_meanings              = new QListWidget (this);

    /* Axis range controller */
    m_axisRangeContainer    = new QWidget (this);
    m_axisRange             = new QDoubleSpinBox (this);
    m_axisRangeLabel        = new QLabel (tr ("Axis range"), this);

    /* Keyboard checkbox & close button */
    m_bottomWidget          = new QWidget (this);
    m_closeButton           = new QPushButton (tr ("Close"), this);
    m_useKeyboardAsJoystick = new QCheckBox (
        tr ("Use my keyboard as a joystick"), this);

    /* Create the 'General' tab */
    m_tab->addTab              (m_generalTab, QIcon(), tr ("General"));

    /* Configure the axis range widget */
    m_axisRange->setRange      (0, 1);
    m_axisRange->setSingleStep (0.01);

    /* Configure the list widgets */
    m_keys->setEnabled         (false);
    m_meanings->setEnabled     (false);

    /* Generate the content of the list widgets */
    QStringList keys =
    {
        "W, A, S, D",
        "Up, Down",
        "Page Up, Page down",
        "I, J, K, L",
        "1, 2, 3, 4, 5, 6, 7, 8, 9",
    };
    QStringList meanings =
    {
        tr ("Primary Thumb (axes 0 & 1)"),
        tr ("Trigger 1 (axis 2)"),
        tr ("Trigger 2 (axis 3)"),
        tr ("Secondary Thumb (axes 4 & 5)"),
        tr ("Joystick buttons")
    };

    /* 'Fill' the list widgets */
    m_keys->addItems           (keys);
    m_meanings->addItems       (meanings);

    /* Resize the list widgets */
    m_keys->setFixedHeight     (m_keys->minimumSizeHint().height() * 2);
    m_meanings->setFixedHeight (m_meanings->minimumSizeHint().height() * 2);
}

//=============================================================================
// VJoystickWindow::createLayouts
//=============================================================================

void VJoystickWindow::createLayouts()
{
    /* Keyboard keys layout */
    m_keysLayout = new QHBoxLayout        (m_keysBox);
    m_keysLayout->setContentsMargins      (MAIN_MARGINS());
    m_keysLayout->addWidget               (m_meanings);
    m_keysLayout->addWidget               (m_keys);

    /* Axis range layout */
    m_axisRangeLayout = new QVBoxLayout   (m_axisRangeContainer);
    m_axisRangeLayout->setContentsMargins (NULL_MARGINS());
    m_axisRangeLayout->setSpacing         (DPI_SCALE (5));
    m_axisRangeLayout->addWidget          (m_axisRangeLabel);
    m_axisRangeLayout->addWidget          (m_axisRange);

    /* 'General' tab layout */
    m_tabsLayout = new QVBoxLayout        (m_generalTab);
    m_tabsLayout->setSpacing              (DPI_SCALE (5));
    m_tabsLayout->setContentsMargins      (MAIN_MARGINS());
    m_tabsLayout->addWidget               (m_keysBox);
    m_tabsLayout->addWidget               (m_axisRangeContainer);
    m_tabsLayout->setStretch              (0, 1);
    m_tabsLayout->setStretch              (1, 0);

    /* Spacer between keyboard checkbox and close button */
    QSpacerItem* spacer = new QSpacerItem (DPI_SCALE (128), 0
                                           QSizePolicy::MinimumExpanding,
                                           QSizePolicy::MinimumExpanding);

    /* Keyboard checkbox & close button */
    m_bottomLayout = new QHBoxLayout      (m_bottomWidget);
    m_bottomLayout->setSpacing            (DPI_SCALE (5));
    m_bottomLayout->setContentsMargins    (MAIN_MARGINS());
    m_bottomLayout->addWidget             (m_useKeyboardAsJoystick);
    m_bottomLayout->addSpacerItem         (spacer);
    m_bottomLayout->addWidget             (m_closeButton);

    /* Put everything together */
    m_mainLayout = new QVBoxLayout        (this);
    m_mainLayout->setSpacing              (DPI_SCALE (5));
    m_mainLayout->setContentsMargins      (MAIN_MARGINS());
    m_mainLayout->addWidget               (m_tab);
    m_mainLayout->addWidget               (m_bottomWidget);
}

//=============================================================================
// VJoystickWindow::connectSlots
//=============================================================================

void VJoystickWindow::connectSlots()
{
    VirtualJoystick* vJoystick = JOYSTICK_MANAGER()->virtualJoystick();

    connect (m_closeButton,           SIGNAL (clicked()),
             this,                      SLOT (hide()));
    connect (m_useKeyboardAsJoystick, SIGNAL (toggled            (bool)),
             vJoystick,                 SLOT (setJoystickEnabled (bool)));
    connect (m_axisRange,             SIGNAL (valueChanged       (double)),
             vJoystick,                 SLOT (setAxisRange       (double)));
}

//=============================================================================
// VJoystickWindow::readSettings
//=============================================================================

void VJoystickWindow::readSettings()
{
    double range = Settings::get ("Axis Range", 0.8).toDouble();
    bool useVJoystick = Settings::get ("Virtual Joystick", false).toBool();

    m_axisRange->setValue (range);
    m_useKeyboardAsJoystick->setChecked (useVJoystick);

    JOYSTICK_MANAGER()->virtualJoystick()->setAxisRange (range);
    JOYSTICK_MANAGER()->virtualJoystick()->setJoystickEnabled (useVJoystick);
}
