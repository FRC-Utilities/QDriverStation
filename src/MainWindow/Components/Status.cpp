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

//==================================================================================================
// System includes
//==================================================================================================

#include <QLabel>
#include <QTimer>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>

//==================================================================================================
// Application includes
//==================================================================================================

#include "Status.h"
#include "Global/Global.h"
#include "Global/Beeper.h"

//==================================================================================================
// CSS definitions
//==================================================================================================

/* Red when uncheked, green when checked */
const QString NORMAL_LED = "QPushButton {"
                           "background-color: rgb(255, 37, 43);"
                           "border: 0px; padding: 4px; }"
                           "QPushButton:checked {"
                           "background-color: rgb(33, 255, 43); }";

/* Red when uncheked, yellow when checked */
const QString CUSTM_LED = "QPushButton {"
                          "background-color: rgb(255, 37, 43);" ""
                          "border: 0px; padding: 4px; }"
                          "QPushButton:checked {"
                          "background-color: rgb(244, 255, 43); }";

//==================================================================================================
// Hacks for the voltage indicator
//==================================================================================================

#define _SPACER         "   "
#define _VOLTAGE_FORMAT "   99.99 V"

//==================================================================================================
// Status::Status
//==================================================================================================

Status::Status (QWidget* parent) : QWidget (parent) {
    createWidgets();
    createLayouts();
    configureStyles();
    connectSlots();

    DS::log (DS::kInfoLevel, "MainWindow: Status/Central widget created");
}

//==================================================================================================
// Status::~Status
//==================================================================================================

Status::~Status() {
    DS::log (DS::kInfoLevel, "MainWindow: Status/Central widget destroyed");
}

//==================================================================================================
// Status::doErrorAnimation
//==================================================================================================

void Status::doErrorAnimation() {
    for (int i = 0; i < 8; ++i)
        QTimer::singleShot (100 * i, Qt::PreciseTimer, this, SLOT (toggleStatusColors()));

    MORSE_BEEP ("..-", 550);
}

//==================================================================================================
// Status::createWidgets
//==================================================================================================

void Status::createWidgets() {
    /* Create the indicator labels */
    m_estop        = false;
    m_team         = new QLabel (NO_DATA, this);
    m_voltage      = new QLabel (NO_DATA, this);
    m_robotStatus  = new QLabel (tr ("No Robot Communication"), this);

    /* Create the indicator LEDs */
    m_comm         = new QPushButton ("", this);
    m_code         = new QPushButton ("", this);
    m_sticks       = new QPushButton ("", this);

    /* Create the caption labels */
    m_teamLabel    = new QLabel (tr ("Team #"), this);
    m_commLabel    = new QLabel (tr ("Communications"), this);
    m_codeLabel    = new QLabel (tr ("Robot Code"), this);
    m_sticksLabel  = new QLabel (tr ("Joysticks"), this);
    m_voltageLabel = new QLabel (QChar (fa::battery0), this);

    /* Create container widgets */
    m_LEDsWidget   = new QWidget (this);
    m_upperWidget  = new QWidget (this);

    /* Do not allow the user to change state of LEDs */
    m_comm->setEnabled   (false);
    m_code->setEnabled   (false);
    m_sticks->setEnabled (false);
}

//==================================================================================================
// Status::createLayouts
//==================================================================================================

void Status::createLayouts() {
    /* Configure layout of LED indicators & captions */
    m_LEDsLayout = new QGridLayout      (m_LEDsWidget);
    m_LEDsLayout->addWidget             (m_commLabel,   0, 0);
    m_LEDsLayout->addWidget             (m_codeLabel,   1, 0);
    m_LEDsLayout->addWidget             (m_sticksLabel, 2, 0);
    m_LEDsLayout->addWidget             (m_comm,        0, 1);
    m_LEDsLayout->addWidget             (m_code,        1, 1);
    m_LEDsLayout->addWidget             (m_sticks,      2, 1);

    /* Configure layout of voltage and team indicators */
    m_upperLayout = new QGridLayout     (m_upperWidget);
    m_upperLayout->addWidget            (m_teamLabel,    0, 0);
    m_upperLayout->addWidget            (m_voltageLabel, 1, 0);
    m_upperLayout->addWidget            (m_team,         0, 1);
    m_upperLayout->addWidget            (m_voltage,      1, 1);

    /* Configure main layout */
    m_mainLayout = new QVBoxLayout      (this);
    m_mainLayout->addWidget             (m_upperWidget);
    m_mainLayout->addWidget             (m_LEDsWidget);
    m_mainLayout->addWidget             (m_robotStatus);

    /* Set layout spacing */
    m_mainLayout->setSpacing            (DPI_SCALE (5));
    m_LEDsLayout->setSpacing            (DPI_SCALE (5));
    m_upperLayout->setSpacing           (DPI_SCALE (10));
    m_LEDsLayout->setHorizontalSpacing  (DPI_SCALE (10));
    m_upperLayout->setHorizontalSpacing (DPI_SCALE (25));

    /* Set margins */
    m_mainLayout->setContentsMargins    (DPI_SCALE (5),
                                         DPI_SCALE (5),
                                         DPI_SCALE (5),
                                         DPI_SCALE (5));
    m_LEDsLayout->setContentsMargins    (DPI_SCALE (5),
                                         DPI_SCALE (5),
                                         DPI_SCALE (5),
                                         DPI_SCALE (5));
    m_upperLayout->setContentsMargins   (DPI_SCALE (5),
                                         DPI_SCALE (5),
                                         DPI_SCALE (5),
                                         DPI_SCALE (5));
}

//==================================================================================================
// Status::configureStyles
//==================================================================================================

void Status::configureStyles() {
    /* Set style of LEDs*/
    m_comm->setFlat               (true);
    m_code->setFlat               (true);
    m_sticks->setFlat             (true);
    m_comm->setCheckable          (true);
    m_code->setCheckable          (true);
    m_sticks->setCheckable        (true);
    m_comm->setStyleSheet         (CUSTM_LED);
    m_code->setStyleSheet         (NORMAL_LED);
    m_sticks->setStyleSheet       (NORMAL_LED);

    /* Resize the LEDs */
    m_comm->setFixedSize          (DPI_SCALE (24), DPI_SCALE (14));
    m_code->setFixedSize          (DPI_SCALE (24), DPI_SCALE (14));
    m_sticks->setFixedSize        (DPI_SCALE (24), DPI_SCALE (14));

    /* Set the alignment the indicator labels */
    m_team->setAlignment          (Qt::AlignCenter);
    m_robotStatus->setAlignment   (Qt::AlignCenter);
    m_robotStatus->setWordWrap    (true);

    /* Align the LED captions to the right */
    m_commLabel->setFixedHeight   (DPI_SCALE (14));
    m_codeLabel->setFixedHeight   (DPI_SCALE (14));
    m_sticksLabel->setFixedHeight (DPI_SCALE (14));
    m_commLabel->setAlignment     (Qt::AlignCenter | Qt::AlignRight);
    m_codeLabel->setAlignment     (Qt::AlignCenter | Qt::AlignRight);
    m_sticksLabel->setAlignment   (Qt::AlignCenter | Qt::AlignRight);

    /* Configure indicator font */
    QFont indicatorFont;
    indicatorFont.setBold         (true);
    indicatorFont.setPixelSize    (DPI_SCALE (14));

    /* Configure robot status font */
    QFont robotStatusFont;
    robotStatusFont.setBold       (true);
    robotStatusFont.setPixelSize  (DPI_SCALE (15));

    /* Apply the configured fonts */
    m_team->setFont               (indicatorFont);
    m_voltage->setFont            (indicatorFont);
    m_teamLabel->setFont          (indicatorFont);
    m_team->setFont               (indicatorFont);
    m_robotStatus->setFont        (robotStatusFont);
    m_voltageLabel->setFont       (AWESOME()->font (DPI_SCALE (42)));

    /* Set the size of the voltage indicator */
    QFontMetrics metrics (m_voltage->font());
    m_voltage->setFixedWidth  (metrics.width (_VOLTAGE_FORMAT));
    m_voltage->setFixedHeight (DPI_SCALE (24));
    m_voltage->setAlignment   (Qt::AlignLeft | Qt::AlignVCenter);
}

//==================================================================================================
// Status::connectSlots
//==================================================================================================

void Status::connectSlots() {
    connect (QDS(), &DriverStation::joystickCountChanged,
             this,  &Status::updateJoysticks);
    connect (QDS(), &DriverStation::communicationsChanged,
             this,  &Status::updateCommStatus);
    connect (QDS(), &DriverStation::codeChanged,
             this,  &Status::updateCodeStatus);
    connect (QDS(), &DriverStation::voltageChanged,
             this,  &Status::updateVoltage);
    connect (QDS(), &DriverStation::robotStatusChanged,
             this,  &Status::updateStatus);
    connect (QDS(), &DriverStation::teamChanged,
             this,  &Status::updateTeam);
    connect (QDS(), &DriverStation::emergencyStopped,
             this,  &Status::updateEmergencyStop);
}

//==================================================================================================
// Status::updateJoysticks
//==================================================================================================

void Status::updateJoysticks (void) {
    m_sticks->setChecked (QDS()->joystickCount() > 0);
}

//==================================================================================================
// Status::updateEmergencyStop
//==================================================================================================

void Status::updateEmergencyStop (void) {
    if (QDS()->isEmergencyStopped() && !m_estop) {
        m_estop = true;
        BEEPER()->beep (660, 800);
    }
}

//==================================================================================================
// Status::updateTeam
//==================================================================================================

void Status::updateTeam (int team) {
    m_team->setText (QString::number (team));
}

//==================================================================================================
// Status::updateStatus
//==================================================================================================

void Status::updateStatus (QString status) {
    if (!QDS()->isPractice())
        m_robotStatus->setText (status);

    else
        m_robotStatus->setText (tr ("Practice Match"));
}

//==================================================================================================
// Status::updateVoltage
//==================================================================================================

void Status::updateVoltage (QString voltage) {
    if (!voltage.isEmpty() && QDS()->isConnected())
        m_voltage->setText (_SPACER + QString ("%1 V").arg (voltage));

    else
        m_voltage->setText (_SPACER + NO_DATA);
}

//==================================================================================================
// Status::toggleStatusColors
//==================================================================================================

void Status::toggleStatusColors() {
    QPalette palette;
    QColor redColor = QColor (255, 8, 21);

    /* Decide the color to use on the label */
    if (m_robotStatus->palette().windowText().color() != redColor)
        palette.setColor (QPalette::WindowText, redColor);

    /* Flash status label */
    m_robotStatus->setPalette (palette);

    /* Flash communications label */
    if (!m_comm->isChecked())
        m_commLabel->setPalette (palette);

    /* Flash robot code label */
    if (!m_code->isChecked())
        m_codeLabel->setPalette (palette);
}

//==================================================================================================
// Status::updateCodeStatus
//==================================================================================================

void Status::updateCodeStatus (bool codeAvailable) {
    m_comm->setChecked (QDS()->isConnected());
    m_code->setChecked (codeAvailable);
}

//==================================================================================================
// Status::updateCommStatus
//==================================================================================================

void Status::updateCommStatus (DS::CommStatus status) {
    m_comm->setChecked (status != DS::kFailing);
    m_comm->setStyleSheet (status != DS::kPartial ? NORMAL_LED : CUSTM_LED);
}
