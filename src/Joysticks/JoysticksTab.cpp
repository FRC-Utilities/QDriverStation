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

#include <QPointer>
#include <QSpinBox>
#include <QPushButton>
#include <QProgressBar>

#include <QDebug>

#include "JoysticksTab.h"
#include "VirtualJoystick.h"

//=============================================================================
// JoysticksTab::JoysticksTab
//=============================================================================

JoysticksTab::JoysticksTab (QWidget* parent) : QWidget (parent) {
    ui.setupUi (this);

    /* Hide the indicator containers */
    ui.Axes->setVisible (false);
    ui.Hats->setVisible (false);
    ui.Buttons->setVisible (false);

    /* Send SDL and virtual joystick data to this widget */
    m_keyboardDrive = new VirtualJoystick;
    connect (m_keyboardDrive, SIGNAL (HatEvent       (GM_Hat)),
             this,            SLOT   (OnHatEvent     (GM_Hat)));
    connect (m_keyboardDrive, SIGNAL (AxisEvent      (GM_Axis)),
             this,            SLOT   (OnAxisEvent    (GM_Axis)));
    connect (m_keyboardDrive, SIGNAL (ButtonEvent    (GM_Button)),
             this,            SLOT   (OnButtonEvent  (GM_Button)));
    connect (m_keyboardDrive, SIGNAL (CountChanged   (QStringList)),
             this,            SLOT   (OnCountChanged (QStringList)));

    /* Re-generate indicators when user selects another joystick */
    connect (ui.JoystickList, SIGNAL (currentRowChanged  (int)),
             this,            SLOT   (GenerateIndicators (int)));
}

//=============================================================================
// JoysticksTab::~JoysticksTab
//=============================================================================

JoysticksTab::~JoysticksTab() {
    delete m_keyboardDrive;
}

//=============================================================================
// JoysticksTab::ReadSettings
//=============================================================================

void JoysticksTab::ReadSettings() {
    m_keyboardDrive->ReadSettings();
}

//=============================================================================
// JoysticksTab::ShowKeyboardWindow
//=============================================================================

void JoysticksTab::ShowKeyboardWindow() {
    m_keyboardDrive->show();
}

//=============================================================================
// JoysticksTab::GenerateIndicators
//=============================================================================

void JoysticksTab::GenerateIndicators (int row) {
    /* Remove all joystick indicators in the widget */
    foreach (QSpinBox *     b, findChildren<QSpinBox*>())     delete b;
    foreach (QPushButton *  c, findChildren<QPushButton*>())  delete c;
    foreach (QProgressBar * p, findChildren<QProgressBar*>()) delete p;

    /* Clear joystick data */
    m_axes.clear();
    m_hats.clear();
    m_buttons.clear();

    /* Avoid crashing the application when there are no joysticks */
    if (row < 0)
        return;

    /* Get joystick information */
    int hatCount = m_keyboardDrive->GetNumHats (row);
    int axisCount = m_keyboardDrive->GetNumAxes (row);
    int buttonCount = m_keyboardDrive->GetNumButtons (row);

    /* Make the indicator containers visible */
    ui.Hats->setVisible (hatCount > 0);
    ui.Axes->setVisible (axisCount > 0);
    ui.Buttons->setVisible (buttonCount > 0);

    /* Create a progress bar for each axis */
    for (int i = 0; i < axisCount; ++i) {
        QPointer<QProgressBar> bar = new QProgressBar (this);

        bar->setMaximumHeight (19);
        bar->setMinimumHeight (19);

        bar->setValue (0);
        bar->setMaximum (100);
        bar->setMinimum (-100);
        bar->setFormat  (tr ("Axis %1").arg (i));

        m_axes.append (bar);
        ui.AxesWidget->layout()->addWidget (bar);
    }

    /* Create a button for each joystick button */
    for (int i = 0; i < buttonCount; ++i) {
        QPointer<QPushButton> button = new QPushButton (this);

        button->setEnabled (false);
        button->setCheckable (true);
        button->setMaximumSize (18, 12);
        button->setMinimumSize (18, 12);
        button->setToolTip     (tr ("Button %1").arg (i));

        /* Distribute the button items in a nice layout */
        int row = (i <= 7) ? i : i - 8;
        int column = (i <= 7) ? 0 : (i / 8);

        m_buttons.append (button);
        ui.ButtonLayout->addWidget (button, row, column);
    }

    /* Create a spinbox for each joystick hat */
    for (int i = 0; i < hatCount; ++i) {
        QPointer<QSpinBox> box = new QSpinBox (this);

        box->setRange (0, 360);
        box->setEnabled (false);

        m_hats.append (box);
        ui.HatsWidget->layout()->addWidget (box);
    }
}

//=============================================================================
// JoysticksTab::OnCountChanged
//=============================================================================

void JoysticksTab::OnCountChanged (QStringList list) {
    /* A joystick was removed */
    if (list.count() < ui.JoystickList->count())
        emit JoystickRemoved();

    /* Clear the joystick list (we will re-generate it later) */
    ui.JoystickList->clear();

    /* Put the current joysticks in the joystick list */
    if (list.count() > 0) {
        for (int i = 1; i <= list.count(); ++i)
            ui.JoystickList->addItem (QString ("%1: ").arg (i) + list.at (i - 1));

        /* Select the first item in the joystick selector */
        ui.JoystickList->setCurrentRow (0);
    }

    /* Notify other objects that JS count has changed */
    emit StatusChanged (list.count() > 0);
}

//=============================================================================
// JoysticksTab::OnHatEvent
//=============================================================================

void JoysticksTab::OnHatEvent (const GM_Hat& hat) {
    if (ui.JoystickList->currentRow() != hat.joystick.id)
        return;

    if (hat.id < m_hats.count())
        m_hats.at (hat.id)->setValue ((hat.angle == 0) ? 360 : hat.angle);
}

//=============================================================================
// JoysticksTab::OnAxisEvent
//=============================================================================

void JoysticksTab::OnAxisEvent (const GM_Axis& axis) {
    if (ui.JoystickList->currentRow() != axis.joystick.id)
        return;

    if (axis.id < m_axes.count())
        m_axes.at (axis.id)->setValue (axis.value * 100);
}

//=============================================================================
// JoysticksTab::OnButtonEvent
//=============================================================================

void JoysticksTab::OnButtonEvent (const GM_Button& button) {
    if (ui.JoystickList->currentRow() != button.joystick.id)
        return;

    if (button.id < m_buttons.count())
        m_buttons.at (button.id)->setChecked (button.pressed);
}
