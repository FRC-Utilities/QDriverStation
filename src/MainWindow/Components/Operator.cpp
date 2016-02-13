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
#include <QTimer>
#include <QComboBox>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QProgressBar>
#include <QFontMetrics>
#include <QButtonGroup>

//=============================================================================
// Application includes
//=============================================================================

#include "Operator.h"
#include "Global/Global.h"
#include "Global/Settings.h"
#include "Dashboards/Dashboards.h"
#include "MainWindow/System/CPU.h"
#include "MainWindow/System/Battery.h"

//=============================================================================
// CSS definitions
//=============================================================================

const QString ENABLED_UNCHECK  = "QPushButton { color: rgb(  0,  47,  2); "
                                 "border-right:  0px solid; }";
const QString ENABLED_CHECK    = "QPushButton { color: rgb( 33, 255, 43); "
                                 "border-right:  0px solid; }";
const QString DISABLED_UNCHECK = "QPushButton { color: rgb( 43,   0,  0); }";
const QString DSABLED_CHECK    = "QPushButton { color: rgb(255,   8, 21); }";
const QString MODES_CSS        = "padding: 6px;"
                                 "text-align: left;";
const QString OPERATOR_CSS     = "QPushButton {"
                                 "color:#fff;"
                                 "border: 0.05em solid rgba(0,0,0,100);"
                                 "background-color: rgba(200,200,200,10);"
                                 "padding: 8px; }"
                                 "QPushButton:disabled { "
                                 "color:#888; }"
                                 "QPushButton:checked { "
                                 "background-color: rgba(0,0,0,80); }";

//=============================================================================
// Operator::Operator
//=============================================================================

Operator::Operator (QWidget* parent) : QWidget (parent)
{
    createWidgets();
    createLayouts();
    configureStyles();

    connectSlots();
    updateProgressbars();
}

//=============================================================================
// Operator::createWidgets
//=============================================================================

void Operator::createWidgets()
{
    /* Create main container widgets */
    m_operationWidget  = new QWidget (this);
    m_indicatorsWidget = new QWidget (this);

    /* Create and configure enable/disable buttons */
    m_stateWidget      = new QWidget (this);
    m_stateGroup       = new QButtonGroup (this);
    m_enable           = new QPushButton (tr ("Enable"), this);
    m_dsable           = new QPushButton (tr ("Disable"), this);

    /* Create and configure mode checkboxes */
    m_modesWidget      = new QWidget (this);
    m_modesGroup       = new QButtonGroup (this);
    m_test             = new QPushButton (tr ("Test"),         this);
    m_teleop           = new QPushButton (tr ("Teleoperated"), this);
    m_practice         = new QPushButton (tr ("Practice"),     this);
    m_autonomous       = new QPushButton (tr ("Autonomous"),   this);

    /* Create indicators */
    m_cpu              = new QProgressBar (this);
    m_battery          = new QProgressBar (this);
    m_teamStation      = new QComboBox    (this);
    m_elapsedTime      = new QLabel (NO_DATA, this);
    m_plugIcon         = new QLabel (QChar (fa::plug), this);

    /* Create indicator captions */
    m_cpuLabel         = new QLabel (tr ("PC CPU"),       this);
    m_batteryLabel     = new QLabel (tr ("PC Battery"),   this);
    m_windowModeLabel  = new QLabel (tr ("Window Mode"),  this);
    m_elapsedTimeLabel = new QLabel (tr ("Elapsed Time"), this);
    m_teamStationLabel = new QLabel (tr ("Team Station"), this);

    /* Create window mode buttons */
    m_windowGroup      = new QButtonGroup (this);
    m_windowWidget     = new QWidget      (this);
    m_windowNormal     = new QPushButton  (QChar (fa::reply),  this);
    m_windowDocked     = new QPushButton  (QChar (fa::expand), this);

    /* Configure the button groups */
    m_stateGroup->addButton  (m_enable);
    m_stateGroup->addButton  (m_dsable);
    m_modesGroup->addButton  (m_test);
    m_modesGroup->addButton  (m_teleop);
    m_modesGroup->addButton  (m_practice);
    m_modesGroup->addButton  (m_autonomous);
    m_windowGroup->addButton (m_windowNormal);
    m_windowGroup->addButton (m_windowDocked);

    /* Change object names to identify some objects */
    m_enable->setObjectName  ("Enable button");
    m_dsable->setObjectName  ("Dsable button");
}

//=============================================================================
// Operator::createLayouts
//=============================================================================

void Operator::createLayouts()
{
    /* Configure enable/disable buttons layout */
    m_stateLayout = new QHBoxLayout          (m_stateWidget);
    m_stateLayout->setSpacing                (DPI_SCALE (0));
    m_stateLayout->setContentsMargins        (NULL_MARGINS());
    m_stateLayout->addWidget                 (m_enable);
    m_stateLayout->addWidget                 (m_dsable);

    /* Configure operation modes layout */
    m_modesLayout = new QVBoxLayout          (m_modesWidget);
    m_modesLayout->setSpacing                (DPI_SCALE (0));
    m_modesLayout->setContentsMargins        (NULL_MARGINS());
    m_modesLayout->addWidget                 (m_teleop);
    m_modesLayout->addWidget                 (m_autonomous);
    m_modesLayout->addWidget                 (m_practice);
    m_modesLayout->addWidget                 (m_test);

    /* Configure window display mode buttons layout */
    m_windowModeLayout = new QHBoxLayout     (m_windowWidget);
    m_windowModeLayout->setSpacing           (DPI_SCALE (0));
    m_windowModeLayout->setContentsMargins   (NULL_MARGINS());
    m_windowModeLayout->addWidget            (m_windowNormal);
    m_windowModeLayout->addWidget            (m_windowDocked);

    /* Configure operation container layout */
    m_operationLayout = new QVBoxLayout      (m_operationWidget);
    m_operationLayout->setSpacing            (DPI_SCALE (5));
    m_operationLayout->setContentsMargins    (NULL_MARGINS());
    m_operationLayout->addWidget             (m_modesWidget);
    m_operationLayout->addWidget             (m_stateWidget);

    /* Configure the indicator layout */
    m_indicatorsLayout = new QGridLayout     (m_indicatorsWidget);
    m_indicatorsLayout->setVerticalSpacing   (DPI_SCALE (5));
    m_indicatorsLayout->setHorizontalSpacing (DPI_SCALE (5));
    m_indicatorsLayout->setContentsMargins   (NULL_MARGINS());
    m_indicatorsLayout->addWidget            (m_elapsedTimeLabel, 0, 0);
    m_indicatorsLayout->addWidget            (m_batteryLabel,     2, 0);
    m_indicatorsLayout->addWidget            (m_cpuLabel,         3, 0);
    m_indicatorsLayout->addWidget            (m_windowModeLabel,  5, 0);
    m_indicatorsLayout->addWidget            (m_teamStationLabel, 7, 0);
    m_indicatorsLayout->addWidget            (m_plugIcon,         2, 1);
    m_indicatorsLayout->addWidget            (m_elapsedTime,      0, 2);
    m_indicatorsLayout->addItem              (SPACER(),           1, 0);
    m_indicatorsLayout->addWidget            (m_battery,          2, 2);
    m_indicatorsLayout->addWidget            (m_cpu,              3, 2);
    m_indicatorsLayout->addItem              (SPACER(),           4, 1);
    m_indicatorsLayout->addWidget            (m_windowWidget,     5, 2);
    m_indicatorsLayout->addItem              (SPACER(),           6, 0);
    m_indicatorsLayout->addWidget            (m_teamStation,      7, 2);
    m_indicatorsLayout->addItem              (SPACER(),           8, 1);

    /* Configure the main layout */
    m_mainLayout = new QHBoxLayout           (this);
    m_mainLayout->setSpacing                 (DPI_SCALE (25));
    m_mainLayout->setContentsMargins         (MAIN_MARGINS());
    m_mainLayout->addWidget                  (m_operationWidget);
    m_mainLayout->addWidget                  (m_indicatorsWidget);

    /* Configure size of widgets */
    m_cpu->setFixedHeight                    (DPI_SCALE (16));
    m_battery->setFixedHeight                (DPI_SCALE (16));
    m_enable->setFixedHeight                 (DPI_SCALE (48));
    m_dsable->setFixedHeight                 (DPI_SCALE (48));
    m_cpu->setFixedSize                      (DPI_SCALE (96), DPI_SCALE (18));
    m_battery->setFixedSize                  (DPI_SCALE (96), DPI_SCALE (18));
    m_windowNormal->setFixedSize             (DPI_SCALE (48), DPI_SCALE (28));
    m_windowDocked->setFixedSize             (DPI_SCALE (48), DPI_SCALE (28));
    m_teamStation->setFixedSize              (DPI_SCALE (96), DPI_SCALE (24));
}

//=============================================================================
// Operator::configureStyles
//=============================================================================

void Operator::configureStyles()
{
    /* Change the font to FontAwesome in some widgets */
    m_plugIcon->setFont           (AWESOME()->font (DPI_SCALE (12)));
    m_windowDocked->setFont       (AWESOME()->font (DPI_SCALE (14)));
    m_windowNormal->setFont       (AWESOME()->font (DPI_SCALE (14)));

    /* Configure style of enable/disable buttons */
    m_enable->setFlat             (true);
    m_dsable->setFlat             (true);
    m_dsable->setDefault          (true);
    m_enable->setCheckable        (true);
    m_dsable->setCheckable        (true);
    m_practice->setEnabled        (false);

    /* Configure the style of the mode buttons */
    m_test->setFlat               (true);
    m_teleop->setFlat             (true);
    m_practice->setFlat           (true);
    m_autonomous->setFlat         (true);
    m_test->setCheckable          (true);
    m_teleop->setCheckable        (true);
    m_practice->setCheckable      (true);
    m_autonomous->setCheckable    (true);

    /* Configure the style of the window mode buttons */
    m_windowDocked->setFlat       (true);
    m_windowNormal->setFlat       (true);
    m_windowDocked->setCheckable  (true);
    m_windowNormal->setCheckable  (true);

    /* Configure default checked buttons */
    m_teleop->setChecked          (true);
    m_dsable->setChecked          (true);
    m_windowDocked->setChecked    (Settings::get ("Docked", false).toBool());
    m_windowNormal->setChecked    (!m_windowDocked->isChecked());

    /* Put the available alliances & positions in the station combo */
    m_teamStation->insertItems    (0, DS()->alliances());

    /* Setup le CSS */
    setStyleSheet (OPERATOR_CSS);
    m_modesWidget->setStyleSheet  (MODES_CSS);
    m_teleop->setStyleSheet       ("border-bottom: 0px solid;");
    m_autonomous->setStyleSheet   ("border-bottom: 0px solid;");
    m_practice->setStyleSheet     ("border-bottom: 0px solid;");
    m_windowNormal->setStyleSheet ("border-right:  0px solid;");

    /* Le font style */
    QFont font;
    font.setBold                  (true);
    font.setPixelSize             (DPI_SCALE (14));
    m_enable->setFont             (font);
    m_dsable->setFont             (font);
    m_elapsedTime->setFont        (font);

    /* Update the enable/disable state and appearance */
    updateEnableState();

    /* Resize the buttons to fit */
    QFontMetrics metrics (font);
    int dsableWidth = metrics.width (m_dsable->text()) * 1.15;
    int enableWidth = metrics.width (m_enable->text()) * 1.15;
    int buttonWidth = dsableWidth > enableWidth ? dsableWidth : enableWidth;
    buttonWidth = buttonWidth < DPI_SCALE (74) ? DPI_SCALE (74) : buttonWidth;
    m_dsable->setFixedWidth (buttonWidth);
    m_enable->setFixedWidth (buttonWidth);

    /* Update progressbar style */
    CPU::initQueryProcess();
    m_cpu->setFormat              ("");
    m_battery->setFormat          ("");
    m_plugIcon->setVisible        (false);
}

//=============================================================================
// Operator::connectSlots
//=============================================================================

void Operator::connectSlots()
{
    connect (m_enable,                  SIGNAL (clicked             (void)),
             this,                        SLOT (updateEnableState   (void)));
    connect (m_dsable,                  SIGNAL (clicked             (void)),
             this,                        SLOT (updateEnableState   (void)));
    connect (m_windowDocked,            SIGNAL (clicked             (void)),
             this,                      SIGNAL (showDocked          (void)));
    connect (m_windowNormal,            SIGNAL (clicked             (void)),
             this,                      SIGNAL (showUnDocked        (void)));
    connect (DS(),                      SIGNAL (elapsedTimeChanged  (QString)),
             m_elapsedTime,               SLOT (setText             (QString)));
    connect (m_modesGroup,              SIGNAL (buttonClicked       (int)),
             this,                        SLOT (updateControlMode   (int)));
    connect (m_teamStation,             SIGNAL (currentIndexChanged (int)),
             DS(),                        SLOT (setAlliance         (int)));
    connect (Dashboards::getInstance(), SIGNAL (dashboardChanged    (void)),
             this,                        SLOT (updateWindowState   (void)));
}

//=============================================================================
// Operator::updateEnableState
//=============================================================================

void Operator::updateEnableState()
{
    bool enabled = m_enable->isChecked() && DS()->canBeEnabled();
    DS()->setEnabled (enabled);

    /* Make the enable button light green */
    if (enabled)
        {
            m_enable->setStyleSheet (ENABLED_CHECK);
            m_dsable->setStyleSheet (DISABLED_UNCHECK);
        }

    /* Make the disable button light red */
    else
        {
            m_dsable->setStyleSheet (DSABLED_CHECK);
            m_enable->setStyleSheet (ENABLED_UNCHECK);
        }

    /* Error animation if user clicked on enabled and something is f*cked up */
    if (sender() != Q_NULLPTR)
        {
            if (!enabled && sender()->objectName() == m_enable->objectName())
                {
                    m_dsable->setChecked (true);
                    emit requestErrorAnimation();
                }
        }
}

//=============================================================================
// Operator::updateWindowState
//=============================================================================

void Operator::updateWindowState()
{
    if (m_windowDocked->isChecked())
        emit showDocked();

    else
        emit showUnDocked();
}

//=============================================================================
// Operator::updateProgressbars
//=============================================================================

void Operator::updateProgressbars()
{
    if (m_cpu && m_battery && m_plugIcon)
        {
            m_cpu->setValue (CPU::getUsage());
            m_battery->setValue (Battery::currentLevel());
            m_plugIcon->setVisible (Battery::isConenctedToPowerSupply());

            /* Create CPU palette */
            QPalette cpuPalette;
            cpuPalette.setColor (QPalette::Highlight, QColor ("#dcae00"));

            /* Create battery palette */
            QPalette batteryPalette;
            batteryPalette.setColor (QPalette::Highlight,
                                     QColor (m_battery->value() <= 25 ?
                                             "#ee141a" :
                                             "#deae00"));


            /* Apply palettes */
            m_cpu->setPalette (cpuPalette);
            m_battery->setPalette (batteryPalette);
        }

    /* Start a timer */
    QTimer::singleShot (1000, Qt::CoarseTimer, this, SLOT (updateProgressbars()));
}

//=============================================================================
// Operator::updateControlMode
//=============================================================================

void Operator::updateControlMode (int index)
{
    Q_UNUSED (index);
    DS_ControlMode mode = kControlInvalid;

    if (m_teleop->isChecked())
        mode = kControlTeleoperated;

    else if (m_autonomous->isChecked())
        mode = kControlAutonomous;

    else if (m_test->isChecked())
        mode = kControlTest;

    DS()->setControlMode (mode);
}
