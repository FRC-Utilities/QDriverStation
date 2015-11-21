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
#include <QPushButton>
#include <QProgressBar>

#include "JoysticksTab.h"
#include "VirtualJoystick.h"

JoysticksTab::JoysticksTab (QWidget* parent) : QWidget (parent) {
    ui.setupUi (this);
    ui.Axes->setVisible (false);
    ui.Buttons->setVisible (false);

    m_keyboardDrive = new VirtualJoystick;

    connect (m_keyboardDrive, SIGNAL (axisEvent      (GM_Axis)),
             this,            SLOT   (onAxisEvent    (GM_Axis)));
    connect (m_keyboardDrive, SIGNAL (buttonEvent    (GM_Button)),
             this,            SLOT   (onButtonEvent  (GM_Button)));
    connect (m_keyboardDrive, SIGNAL (countChanged   (QStringList)),
             this,            SLOT   (onCountChanged (QStringList)));
    connect (ui.JoystickList, SIGNAL (currentRowChanged (int)),
             this,            SLOT   (onRowChanged      (int)));
}

JoysticksTab::~JoysticksTab() {
    delete m_keyboardDrive;
}

void JoysticksTab::readSettings() {
    m_keyboardDrive->readSettings();
}

void JoysticksTab::showKeyboardWindow() {
    m_keyboardDrive->show();
}

//------------------------------------------------------------------------------
// SEND KEYBOARD INPUT TO VIRTUAL JOSYTICK
//------------------------------------------------------------------------------

void JoysticksTab::registerKeyPress (QKeyEvent* event) {
    m_keyboardDrive->registerKeyPress (event);
}

void JoysticksTab::registerKeyRelease (QKeyEvent* event) {
    m_keyboardDrive->registerKeyRelease (event);
}

//------------------------------------------------------------------------------
// REACT TO UI EVENTS
//------------------------------------------------------------------------------

void JoysticksTab::onRowChanged (int row) {
    /* Remove all buttons and progress bars in the widget */
    foreach (QPushButton *  c, findChildren<QPushButton*>())  delete c;
    foreach (QProgressBar * p, findChildren<QProgressBar*>()) delete p;

    /* Clear joystick data */
    m_axes.clear();
    m_buttons.clear();
    ui.Axes->setVisible (false);
    ui.Buttons->setVisible (false);

    /* Avoid crashing the application when there are no joysticks */
    if (row < 0)  return;

    /* Get joystick information */
    int axisCount = m_keyboardDrive->getNumAxes (row);
    int buttonCount = m_keyboardDrive->getNumButtons (row);

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
}

//------------------------------------------------------------------------------
// REACT TO JOYSTICK EVENTS
//------------------------------------------------------------------------------

void JoysticksTab::onCountChanged (QStringList list) {
    if (list.count() < ui.JoystickList->count())
        emit joystickRemoved();

    ui.JoystickList->clear();

    if (list.count() > 0) {
        for (int i = 1; i <= list.count(); ++i)
            ui.JoystickList->addItem (QString ("%1: ").arg (i) + list.at (i - 1));

        ui.JoystickList->setCurrentRow (0);
    }

    emit statusChanged (list.count() > 0);
}

void JoysticksTab::onAxisEvent (const GM_Axis& axis) {
    if (ui.JoystickList->currentRow() != axis.joystick.id)
        return;

    if (axis.id < m_axes.count())
        m_axes.at (axis.id)->setValue (axis.value * 100);
}

void JoysticksTab::onButtonEvent (const GM_Button& button) {
    if (ui.JoystickList->currentRow() != button.joystick.id)
        return;

    if (button.id < m_buttons.count())
        m_buttons.at (button.id)->setChecked (button.pressed);
}
