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

#define d_EnabledCheck    "color: rgb(33, 255, 43);"
#define d_DisabledCheck   "color: rgb(255, 8,  21); border-left: 0px;"
#define d_EnabledUncheck  "color: rgb(0, 47, 2);"
#define d_DisabledUncheck "color: rgb(43, 0, 0); border-left: 0px;"

#define d_NormalStatusLED  \
    "QPushButton { background-color: rgb(255, 37, 43); border: 0px; padding: 4px; }" \
    "QPushButton:checked { background-color: rgb(33, 255, 43); }"

#define d_PartialStatusLED  \
    "QPushButton { background-color: rgb(255, 37, 43); border: 0px; padding: 4px; }" \
    "QPushButton:checked { background-color: rgb(244, 255, 43); }"

/*
 * The font used in the NetConsole text edit
 */
#if defined Q_OS_WIN
#  define d_NetConsoleFont QFont ("Consolas", 10)
#else
#  define d_NetConsoleFont QFont ("Inconsolata", 13)
#endif

MainWindow::MainWindow() {
    ui = new Ui::MainWindow;
    ui->setupUi (this);
    m_updater = new Updater;
    m_tabStyle = new CustomTabStyle;
    m_advancedSettings = new SettingsDialog;
    m_joysticksWidget = new JoysticksTab (ui->JoysticksTab);

    connectSlots();
    configureWidgetAppearance();

    readSettings();
    redrawWidgets();

    setTeamNumber (InitTasks::getTeamNumber());
    InitTasks::executeFirstRunTasks();
    DriverStation::getInstance()->init();

    ui->WindowDocked->setChecked (Settings::get ("Docked", false).toBool());
    setWindowMode (ui->WindowDocked->isChecked() ? kDocked : kNormal);
}

MainWindow::~MainWindow() {
    delete ui;
    delete m_updater;
    delete m_tabStyle;
    delete m_joysticksWidget;
    delete m_advancedSettings;
}

//------------------------------------------------------------------------------
// LONG & UGLY FUNCTIONS
//------------------------------------------------------------------------------

void MainWindow::connectSlots() {
    /* Joystick information tab */
    ui->JoysticksTab->layout()->addWidget (m_joysticksWidget);
    updateJoysticksTab (false);
    connect (m_joysticksWidget, SIGNAL (joystickRemoved()),
             this,              SLOT   (onJoystickRemoved()));
    connect (m_joysticksWidget, SIGNAL (statusChanged (bool)),
             this,              SLOT   (updateJoysticksTab (bool)));
    connect (m_joysticksWidget, SIGNAL (statusChanged (bool)),
             ui->Joysticks,     SLOT   (setChecked (bool)));
    connect (ui->KeysButton,    SIGNAL (clicked()),
             m_joysticksWidget, SLOT   (showKeyboardWindow()));

    /* DriverStation to MainWindow */
    m_ds = DriverStation::getInstance();
    connect (m_ds, SIGNAL (codeChanged             (bool)),
             this, SLOT   (onCodeChanged           (bool)));
    connect (m_ds, SIGNAL (communicationsChanged   (DS_CommunicationStatus)),
             this, SLOT   (onCommunicationsChanged (DS_CommunicationStatus)));
    connect (m_ds, SIGNAL (radioChanged            (bool)),
             this, SLOT   (onRadioChanged          (bool)));
    connect (m_ds, SIGNAL (voltageChanged          (QString)),
             this, SLOT   (onVoltageChanged        (QString)));
    connect (m_ds, SIGNAL (libVersionChanged       (QString)),
             this, SLOT   (onLibVersionChanged     (QString)));
    connect (m_ds, SIGNAL (rioVersionChanged       (QString)),
             this, SLOT   (onRioVersionChanged     (QString)));
    connect (m_ds, SIGNAL (robotStatusChanged      (QString)),
             this, SLOT   (onRobotStatusChanged    (QString)));
    connect (m_ds, SIGNAL (ramUsageChanged         (quint8)),
             this, SLOT   (onRamUsageChanged       (quint8)));
    connect (m_ds, SIGNAL (diskUsageChanged        (quint8)),
             this, SLOT   (onDiskUsageChanged      (quint8)));
    connect (m_ds, SIGNAL (controlModeChanged      (DS_ControlMode)),
             this, SLOT   (onControlModeChanged    (DS_ControlMode)));
    connect (DS_Timers::getInstance(), SIGNAL (timeout1000()),
             this,                     SLOT   (updatePcStatusWidgets()));

    /* DriverStation to MainWindow UI */
    connect (m_ds,               SIGNAL (newMessage         (QString)),
             ui->NetConsoleEdit, SLOT   (append             (QString)));
    connect (m_ds,               SIGNAL (elapsedTimeChanged (QString)),
             ui->ElapsedTime,    SLOT   (setText            (QString)));
    connect (ui->NetConsoleEdit, SIGNAL (textChanged()),
             this,               SLOT   (scrollNetConsole()));

    /* UI to window logic */
    connect (ui->Changes,            SIGNAL (clicked()),
             this,                   SLOT   (onChangesClicked()));
    connect (ui->Website,            SIGNAL (clicked()),
             this,                   SLOT   (onWebsiteClicked()));
    connect (ui->EnableButton,       SIGNAL (clicked()),
             this,                   SLOT   (onEnabledClicked()));
    connect (ui->DisableButton,      SIGNAL (clicked()),
             this,                   SLOT   (onDisabledClicked()));
    connect (ui->RebootRioButton,    SIGNAL (clicked()),
             this,                   SLOT   (onRebootClicked()));
    connect (ui->WindowDocked,       SIGNAL (clicked()),
             this,                   SLOT   (onWindowModeChanged()));
    connect (ui->WindowNormal,       SIGNAL (clicked()),
             this,                   SLOT   (onWindowModeChanged()));
    connect (ui->RestartCodeButton,  SIGNAL (clicked()),
             this,                   SLOT   (onRestartClicked()));
    connect (ui->RobotModeGroup,     SIGNAL (buttonClicked       (int)),
             this,                   SLOT   (onRobotModeChanged  (int)));
    connect (ui->TeamNumberSpin,     SIGNAL (valueChanged        (int)),
             this,                   SLOT   (setTeamNumber       (int)));
    connect (ui->PracticeDelay,      SIGNAL (valueChanged        (int)),
             this,                   SLOT   (onPracticeValuesChanged()));
    connect (ui->PracticeTeleOp,     SIGNAL (valueChanged        (int)),
             this,                   SLOT   (onPracticeValuesChanged()));
    connect (ui->PracticeEndGame,    SIGNAL (valueChanged        (int)),
             this,                   SLOT   (onPracticeValuesChanged()));
    connect (ui->PracticeCountdown,  SIGNAL (valueChanged        (int)),
             this,                   SLOT   (onPracticeValuesChanged()));
    connect (ui->PracticeAutonomous, SIGNAL (valueChanged        (int)),
             this,                   SLOT   (onPracticeValuesChanged()));
    connect (ui->ClearButton,        SIGNAL (clicked()),
             ui->NetConsoleEdit,     SLOT   (clear()));
    connect (ui->CopyButton,         SIGNAL (clicked()),
             this,                   SLOT   (onCopyClicked()));

    /* Advanced settings window */
    connect (ui->SettingsButton, SIGNAL (clicked()),
             m_advancedSettings, SLOT   (show()));
    connect (m_advancedSettings, SIGNAL (updateColors()),
             this,               SLOT   (redrawWidgets()));
}

void MainWindow::configureWidgetAppearance() {
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
    ui->AppIcon->setFont        (largeIconFont);
    ui->CanIcon->setFont        (largeIconFont);
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
    ui->Joysticks->setStyleSheet      (d_NormalStatusLED);
    ui->RobotCode->setStyleSheet      (d_NormalStatusLED);
    ui->Communications->setStyleSheet (d_NormalStatusLED);

    /* Configure the 'About' tab */
    ui->DsVersion->setText (AssemblyInfo::version());
    ui->AppName->setText (QString ("<h2>%1</h2>").arg (AssemblyInfo::name()));
    ui->AppVersion->setText (QString ("Version %1").arg (AssemblyInfo::version()));

    /* We use this to resize the UI based on text size */
    QFontMetrics metrics (boldFont);
    QSize utilSize = QSize (metrics.height() * 2, metrics.height() * 2);

    /* Configure the NetConsole */
    ui->NetConsoleEdit->setFont (d_NetConsoleFont);
    ui->RightTab->setMinimumWidth (utilSize.height() * 12);

    /* Configure the enable/disable buttons */
    ui->DisableButton->setStyleSheet (d_DisabledCheck);
    ui->EnableButton->setStyleSheet  (d_EnabledUncheck);
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
    ui->StationCombo->addItems (m_ds->alliances());
    ui->ProtocolCombo->addItems (m_ds->protocols());
    ui->DbCombo->addItems (Dashboard::getInstance()->getAvailableDashboards());
    ui->DbCombo->setCurrentIndex (
        Dashboard::getInstance()->getCurrentDashboard());

    /* Configure spacing */
    int spacing = utilSize.height() * 0.10;
    ui->PowerSpacer->setFixedHeight (spacing * 6.1);
    ui->UtilsWidget->layout()->setSpacing (spacing);
    ui->InfoFrame->layout()->setSpacing   (spacing * 1.5);
    ui->TeamSpacer->changeSize (1, spacing * 2,
                                QSizePolicy::Minimum,
                                QSizePolicy::MinimumExpanding);

    /* Load license */
    QFile license (":/texts/License.txt");
    if (license.open (QFile::ReadOnly))
        ui->License->setPlainText (QString::fromUtf8 (license.readAll()));

    /* Load authors */
    QFile authors (":/texts/Authors.html");
    if (authors.open (QFile::ReadOnly))
        ui->Authors->setText (QString::fromUtf8 (authors.readAll()));
}

void MainWindow::readSettings() {
    /* Read practice values */
    int d = Settings::get ("Practice Delay", 1).toInt();
    int t = Settings::get ("Practice TeleOp", 100).toInt();
    int e = Settings::get ("Practice End Game", 20).toInt();
    int c = Settings::get ("Practice Countdown", 5).toInt();
    int a = Settings::get ("Practice Autonomous", 15).toInt();

    /* Apply practice values */
    ui->PracticeDelay->setValue      (d);
    ui->PracticeTeleOp->setValue     (t);
    ui->PracticeEndGame->setValue    (e);
    ui->PracticeCountdown->setValue  (c);
    ui->PracticeAutonomous->setValue (a);

    /* Read team station and protocol values */
    int station  = Settings::get ("Station",  kAllianceRed1).toInt();
    int protocol = Settings::get ("Protocol", DriverStation::kProtocol2015).toInt();

    /* Apply saved settings without crashing app if something goes wrong */
    if (ui->ProtocolCombo->count() > protocol)
        ui->ProtocolCombo->setCurrentIndex (protocol);
    if (ui->StationCombo->count() > station)
        ui->StationCombo->setCurrentIndex (station);

    /* Update internal values */
    onStationChanged  (ui->StationCombo->currentIndex());
    onProtocolChanged (ui->ProtocolCombo->currentIndex());

    /* Send the saved networking config to the Driver Station */
    m_advancedSettings->readSettings();
    m_advancedSettings->applySettings();

    /* Update DriverStation and Dashboard when user changes something in UI */
    connect (ui->StationCombo,       SIGNAL (currentIndexChanged (int)),
             this,                   SLOT   (onStationChanged    (int)));
    connect (ui->ProtocolCombo,      SIGNAL (currentIndexChanged (int)),
             this,                   SLOT   (onProtocolChanged   (int)));
    connect (ui->DbCombo,            SIGNAL (currentIndexChanged (int)),
             this,                   SLOT   (setDashboard        (int)));

    /* Configure the joysticks widget */
    m_joysticksWidget->readSettings();
    m_advancedSettings->readSettings();
}

//------------------------------------------------------------------------------
// FUNCTIONS THAT MAY BE CALLED REGULARLY
//------------------------------------------------------------------------------

void MainWindow::redrawWidgets() {
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
    ui->PcCpuProgress->setType (SmartProgressbar::kCpuUsageProgressbar);
    ui->PcBatteryProgress->setType (SmartProgressbar::kBatteryProgressbar);
}

void MainWindow::updatePcStatusWidgets() {
    if (ui->LeftTab->currentIndex() == 0) {
        int usage = CPU::getUsage();
        int level = Battery::currentLevel();

        /* Update progress bars */
        ui->PcCpuProgress->setValue (usage);
        ui->PcBatteryProgress->setValue (level);

        /* Change status of AC plug icon */
        ui->PlugIcon->setVisible (Battery::isPlugged());
    }
}

//------------------------------------------------------------------------------
// UI TO WINDOW-LOGIC FUNCTIONS
//------------------------------------------------------------------------------

void MainWindow::onCopyClicked() {
    qApp->clipboard()->setText (ui->NetConsoleEdit->toPlainText());
    ui->NetConsoleEdit->append ("<font color=\"#aaa\"><p>"
                                "INFO: NetConsole output copied to clipboard"
                                "</p></font>");
}

void MainWindow::onStationChanged (int station) {
    Settings::set ("Station", station);
    m_ds->setAlliance ((DS_Alliance) station);
}

void MainWindow::onProtocolChanged (int protocol) {
    m_ds->setProtocol ((DriverStation::ProtocolType) protocol);
    m_ds->setTeamNumber (ui->TeamNumberSpin->value());

    Settings::set ("Protocol", protocol);
    m_advancedSettings->updatePlaceholder();
}

void MainWindow::onRebootClicked() {
    m_ds->canBeEnabled() ? m_ds->reboot() : statusLabelAnimation();
}

void MainWindow::onRestartClicked() {
    m_ds->canBeEnabled() ? m_ds->restartCode() : statusLabelAnimation();
}

void MainWindow::onChangesClicked() {
    QDesktopServices::openUrl (QUrl ("https://github.com/WinT-3794/"
                                     "QDriverStation/blob/master/CHANGES.md"));
}

void MainWindow::onWebsiteClicked() {
    QDesktopServices::openUrl (QUrl ("http://qdriverstation.sf.net"));
}

void MainWindow::onEnabledClicked() {
    /* Flash status label if we cannot enable the robot */
    if (!m_ds->canBeEnabled()) {
        onDisabledClicked();
        statusLabelAnimation();
        return;
    }

    /* Enable robot and update application style */
    setRobotEnabled (true);
    ui->EnableButton->setChecked (true);
    ui->EnableButton->setStyleSheet  (d_EnabledCheck);
    ui->DisableButton->setStyleSheet (d_DisabledUncheck);
}

void MainWindow::onDisabledClicked() {
    /* Only disable the robot if it can be disabled */
    if (!m_ds->isDisabled() && !m_ds->isEmergencyStop() && m_ds->isConnected())
        setRobotEnabled (false);

    /* Update the application style */
    ui->DisableButton->setChecked (true);
    ui->DisableButton->setStyleSheet (d_DisabledCheck);
    ui->EnableButton->setStyleSheet  (d_EnabledUncheck);
}

void MainWindow::onJoystickRemoved() {
    if (m_ds->isTeleoperated())
        onDisabledClicked();
}

void MainWindow::updateJoysticksTab (bool available) {
    m_joysticksWidget->setVisible (available);
    ui->NoJoystickWidget->setVisible (!available);
}

void MainWindow::onWindowModeChanged() {
    setWindowMode (ui->WindowDocked->isChecked() ? kDocked : kNormal);
}

void MainWindow::onRobotModeChanged (int mode) {
    Q_UNUSED (mode);
    m_ds->startDisabled();
}

void MainWindow::onPracticeValuesChanged() {
    Settings::set ("Practice Delay",      ui->PracticeDelay->value());
    Settings::set ("Practice TeleOp",     ui->PracticeTeleOp->value());
    Settings::set ("Practice End Game",   ui->PracticeEndGame->value());
    Settings::set ("Practice Countdown",  ui->PracticeCountdown->value());
    Settings::set ("Practice Autonomous", ui->PracticeAutonomous->value());
}

void MainWindow::setDashboard (int dashboard) {
    Settings::set ("Dashboard", dashboard);
    Dashboard::getInstance()->reloadDashboard();
}

void MainWindow::setTeamNumber (int team) {
    Settings::set ("Team ID", team);
    ui->TeamNumberSpin->setValue (team);
    ui->TeamNumber->setText (QString ("%1").arg (team));

    m_ds->setTeamNumber (team);
    m_advancedSettings->updatePlaceholder();
}

void MainWindow::setRobotEnabled (bool enabled) {
    if (enabled) {
        if (ui->Test->isChecked())
            m_ds->startTest();

        else if (ui->TeleOp->isChecked())
            m_ds->startTeleoperated();

        else if (ui->Autonomous->isChecked())
            m_ds->startAutonomous();

        else if (ui->Practice->isChecked())
            m_ds->startPractice (ui->PracticeCountdown->value(),
                                 ui->PracticeAutonomous->value(),
                                 ui->PracticeDelay->value(),
                                 ui->PracticeTeleOp->value(),
                                 ui->PracticeEndGame->value());
    }

    else m_ds->startDisabled();
}

//------------------------------------------------------------------------------
// DRIVER STATION TO WINDOW FUNCTIONS
//------------------------------------------------------------------------------

void MainWindow::updateLabelText (QLabel* label, QString text) {
    label->setText (m_ds->isConnected() && !text.isEmpty() ? text : "--.--");
}

void MainWindow::onCodeChanged (bool available) {
    ui->RobotCode->setChecked (available);
}

void MainWindow::onCommunicationsChanged (DS_CommunicationStatus status) {
    ui->RobotCheck->setChecked (status == kFull);
    ui->Communications->setChecked (status != kFailing);

    /* Decide if we should use a green LED or a yellow LED */
    switch (status) {
    case kPartial:
        ui->Communications->setStyleSheet (d_PartialStatusLED);
        ui->Communications->setToolTip (tr ("Partial communication with robot"));
        break;
    case kFull:
        ui->Communications->setStyleSheet (d_NormalStatusLED);
        ui->Communications->setToolTip (tr ("Full communication with robot"));
        break;
    case kFailing:
        ui->Communications->setToolTip (tr ("No communication with robot"));
        break;
    }

    onDisabledClicked();
}

void MainWindow::onControlModeChanged (DS_ControlMode mode) {
    switch (mode) {
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

void MainWindow::onRadioChanged (bool available) {
    ui->DsRadioCheck->setChecked (available);
}

void MainWindow::onVoltageChanged (QString voltage) {
    updateLabelText (ui->VoltageLabel, tr ("%1 V").arg (voltage));
}

void MainWindow::onLibVersionChanged (QString version) {
    updateLabelText (ui->LibVersion, version);
}

void MainWindow::onRioVersionChanged (QString version) {
    updateLabelText (ui->RioVersion, version);
}

void MainWindow::onRobotStatusChanged (QString status) {
    if (m_ds->canBeEnabled() && !m_ds->isEmergencyStop()) {
        /* Get 'TeleOp Disabled' instead of 'Disabled' */
        if (m_ds->isDisabled() && m_ds->isConnected()) {
            QString mode;

            if (ui->Test->isChecked())
                mode = ui->Test->text();

            else if (ui->TeleOp->isChecked())
                mode = ui->TeleOp->text();

            else if (ui->Autonomous->isChecked())
                mode = ui->Autonomous->text();

            else if (ui->Practice->isChecked())
                mode = ui->Practice->text();

            ui->StatusLabel->setText (QString ("%1\nDisabled").arg (mode));
        }

        /* Append 'Enabled' to the current operation mode */
        else if (!m_ds->isEmergencyStop())
            ui->StatusLabel->setText (tr ("%1\nEnabled").arg (status));
    }

    else
        ui->StatusLabel->setText (status);
}

void MainWindow::onRamUsageChanged (quint8 percent) {
    updateLabelText (ui->RamUsage, tr ("%1 %").arg (percent));
}

void MainWindow::onDiskUsageChanged (quint8 percent) {
    updateLabelText (ui->DiskUsage, tr ("%1 %").arg (percent));
}

//------------------------------------------------------------------------------
// MISC. FUNCTIONS
//------------------------------------------------------------------------------

void MainWindow::scrollNetConsole() {
    ui->NetConsoleEdit->ensureCursorVisible();
}

void MainWindow::toggleStatusColor() {
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

void MainWindow::statusLabelAnimation() {
    for (int i = 0; i < 8; ++i)
        QTimer::singleShot (100 * i, Qt::PreciseTimer,
                            this, SLOT (toggleStatusColor()));
}
