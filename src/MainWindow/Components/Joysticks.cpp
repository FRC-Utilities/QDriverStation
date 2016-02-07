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

#include <QPointer>
#include <QGroupBox>
#include <QShowEvent>
#include <QHideEvent>
#include <QListWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QSpacerItem>

//=============================================================================
// Application includes
//=============================================================================

#include "Joysticks.h"
#include "Global/Global.h"
#include "Joysticks/JoystickManager.h"

//=============================================================================
// CSS
//=============================================================================

QString BUTTON_CSS = "QPushButton {"
                     "background-color: rgb(38, 38, 38);"
                     "border: 0px;"
                     "padding: 4px;"
                     "} QPushButton:checked {"
                     "background-color: rgb(33, 255, 43); }";

//=============================================================================
// Joysticks::Joysticks
//=============================================================================

Joysticks::Joysticks (QWidget* parent) : QWidget (parent)
{
    /* Create Widgets */
    m_joystickNames    = new QListWidget (this);
    m_JSIndicators     = new QGroupBox   (tr ("USB"),     this);
    m_POVIndicators    = new QGroupBox   (tr ("POVs"),    this);
    m_axisIndicators   = new QGroupBox   (tr ("Axes"),    this);
    m_buttonIndicators = new QGroupBox   (tr ("Buttons"), this);

    /* Create container widgets */
    m_axesContainer    = new QWidget     (this);
    m_povsContainer    = new QWidget     (this);
    m_buttonsContainer = new QWidget     (this);

    /* Create the container layouts */
    m_axesBox          = new QVBoxLayout (m_axesContainer);
    m_povsBox          = new QVBoxLayout (m_povsContainer);
    m_buttonsBox       = new QGridLayout (m_buttonsContainer);

    /* Set indicator layouts */
    m_joystickBox      = new QVBoxLayout (m_JSIndicators);
    m_povGroupBox      = new QVBoxLayout (m_POVIndicators);
    m_axisGroupBox     = new QVBoxLayout (m_axisIndicators);
    m_buttonGroupBox   = new QVBoxLayout (m_buttonIndicators);

    /* Set layout spacings */
    int s = DPI_SCALE (4);
    m_joystickBox->setSpacing            (s);
    m_povGroupBox->setSpacing            (s);
    m_axisGroupBox->setSpacing           (s);
    m_buttonGroupBox->setSpacing         (s);
    m_povsBox->setSpacing                (s);
    m_axesBox->setSpacing                (s);
    m_buttonsBox->setSpacing             (s);
    m_povGroupBox->setSpacing            (s);
    m_axisGroupBox->setSpacing           (s);
    m_buttonGroupBox->setSpacing         (s);

    /* Set the margins */
    int m = DPI_SCALE (2);
    m_joystickBox->setContentsMargins    (m, m, m, m);
    m_povGroupBox->setContentsMargins    (m, m, m, m);
    m_axisGroupBox->setContentsMargins   (m, m, m, m);
    m_buttonGroupBox->setContentsMargins (m, m, m, m);
    m_povsBox->setContentsMargins        (m, m, m, m);
    m_axesBox->setContentsMargins        (m, m, m, m);
    m_buttonsBox->setContentsMargins     (m, m, m, m);

    /* Set indicators layout */
    m_axisGroupBox->addWidget            (m_axesContainer);
    m_povGroupBox->addWidget             (m_povsContainer);
    m_buttonGroupBox->addWidget          (m_buttonsContainer);
    m_axisGroupBox->addSpacerItem        (SPACER());
    m_povGroupBox->addSpacerItem         (SPACER());
    m_buttonGroupBox->addSpacerItem      (SPACER());

    /* Set main layout */
    m_mainLayout = new QHBoxLayout       (this);
    m_mainLayout->setSpacing             (DPI_SCALE (5));
    m_mainLayout->addWidget              (m_JSIndicators);
    m_mainLayout->addWidget              (m_axisIndicators);
    m_mainLayout->addWidget              (m_buttonIndicators);
    m_mainLayout->addWidget              (m_POVIndicators);
    m_mainLayout->addSpacerItem          (SPACER());
    m_joystickBox->addWidget             (m_joystickNames);

    /* Update UI config */
    m_POVIndicators->setVisible          (false);
    m_axisIndicators->setVisible         (false);
    m_buttonIndicators->setVisible       (false);
    m_buttonIndicators->setStyleSheet    (BUTTON_CSS);
    onCountChanged();

    /* Connect slots */
    connect (JOYSTICK_MANAGER(), &JoystickManager::countChanged,
             this,               &Joysticks::onCountChanged);
    connect (JOYSTICK_MANAGER(), &JoystickManager::POVEvent,
             this,               &Joysticks::onPOVEvent);
    connect (JOYSTICK_MANAGER(), &JoystickManager::axisEvent,
             this,               &Joysticks::onAxisEvent);
    connect (JOYSTICK_MANAGER(), &JoystickManager::buttonEvent,
             this,               &Joysticks::onButtonEvent);
    connect (m_joystickNames,    &QListWidget::currentRowChanged,
             this,               &Joysticks::setupIndicators);
}

//=============================================================================
// Joysticks::showEvent
//=============================================================================

void Joysticks::showEvent (QShowEvent* event)
{
    /* Load joystick indicators */
    onCountChanged();

    /* Actually show the widget */
    event->accept();
}

//=============================================================================
// Joysticks::hideEvent
//=============================================================================

void Joysticks::hideEvent (QHideEvent* event)
{
    /* Remove joystick indicators and resize to avoid ugly shit */
    setupIndicators (-1);
    resize (minimumSizeHint());

    /* Allow Qt to hide the widget */
    event->accept();
}

//=============================================================================
// Joysticks::onCountChanged
//=============================================================================

void Joysticks::onCountChanged()
{
    m_joystickNames->clear();
    m_joystickNames->addItems (JOYSTICK_MANAGER()->deviceNames());

    if (m_joystickNames->count() > 0)
        {
            setupIndicators (0);
            m_joystickNames->setCurrentRow (0);
        }

    else
        {
            m_POVIndicators->setVisible (false);
            m_axisIndicators->setVisible (false);
            m_buttonIndicators->setVisible (false);
        }
}

//=============================================================================
// Joysticks::setupIndicators
//=============================================================================

void Joysticks::setupIndicators (int row)
{
    /* Remove all joystick indicators in the widget */
    foreach (QSpinBox *     b, findChildren<QSpinBox*>())     delete b;
    foreach (QPushButton *  c, findChildren<QPushButton*>())  delete c;
    foreach (QProgressBar * p, findChildren<QProgressBar*>()) delete p;

    /* Clear joystick data */
    m_axes.clear();
    m_povs.clear();
    m_buttons.clear();

    /* Avoid crashing the application when there are no joysticks */
    if (row < 0)
        return;

    /* Get joystick information */
    int povCount    = JOYSTICK_MANAGER()->getInputDevice (row).numPOVs;
    int axisCount   = JOYSTICK_MANAGER()->getInputDevice (row).numAxes;
    int buttonCount = JOYSTICK_MANAGER()->getInputDevice (row).numButtons;

    /* Make the indicator containers visible */
    m_POVIndicators->setVisible (povCount > 0);
    m_axisIndicators->setVisible (axisCount > 0);
    m_buttonIndicators->setVisible (buttonCount > 0);

    /* Create a progress bar for each axis */
    for (int i = 0; i < axisCount; ++i)
        {
            QProgressBar* bar = new QProgressBar (this);

            bar->setFixedHeight (DPI_SCALE (19));

            bar->setValue (0);
            bar->setMaximum (100);
            bar->setMinimum (-100);
            bar->setFormat  (tr ("Axis %1").arg (i));

            m_axes.append (bar);
            m_axesBox->addWidget (bar);
        }

    /* Create a button for each joystick button */
    for (int i = 0; i < buttonCount; ++i)
        {
            QPushButton* button = new QPushButton (this);

            button->setEnabled (false);
            button->setCheckable (true);
            button->setFixedSize (DPI_SCALE (18), DPI_SCALE (12));
            button->setToolTip     (tr ("Button %1").arg (i));

            /* Distribute the button items in a nice layout */
            int row = (i <= 7) ? i : i - 8;
            int column = (i <= 7) ? 0 : (i / 8);

            m_buttons.append (button);
            m_buttonsBox->addWidget (button, row, column);
        }

    /* Create a spinbox for each joystick hat */
    for (int i = 0; i < povCount; ++i)
        {
            QSpinBox* box = new QSpinBox (this);

            box->setRange (0, 360);
            box->setEnabled (false);

            m_povs.append (box);
            m_povsBox->addWidget (box);
        }
}

//=============================================================================
// Joysticks::setTipsVisible
//=============================================================================

void Joysticks::setTipsVisible (bool visible)
{
    Q_UNUSED (visible);
}

//=============================================================================
// Joysticks::onPOVEvent
//=============================================================================

void Joysticks::onPOVEvent (QDS_POVEvent event)
{
    if (m_joystickNames->currentRow() != event.joystick.id || !isVisible())
        return;

    if (event.pov < m_povs.count())
        m_povs.at (event.pov)->setValue ((event.angle == 0) ? 360 : event.angle);
}

//=============================================================================
// Joysticks::onAxisEvent
//=============================================================================

void Joysticks::onAxisEvent (QDS_AxisEvent event)
{
    if (m_joystickNames->currentRow() != event.joystick.id || !isVisible())
        return;

    if (event.axis < m_axes.count())
        m_axes.at (event.axis)->setValue (event.value * 100);
}

//=============================================================================
// Joysticks::onButtonEvent
//=============================================================================

void Joysticks::onButtonEvent (QDS_ButtonEvent event)
{
    if (m_joystickNames->currentRow() != event.joystick.id || !isVisible())
        return;

    if (event.button < m_buttons.count())
        m_buttons.at (event.button)->setChecked (event.pressed);
}
