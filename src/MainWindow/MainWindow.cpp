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

#include "MainWindow.h"

#include <QTimer>
#include <QPalette>
#include <QClipboard>
#include <QDesktopServices>
#include <LibDS/Core/Timers.h>

#include "Global/Settings.h"
#include "Global/AssemblyInfo.h"

#include "Updater/Updater.h"
#include "Joysticks/JoysticksTab.h"
#include "SettingsDialog/SettingsDialog.h"

#include "Utilities/CPU.h"
#include "Utilities/Battery.h"
#include "Utilities/FirstRun.h"
#include "Utilities/Dashboards.h"
#include "Utilities/SmartProgressbar.h"

/*
 * Much code, very mess, wow
 * -------------------------
 * This file is so long because it must deal with very specific UI components.
 * If you want to understand  something, I recommend you to go to the header
 * file  and CTLR + Click a function/method that you are interested in.
 */

/* Enable/Disable buttons stylesheets */
const QString ENABLED_UNCHECK  = "color: rgb(0, 47, 2);";
const QString ENABLED_CHECK    = "color: rgb(33, 255, 43);";
const QString DISABLED_UNCHECK = "color: rgb(43, 0, 0); border-left: 0px;";
const QString DISABLED_CHECK   = "color: rgb(255, 8,  21); border-left: 0px;";

/* Status LED, red when uncheked, green when checked */
const QString NORMAL_STATUS_LED = "QPushButton {"
                                  "background-color: rgb(255, 37, 43);"
                                  "border: 0px; padding: 4px; }"
                                  "QPushButton:checked {"
                                  "background-color: rgb(33, 255, 43); }";

/* Status LED, red when uncheked, yellow when checked */
const QString PARTIAL_STATUS_LED = "QPushButton {"
                                   "background-color: rgb(255, 37, 43);" ""
                                   "border: 0px; padding: 4px; }"
                                   "QPushButton:checked {"
                                   "background-color: rgb(244, 255, 43); }";

/* The font used in the NetConsole text edit */
#if defined Q_OS_WIN
const QFont NETCONSOLE_FONT = QFont ("Consolas", 10);
#else
const QFont NETCONSOLE_FONT = QFont ("Inconsolata", 13);
#endif

//=============================================================================
// MainWindow::MainWindow
//=============================================================================

MainWindow::MainWindow()
{
    ui = new Ui::MainWindow;
    ui->setupUi (this);
    m_updater = new Updater;
    m_tabStyle = new CustomTabStyle;
    m_advancedSettings = new SettingsDialog;
    m_joysticksWidget = new JoysticksTab (ui->JoysticksTab);

    ConnectSlots();
    ConfigureAppearance();

    ReadSettings();
    RedrawControls();

    SetTeamNumber (InitTasks::GetTeamNumber());

    CPU::InitQueryProcess();
    InitTasks::FirstRunTasks();
    DriverStation::GetInstance()->Init();

    ui->WindowDocked->setChecked (Settings::Get ("Docked", false).toBool());
    SetWindowMode (ui->WindowDocked->isChecked() ? kDocked : kNormal);
}

//=============================================================================
// MainWindow::~MainWindow
//=============================================================================

MainWindow::~MainWindow()
{
    delete ui;
    delete m_updater;
    delete m_tabStyle;
    delete m_joysticksWidget;
    delete m_advancedSettings;
}

//=============================================================================
// MainWindow::ConnectSlots
//=============================================================================

void MainWindow::ConnectSlots()
{
    /* Joystick information tab */
    ui->JoysticksTab->layout()->addWidget (m_joysticksWidget);
    UpdateJoysticksTab (false);

    connect (m_joysticksWidget, SIGNAL (JoystickRemoved()),
             this,              SLOT   (DisableRobot()));
    connect (m_joysticksWidget, SIGNAL (StatusChanged (bool)),
             this,              SLOT   (UpdateJoysticksTab (bool)));
    connect (m_joysticksWidget, SIGNAL (StatusChanged (bool)),
             ui->Joysticks,     SLOT   (setChecked (bool)));
    connect (ui->KeysButton,    SIGNAL (clicked()),
             m_joysticksWidget, SLOT   (ShowKeyboardWindow()));

    /* DriverStation to MainWindow */
    m_ds = DriverStation::GetInstance();
    connect (m_ds, SIGNAL (CodeChanged             (bool)),
             this, SLOT   (UpdateRobotCodeLED      (bool)));
    connect (m_ds, SIGNAL (CommunicationsChanged   (DS_CommStatus)),
             this, SLOT   (UpdateCommunicationsLED (DS_CommStatus)));
    connect (m_ds, SIGNAL (RadioChanged            (bool)),
             this, SLOT   (UpdateRadioCheckbox     (bool)));
    connect (m_ds, SIGNAL (VoltageChanged          (QString)),
             this, SLOT   (UpdateVoltageLabel      (QString)));
    connect (m_ds, SIGNAL (LibVersionChanged       (QString)),
             this, SLOT   (UpdateLibVersionLabel   (QString)));
    connect (m_ds, SIGNAL (RIOVersionChanged       (QString)),
             this, SLOT   (UpdateRIOVersionLabel   (QString)));
    connect (m_ds, SIGNAL (RobotStatusChanged      (QString)),
             this, SLOT   (UpdateClientStatus      (QString)));
    connect (m_ds, SIGNAL (RAMUsageChanged         (int)),
             this, SLOT   (UpdateRAMUsage          (int)));
    connect (m_ds, SIGNAL (DiskUsageChanged        (int)),
             this, SLOT   (UpdateDiskUsage         (int)));
    connect (m_ds, SIGNAL (ControlModeChanged      (DS_ControlMode)),
             this, SLOT   (UpdateRobotModeControls (DS_ControlMode)));
    connect (DS_Timers::GetInstance(), SIGNAL      (Timeout1000()),
             this,                     SLOT        (UpdatePCStatusControls()));

    /* DriverStation to MainWindow UI */
    connect (m_ds,               SIGNAL (NewMessage         (QString)),
             ui->NetConsoleEdit, SLOT   (append             (QString)));
    connect (m_ds,               SIGNAL (ElapsedTimeChanged (QString)),
             ui->ElapsedTime,    SLOT   (setText            (QString)));
    connect (ui->NetConsoleEdit, SIGNAL (textChanged()),
             this,               SLOT   (AutoscrollNetConsole()));

    /* UI to window logic */
    connect (ui->Changes,            SIGNAL (clicked()),
             this,                   SLOT   (ViewChanges()));
    connect (ui->Website,            SIGNAL (clicked()),
             this,                   SLOT   (OpenProjectWebsite()));
    connect (ui->EnableButton,       SIGNAL (clicked()),
             this,                   SLOT   (EnableRobot()));
    connect (ui->DisableButton,      SIGNAL (clicked()),
             this,                   SLOT   (DisableRobot()));
    connect (ui->RebootRioButton,    SIGNAL (clicked()),
             this,                   SLOT   (RebootRobot()));
    connect (ui->WindowDocked,       SIGNAL (clicked()),
             this,                   SLOT   (UpdateWindowDisplayMode()));
    connect (ui->WindowNormal,       SIGNAL (clicked()),
             this,                   SLOT   (UpdateWindowDisplayMode()));
    connect (ui->RestartCodeButton,  SIGNAL (clicked()),
             this,                   SLOT   (RestartRobotCode()));
    connect (ui->RobotModeGroup,     SIGNAL (buttonClicked (int)),
             this,                   SLOT   (DisableRobotWhenSwitchingModes (int)));
    connect (ui->TeamNumberSpin,     SIGNAL (valueChanged        (int)),
             this,                   SLOT   (SetTeamNumber       (int)));
    connect (ui->PracticeDelay,      SIGNAL (valueChanged        (int)),
             this,                   SLOT   (SavePracticeTimings()));
    connect (ui->PracticeTeleOp,     SIGNAL (valueChanged        (int)),
             this,                   SLOT   (SavePracticeTimings()));
    connect (ui->PracticeEndGame,    SIGNAL (valueChanged        (int)),
             this,                   SLOT   (SavePracticeTimings()));
    connect (ui->PracticeCountdown,  SIGNAL (valueChanged        (int)),
             this,                   SLOT   (SavePracticeTimings()));
    connect (ui->PracticeAutonomous, SIGNAL (valueChanged        (int)),
             this,                   SLOT   (SavePracticeTimings()));
    connect (ui->ClearButton,        SIGNAL (clicked()),
             ui->NetConsoleEdit,     SLOT   (clear()));
    connect (ui->CopyButton,         SIGNAL (clicked()),
             this,                   SLOT   (CopyNetconsoleText()));

    /* Advanced settings window */
    connect (ui->SettingsButton, SIGNAL (clicked()),
             m_advancedSettings, SLOT   (show()));
    connect (m_advancedSettings, SIGNAL (UpdateColors()),
             this,               SLOT   (RedrawControls()));
}

//=============================================================================
// MainWindow::ConfigureAppearance
//=============================================================================

void MainWindow::ConfigureAppearance()
{
    ui->TeleOp->setChecked (true);
    ui->DisableButton->setChecked (true);

    /* Rotate the text (or icons) in the tabs */
    ui->LeftTab->tabBar()->setStyle  (m_tabStyle);
    ui->RightTab->tabBar()->setStyle (m_tabStyle);

    /* Set the icon size of the tabs and buttons */
    QFont fontAwesome;
    fontAwesome.setFamily     ("FontAwesome");
    fontAwesome.setPointSize  (centralWidget()->font().pointSize() * 1.4);
    ui->LeftTab->setFont      (fontAwesome);
    ui->RightTab->setFont     (fontAwesome);
    ui->CopyButton->setFont   (fontAwesome);
    ui->ClearButton->setFont  (fontAwesome);
    ui->UtilsWidget->setFont  (fontAwesome);
    ui->WindowNormal->setFont (fontAwesome);
    ui->WindowDocked->setFont (fontAwesome);

    /* Set the voltage icon and plug icon fonts */
    QFont smallIconFont;
    QFont largeIconFont;
    smallIconFont.setFamily     ("FontAwesome");
    largeIconFont.setFamily     ("FontAwesome");
    smallIconFont.setPointSize  (centralWidget()->font().pointSize() * 1.2);
    largeIconFont.setPointSize  (centralWidget()->font().pointSize() * 3.5);
    ui->PlugIcon->setFont       (smallIconFont);
    ui->VoltageIcon->setFont    (largeIconFont);
    ui->NoJoystickIcon->setFont (largeIconFont);

    /* Restore the default application font to the widgets inside tabs */
    QFont defaultFont;
    defaultFont.setFamily       (centralWidget()->font().family());
    defaultFont.setPointSize    (centralWidget()->font().pointSize());
    ui->AboutTab->setFont       (defaultFont);
    ui->PowerTab->setFont       (defaultFont);
    ui->SetupTab->setFont       (defaultFont);
    ui->MessagesTab->setFont    (defaultFont);
    ui->JoysticksTab->setFont   (defaultFont);
    ui->OperationTab->setFont   (defaultFont);
    ui->DiagnosticsTab->setFont (defaultFont);

    /* Make some labels have a bold and bigger font */
    QFont boldFont;
    boldFont.setBold          (true);
    boldFont.setPointSize     (boldFont.pointSize() * 1.4);
    ui->VoltageLabel->setFont (boldFont);

    /* Configure the status label font */
    QFont statusFont;
    statusFont.setBold         (true);
    statusFont.setPointSize    (statusFont.pointSize() * 1.4);
    ui->TeamLabel->setFont     (statusFont);
    ui->TeamNumber->setFont    (statusFont);
    ui->StatusLabel->setFont   (statusFont);
    ui->ElapsedTime->setFont   (statusFont);
    ui->EnableButton->setFont  (statusFont);
    ui->DisableButton->setFont (statusFont);

    /* Configure status LEDs */
    ui->Joysticks->setStyleSheet      (NORMAL_STATUS_LED);
    ui->RobotCode->setStyleSheet      (NORMAL_STATUS_LED);
    ui->Communications->setStyleSheet (NORMAL_STATUS_LED);

    /* Configure the 'About' tab */
    ui->DsVersion->setText (AssemblyInfo::Version());
    ui->AppVersion->setText (QString ("Version %1").arg (AssemblyInfo::Version()));

    /* We use this to resize the UI based on text size */
    QFontMetrics metrics (boldFont);
    QSize utilSize = QSize (metrics.height() * 2, metrics.height() * 2);

    /* Configure the NetConsole */
    ui->NetConsoleEdit->setFont (NETCONSOLE_FONT);
    ui->RightTab->setMinimumWidth (utilSize.height() * 12);

    /* Configure the enable/disable buttons */
    ui->Practice->setEnabled         (false);
    ui->DisableButton->setStyleSheet (DISABLED_CHECK);
    ui->EnableButton->setStyleSheet  (ENABLED_UNCHECK);
    ui->DisableButton->setFixedWidth (utilSize.width() * 2.2);
    ui->EnableButton->setFixedWidth  (utilSize.width() * 2.2);

    /* Configure the application buttons */
    ui->KeysButton->setFixedSize     (utilSize);
    ui->CloseButton->setFixedSize    (utilSize);
    ui->SettingsButton->setFixedSize (utilSize);
    ui->WindowDocked->setFixedWidth  (utilSize.width() * 1.4);
    ui->WindowNormal->setFixedWidth  (utilSize.width() * 1.4);
    ui->WindowDocked->setFixedHeight (utilSize.height() * 0.85);
    ui->WindowNormal->setFixedHeight (utilSize.height() * 0.85);

    /* Configure the progress bars */
    ui->PcCpuProgress->setFixedHeight     (utilSize.height() * 0.45);
    ui->PcBatteryProgress->setFixedHeight (utilSize.height() * 0.45);
    ui->PcCpuProgress->setFixedWidth      (ui->WindowDocked->width() * 2);
    ui->PcBatteryProgress->setFixedWidth  (ui->WindowDocked->width() * 2);

    /* Populate list-related widgets */
    ui->StationCombo->clear();
    ui->ProtocolCombo->clear();
    ui->StationCombo->addItems (m_ds->Alliances());
    ui->ProtocolCombo->addItems (m_ds->Protocols());
    ui->DbCombo->addItems (Dashboard::GetInstance()->GetAvailableDashboards());
    ui->DbCombo->setCurrentIndex (
        Dashboard::GetInstance()->GetCurrentDashboard());

    /* Configure spacing */
    int spacing = utilSize.height() * 0.10;
    ui->PowerSpacer->setFixedHeight (spacing * 6.1);
    ui->UtilsWidget->layout()->setSpacing (spacing);
    ui->InfoFrame->layout()->setSpacing   (spacing * 1.5);
    ui->TeamSpacer->changeSize (1, spacing * 2,
                                QSizePolicy::Minimum,
                                QSizePolicy::MinimumExpanding);
}

//=============================================================================
// MainWindow::ReadSettings
//=============================================================================

void MainWindow::ReadSettings()
{
    /* Read practice values */
    int d = Settings::Get ("Practice Delay", 1).toInt();
    int t = Settings::Get ("Practice TeleOp", 100).toInt();
    int e = Settings::Get ("Practice End Game", 20).toInt();
    int c = Settings::Get ("Practice Countdown", 5).toInt();
    int a = Settings::Get ("Practice Autonomous", 15).toInt();

    /* Apply practice values */
    ui->PracticeDelay->setValue      (d);
    ui->PracticeTeleOp->setValue     (t);
    ui->PracticeEndGame->setValue    (e);
    ui->PracticeCountdown->setValue  (c);
    ui->PracticeAutonomous->setValue (a);

    /* Read team station and protocol values */
    int station  = Settings::Get ("Station",  kAllianceRed1).toInt();
    int protocol = Settings::Get ("Protocol", DriverStation::kProtocol2015).toInt();

    /* Apply saved settings without crashing app if something goes wrong */
    if (ui->ProtocolCombo->count() > protocol)
        ui->ProtocolCombo->setCurrentIndex (protocol);
    if (ui->StationCombo->count() > station)
        ui->StationCombo->setCurrentIndex (station);

    /* Update internal values */
    ChangeAlliance  (ui->StationCombo->currentIndex());
    ChangeProtocol (ui->ProtocolCombo->currentIndex());

    /* Send the saved networking config to the Driver Station */
    m_advancedSettings->ReadSettings();
    m_advancedSettings->ApplySettings();

    /* Update DriverStation and Dashboard when user changes something in UI */
    connect (ui->StationCombo,       SIGNAL (currentIndexChanged (int)),
             this,                   SLOT   (ChangeAlliance    (int)));
    connect (ui->ProtocolCombo,      SIGNAL (currentIndexChanged (int)),
             this,                   SLOT   (ChangeProtocol   (int)));
    connect (ui->DbCombo,            SIGNAL (currentIndexChanged (int)),
             this,                   SLOT   (ChangeDashboard        (int)));

    /* Configure the joysticks widget */
    m_joysticksWidget->ReadSettings();
    m_advancedSettings->ReadSettings();
}

//=============================================================================
// MainWindow::RedrawControls
//=============================================================================

void MainWindow::RedrawControls()
{
    /* Update colors of every widget*/
    QPalette palette;
    foreach (QWidget* w, findChildren<QWidget*>())
        w->setPalette (palette);

    /* Make the close button red */
    QPalette closePalette;
    QBrush brush2 (QColor (121, 0, 0, 255));
    brush2.setStyle (Qt::SolidPattern);
    closePalette.setBrush (QPalette::Active, QPalette::Button, brush2);
    QBrush brush3 (QColor (255, 255, 255, 255));
    brush3.setStyle (Qt::SolidPattern);
    closePalette.setBrush (QPalette::Active, QPalette::ButtonText, brush3);
    closePalette.setBrush (QPalette::Inactive, QPalette::Button, brush2);
    closePalette.setBrush (QPalette::Inactive, QPalette::ButtonText, brush3);
    closePalette.setBrush (QPalette::Disabled, QPalette::Button, brush2);
    QBrush brush4 (QColor (148, 148, 148, 255));
    brush4.setStyle (Qt::SolidPattern);
    closePalette.setBrush (QPalette::Disabled, QPalette::ButtonText, brush4);
    ui->CloseButton->setPalette (closePalette);

    /* Set the colors of the PC progress bars */
    ui->PcCpuProgress->SetType (SmartProgressbar::kCpuUsageProgressbar);
    ui->PcBatteryProgress->SetType (SmartProgressbar::kBatteryProgressbar);
}

//=============================================================================
// MainWindow::UpdatePCStatusControls
//=============================================================================

void MainWindow::UpdatePCStatusControls()
{
    if (ui->LeftTab->currentIndex() == 0)
        {
            int usage = CPU::GetUsage();
            int level = Battery::CurrentLevel();

            /* Update progress bars */
            ui->PcCpuProgress->setValue (usage);
            ui->PcBatteryProgress->setValue (level);

            /* Change status of AC plug icon */
            ui->PlugIcon->setVisible (Battery::IsConenctedToPowerSupply());
        }
}

//=============================================================================
// MainWindow::CopyNetconsoleText
//=============================================================================

void MainWindow::CopyNetconsoleText()
{
    qApp->clipboard()->setText (ui->NetConsoleEdit->toPlainText());
    ui->NetConsoleEdit->append ("<font color=\"#aaa\"><p>"
                                "INFO: NetConsole output copied to clipboard"
                                "</p></font>");
}

//=============================================================================
// MainWindow::ChangeAlliance
//=============================================================================

void MainWindow::ChangeAlliance (int station)
{
    Settings::Set ("Station", station);
    m_ds->SetAlliance ((DS_Alliance) station);
}

//=============================================================================
// MainWindow::ChangeProtocol
//=============================================================================

void MainWindow::ChangeProtocol (int protocol)
{
    m_ds->SetProtocol ((DriverStation::ProtocolType) protocol);
    m_ds->SetTeamNumber (ui->TeamNumberSpin->value());

    Settings::Set ("Protocol", protocol);
    m_advancedSettings->UpdateRobotAddressPlaceholder();
}

//=============================================================================
// MainWindow::RebootRobot
//=============================================================================

void MainWindow::RebootRobot()
{
    m_ds->IsConnected() ? m_ds->RebootRobot() : ErrorAnimation();
}

//=============================================================================
// MainWindow::RestartRobotCode
//=============================================================================

void MainWindow::RestartRobotCode()
{
    m_ds->IsConnected() ? m_ds->RestartCode() : ErrorAnimation();
}

//=============================================================================
// MainWindow::ViewChanges
//=============================================================================

void MainWindow::ViewChanges()
{
    QDesktopServices::openUrl (QUrl ("https://github.com/WinT-3794/"
                                     "QDriverStation/blob/master/CHANGES.md"));
}

//=============================================================================
// MainWindow::OpenProjectWebsite
//=============================================================================

void MainWindow::OpenProjectWebsite()
{
    QDesktopServices::openUrl (QUrl ("http://qdriverstation.sf.net"));
}

//=============================================================================
// MainWindow::EnableRobot
//=============================================================================

void MainWindow::EnableRobot()
{
    /* Flash status label if we cannot enable the robot */
    if (!m_ds->CanBeEnabled())
        {
            DisableRobot();
            ErrorAnimation();
            return;
        }

    /* Enable robot and update application style */
    EnableRobot (true);
    ui->EnableButton->setChecked (true);
    ui->EnableButton->setStyleSheet  (ENABLED_CHECK);
    ui->DisableButton->setStyleSheet (DISABLED_UNCHECK);
}

//=============================================================================
// MainWindow::DisableRobot
//=============================================================================

void MainWindow::DisableRobot()
{
    EnableRobot (false);

    /* Update the application style */
    ui->DisableButton->setChecked (true);
    ui->DisableButton->setStyleSheet (DISABLED_CHECK);
    ui->EnableButton->setStyleSheet  (ENABLED_UNCHECK);
}

//=============================================================================
// MainWindow::UpdateJoysticksTab
//=============================================================================

void MainWindow::UpdateJoysticksTab (bool available)
{
    m_joysticksWidget->setVisible (available);
    ui->NoJoystickWidget->setVisible (!available);
}

//=============================================================================
// MainWindow::UpdateWindowDisplayMode
//=============================================================================

void MainWindow::UpdateWindowDisplayMode()
{
    SetWindowMode (ui->WindowDocked->isChecked() ? kDocked : kNormal);
}

//=============================================================================
// MainWindow::DisableRobotWhenSwitchingModes
//=============================================================================

void MainWindow::DisableRobotWhenSwitchingModes (int mode)
{
    Q_UNUSED (mode);

    DisableRobot();
    UpdateControlModes();
}

//=============================================================================
// MainWindow::SavePracticeTimings
//=============================================================================

void MainWindow::SavePracticeTimings()
{
    Settings::Set ("Practice Delay",      ui->PracticeDelay->value());
    Settings::Set ("Practice TeleOp",     ui->PracticeTeleOp->value());
    Settings::Set ("Practice End Game",   ui->PracticeEndGame->value());
    Settings::Set ("Practice Countdown",  ui->PracticeCountdown->value());
    Settings::Set ("Practice Autonomous", ui->PracticeAutonomous->value());
}

//=============================================================================
// MainWindow::ChangeDashboard
//=============================================================================

void MainWindow::ChangeDashboard (int dashboard)
{
    Settings::Set ("Dashboard", dashboard);
    Dashboard::GetInstance()->ReloadDashboard();
}

//=============================================================================
// MainWindow::SetTeamNumber
//=============================================================================

void MainWindow::SetTeamNumber (int team)
{
    Settings::Set ("Team ID", team);
    ui->TeamNumberSpin->setValue (team);
    ui->TeamNumber->setText (QString ("%1").arg (team));

    m_ds->SetTeamNumber (team);
    m_advancedSettings->UpdateRobotAddressPlaceholder();
}

//=============================================================================
// MainWindow::UpdateControlModes
//=============================================================================

void MainWindow::UpdateControlModes()
{
    if (ui->Test->isChecked())
        m_ds->SetControlMode (kControlTest);

    else if (ui->TeleOp->isChecked())
        m_ds->SetControlMode (kControlTeleoperated);

    else if (ui->Autonomous->isChecked())
        m_ds->SetControlMode (kControlAutonomous);

    else if (ui->Practice->isChecked())
        m_ds->Practice (ui->PracticeCountdown->value(),
                        ui->PracticeAutonomous->value(),
                        ui->PracticeDelay->value(),
                        ui->PracticeTeleOp->value(),
                        ui->PracticeEndGame->value());
}

//=============================================================================
// MainWindow::EnableRobot
//=============================================================================

void MainWindow::EnableRobot (bool enabled)
{
    UpdateControlModes();
    m_ds->SetEnabled (enabled);
}

//=============================================================================
// MainWindow::UpdateIndicator
//=============================================================================

void MainWindow::UpdateIndicator (QLabel* label, QString text)
{
    label->setText (m_ds->IsConnected() && !text.isEmpty() ? text : "--.--");
}

//=============================================================================
// MainWindow::UpdateRobotCodeLED
//=============================================================================

void MainWindow::UpdateRobotCodeLED (bool available)
{
    ui->RobotCode->setChecked (available);
}

//=============================================================================
// MainWindow::UpdateCommunicationsLED
//=============================================================================

void MainWindow::UpdateCommunicationsLED (DS_CommStatus status)
{
    ui->RobotCheck->setChecked (status == kFull);
    ui->Communications->setChecked (status != kFailing);

    /* Decide if we should use a green LED or a yellow LED */
    switch (status)
        {
        case kPartial:
            ui->Communications->setStyleSheet (PARTIAL_STATUS_LED);
            ui->Communications->setToolTip (tr ("Partial communication with robot"));
            break;
        case kFull:
            ui->Communications->setStyleSheet (NORMAL_STATUS_LED);
            ui->Communications->setToolTip (tr ("Full communication with robot"));
            break;
        case kFailing:
            ui->Communications->setToolTip (tr ("No communication with robot"));
            break;
        }

    DisableRobot();
}

//=============================================================================
// MainWindow::UpdateRobotModeControls
//=============================================================================

void MainWindow::UpdateRobotModeControls (DS_ControlMode mode)
{
    switch (mode)
        {
        case kControlAutonomous:
            ui->Autonomous->setChecked (true);
            break;
        case kControlTeleoperated:
            ui->TeleOp->setChecked (true);
            break;
        case kControlTest:
            ui->Test->setChecked (true);
            break;
        default:
            ui->DisableButton->click();
            break;
        }
}

//=============================================================================
// MainWindow::UpdateRadioCheckbox
//=============================================================================

void MainWindow::UpdateRadioCheckbox (bool available)
{
    ui->DsRadioCheck->setChecked (available);
}

//=============================================================================
// MainWindow::UpdateVoltageLabel
//=============================================================================

void MainWindow::UpdateVoltageLabel (QString voltage)
{
    UpdateIndicator (ui->VoltageLabel, tr ("%1 V").arg (voltage));
}

//=============================================================================
// MainWindow::UpdateLibVersionLabel
//=============================================================================

void MainWindow::UpdateLibVersionLabel (QString version)
{
    UpdateIndicator (ui->LibVersion, version);
}

//=============================================================================
// MainWindow::UpdateRIOVersionLabel
//=============================================================================

void MainWindow::UpdateRIOVersionLabel (QString version)
{
    UpdateIndicator (ui->RioVersion, version);
}

//=============================================================================
// MainWindow::UpdateClientStatus
//=============================================================================

void MainWindow::UpdateClientStatus (QString status)
{
    ui->StatusLabel->setText (status);
}

//=============================================================================
// MainWindow::UpdateRAMUsage
//=============================================================================

void MainWindow::UpdateRAMUsage (int percent)
{
    UpdateIndicator (ui->RamUsage, tr ("%1 %").arg (percent));
}

//=============================================================================
// MainWindow::UpdateDiskUsage
//=============================================================================

void MainWindow::UpdateDiskUsage (int percent)
{
    UpdateIndicator (ui->DiskUsage, tr ("%1 %").arg (percent));
}

//=============================================================================
// MainWindow::AutoscrollNetConsole
//=============================================================================

void MainWindow::AutoscrollNetConsole()
{
    ui->NetConsoleEdit->ensureCursorVisible();
}

//=============================================================================
// MainWindow::ToggleStatusColors
//=============================================================================

void MainWindow::ToggleStatusColors()
{
    QPalette palette;
    QColor redColor = QColor (255, 8, 21);

    /* Decide the color to use on the label */
    if (ui->StatusLabel->palette().windowText().color() != redColor)
        palette.setColor (QPalette::WindowText, redColor);

    /* Flash status label */
    ui->StatusLabel->setPalette (palette);

    /* Flash communications label */
    if (!ui->Communications->isChecked())
        ui->CommunicationsLabel->setPalette (palette);

    /* Flash robot code label */
    if (!ui->RobotCode->isChecked())
        ui->CodeLabel->setPalette (palette);
}

//=============================================================================
// MainWindow::ErrorAnimation
//=============================================================================

void MainWindow::ErrorAnimation()
{
    for (int i = 0; i < 8; ++i)
        QTimer::singleShot (100 * i, Qt::PreciseTimer,
                            this, SLOT (ToggleStatusColors()));
}
