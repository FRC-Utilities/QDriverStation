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

#include <QList>
#include <QColor>
#include <QPalette>
#include <QPointer>
#include <QClipboard>
#include <QApplication>
#include <QDesktopWidget>
#include <QDesktopServices>

#include "Battery.h"
#include "Settings.h"
#include "CpuUsage.h"
#include "Dashboard.h"
#include "InitTasks.h"
#include "AssemblyInfo.h"
#include "JoysticksWidget.h"
#include "AdvancedSettings.h"

#include <LibDS/DS_Timers.h>

/*
 * Much code, very mess, wow
 * -------------------------
 * This file is so long because it must deal with very specific UI components,
 * if you want to understand  something, I recommend you to go to the header
 * file  and CTLR + Click a function/method that you are interested in.
 */

#define CSS_ENABLED_CHECK    "color: rgb(33, 255, 43);"
#define CSS_DISABLED_CHECK   "color: rgb(255, 33, 43); border-left: 0px;"
#define CSS_ENABLED_UNCHECK  "color: rgb(0, 43, 0);"
#define CSS_DISABLED_UNCHECK "color: rgb(43, 0, 0); border-left: 0px;"

/*
 * The font used in the NetConsole text edit
 */
#if defined __WIN32 || defined __WIN64
#  define FONT_NETCONSOLE QFont ("Consolas", 10)
#else
#  define FONT_NETCONSOLE QFont ("Inconsolata", 13)
#endif

/*
 * The horizontal size we give for the information widget, which displays
 * the current robot status, voltage, etc...
 */
#if defined __APPLE__
#  define INFO_SIZE 2.0
#else
#  define INFO_SIZE 1.7
#endif

MainWindow::MainWindow() {
    /* Initialize members */
    ui = new Ui::MainWindow;
    ui->setupUi (this);
    m_tabStyle = new CustomTabStyle;
    m_advancedSettings = new AdvancedSettings();
    m_joysticksWidget = new JoysticksWidget (ui->JoysticksTab);

    /* Configure application appearance & behavior */
    connectSlots();
    configureWidgetAppearance();

    /* Initial tasks */
    setTeamNumber (InitTasks::getTeamNumber());
    InitTasks::executeFirstRunTasks();

    /* Initialize sub-modules */
    DriverStation::getInstance()->init();
    GamepadManager::getInstance()->init();

    /* Read settings & show window */
    readSettings();
    ui->WindowDocked->setChecked (Settings::get ("Docked", false).toBool());
    setWindowMode (ui->WindowDocked->isChecked() ? Docked : Normal);
}

MainWindow::~MainWindow() {
    delete ui;
    delete m_tabStyle;
    delete m_joysticksWidget;
    delete m_advancedSettings;
}

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

    /* DriverStation to MainWindow */
    m_ds = DriverStation::getInstance();
    connect (m_ds, SIGNAL (codeChanged (bool)),
             this, SLOT   (onCodeChanged (bool)));
    connect (m_ds, SIGNAL (communicationsChanged (bool)),
             this, SLOT   (onCommunicationsChanged (bool)));
    connect (m_ds, SIGNAL (radioChanged (bool)),
             this, SLOT   (onRadioChanged (bool)));
    connect (m_ds, SIGNAL (voltageChanged (QString)),
             this, SLOT   (onVoltageChanged (QString)));
    connect (m_ds, SIGNAL (libVersionChanged (QString)),
             this, SLOT   (onLibVersionChanged (QString)));
    connect (m_ds, SIGNAL (rioVersionChanged (QString)),
             this, SLOT   (onRioVersionChanged (QString)));
    connect (m_ds, SIGNAL (robotStatusChanged (QString)),
             this, SLOT   (onRobotStatusChanged (QString)));
    connect (m_ds, SIGNAL (ramUsageChanged (int, int)),
             this, SLOT   (onRamUsageChanged (int, int)));
    connect (m_ds, SIGNAL (diskUsageChanged (int, int)),
             this, SLOT   (onDiskUsageChanged (int, int)));
    connect (m_ds, SIGNAL (controlModeChanged (DS_ControlMode)),
             this, SLOT   (onControlModeChanged (DS_ControlMode)));
    connect (DS_Timers::getInstance(), SIGNAL (timeout500()),
             this,                     SLOT   (updatePcStatusWidgets()));

    /* DriverStation to MainWindow UI */
    connect (m_ds,               SIGNAL (newNetConsoleMessage (QString)),
             ui->NetConsoleEdit, SLOT   (append               (QString)));
    connect (m_ds,               SIGNAL (elapsedTimeChanged   (QString)),
             ui->ElapsedTime,    SLOT   (setText              (QString)));

    /* MainWindow UI to Driver Station */
    connect (ui->ChartButton,    SIGNAL (clicked()),
             m_ds,               SLOT   (showLogWindow()));
    connect (ui->NetConsoleEdit, SIGNAL (textChanged()),
             this,               SLOT   (scrollNetConsole()));

    /* UI to window logic */
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

    /* Advanced settings window */
    connect (ui->SettingsButton, SIGNAL (clicked()),
             m_advancedSettings, SLOT   (show()));
    connect (m_advancedSettings, SIGNAL (updateColors()),
             this,               SLOT   (updateLabelColors()));
}

void MainWindow::configureWidgetAppearance() {
    ui->TeleOp->setChecked (true);
    ui->DisableButton->setChecked (true);

    /* Rotate the text (or icons) in the tabs */
    ui->LeftTab->tabBar()->setStyle (m_tabStyle);
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
    largeIconFont.setPointSize  (centralWidget()->font().pointSize() * 4.0);
    ui->PlugIcon->setFont       (smallIconFont);
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
    ui->AppName->setFont      (boldFont);
    ui->TeamLabel->setFont    (boldFont);
    ui->TeamNumber->setFont   (boldFont);
    ui->StatusLabel->setFont  (boldFont);
    ui->ElapsedTime->setFont  (boldFont);
    ui->VoltageLabel->setFont (boldFont);

    /* Change the palette of the items with a custom font to match app colors */
    updateLabelColors();

    /* Configure the 'About' tab */
    ui->AppName->setText (AssemblyInfo::name());
    ui->DsVersion->setText (AssemblyInfo::version());
    ui->AppVersion->setText (QString ("%1 (%2)")
                             .arg (AssemblyInfo::version())
                             .arg (AssemblyInfo::buildDateTime()));

    /* Set appearance of the 'Enable' and 'Disable' buttons */
    ui->DisableButton->setStyleSheet (CSS_DISABLED_CHECK);
    ui->EnableButton->setStyleSheet  (CSS_ENABLED_UNCHECK);

    if (ui->DisableButton->width() > ui->EnableButton->width())
        ui->EnableButton->resize (ui->DisableButton->width(),
                                  ui->EnableButton->height());
    else
        ui->DisableButton->resize (ui->EnableButton->width(),
                                   ui->DisableButton->height());

    /* Configure the NetConsole */
    ui->NetConsoleEdit->setFont (FONT_NETCONSOLE);
    connect (ui->ClearButton,    SIGNAL (clicked()),
             ui->NetConsoleEdit, SLOT   (clear()));
    connect (ui->CopyButton,     SIGNAL (clicked()),
             this,               SLOT   (onCopyClicked()));

    /* Configure the info frame */
    int height = ui->StatusLabel->height() * 1.2;
    ui->StatusLabel->setMinimumHeight (height);
    ui->StatusLabel->setMaximumHeight (height);
    ui->InfoFrame->setMinimumWidth    (ui->StatusLabel->width() * INFO_SIZE);
    ui->InfoFrame->setMaximumWidth    (ui->StatusLabel->width() * INFO_SIZE);

    /* Populate list-related widgets */
    ui->StationCombo->clear();
    ui->ProtocolCombo->clear();
    ui->StationCombo->addItems (m_ds->alliances());
    ui->ProtocolCombo->addItems (m_ds->protocols());
    ui->DbCombo->addItems (Dashboard::getInstance()->getAvailableDashboards());
    ui->DbCombo->setCurrentIndex (Dashboard::getInstance()->getCurrentDashboard());
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
    int station  = Settings::get ("Station",  DriverStation::Red1).toInt();
    int protocol = Settings::get ("Protocol", DriverStation::Protocol2015).toInt();

    /* Apply saved settings without crashing app if something goes wrong */
    if (ui->StationCombo->count() > station)
        ui->StationCombo->setCurrentIndex (station);
    if (ui->ProtocolCombo->count() > protocol)
        ui->ProtocolCombo->setCurrentIndex (protocol);

    /* Update internal values */
    onStationChanged  (ui->StationCombo->currentIndex());
    onProtocolChanged (ui->ProtocolCombo->currentIndex());

    /* Update next values automatically */
    connect (ui->StationCombo,       SIGNAL (currentIndexChanged (int)),
             this,                   SLOT   (onStationChanged    (int)));
    connect (ui->ProtocolCombo,      SIGNAL (currentIndexChanged (int)),
             this,                   SLOT   (onProtocolChanged   (int)));
    connect (ui->DbCombo,            SIGNAL (currentIndexChanged (int)),
             this,                   SLOT   (setDashboard        (int)));
}

void MainWindow::updateLabelColors() {
    QPalette p;
    ui->Test->setPalette                (p);
    ui->TeleOp->setPalette              (p);
    ui->AppName->setPalette             (p);
    ui->Practice->setPalette            (p);
    ui->PlugIcon->setPalette            (p);
    ui->CodeLabel->setPalette           (p);
    ui->TeamLabel->setPalette           (p);
    ui->Autonomous->setPalette          (p);
    ui->TeamNumber->setPalette          (p);
    ui->StatusLabel->setPalette         (p);
    ui->ElapsedTime->setPalette         (p);
    ui->VoltageIcon->setPalette         (p);
    ui->WindowNormal->setPalette        (p);
    ui->WindowDocked->setPalette        (p);
    ui->VoltageLabel->setPalette        (p);
    ui->JoysticksLabel->setPalette      (p);
    ui->CommunicationsLabel->setPalette (p);
}

void MainWindow::updatePcStatusWidgets() {
    if (ui->LeftTab->currentIndex() == 0) {
        int usage = CpuUsage::getUsage();
        int level = Battery::currentLevel();

        if (usage < 0 || usage > 100)
            usage = 0;

        if (level < 0 || level > 100)
            level = 0;

        ui->PcCpuProgress->setValue (usage);
        ui->PcBatteryProgress->setValue (level);
        ui->PlugIcon->setVisible (Battery::isPlugged());
        ui->PcCpuProgress->setToolTip (tr ("CPU usage: %1%").arg (usage));
        ui->PcBatteryProgress->setToolTip (tr ("Battery level: %1%").arg (level));
    }
}

void MainWindow::onCopyClicked() {
    qApp->clipboard()->setText (ui->NetConsoleEdit->toPlainText());
    ui->NetConsoleEdit->append ("<font color=\"#aaa\"><p>"
                                "INFO: NetConsole output copied to clipboard"
                                "</p></font>");
}

void MainWindow::onStationChanged (int station) {
    Settings::set ("Station", station);
    m_ds->setAlliance ((DriverStation::AllianceType) station);
}

void MainWindow::onProtocolChanged (int protocol) {
    m_ds->setProtocol ((DriverStation::ProtocolType) protocol);
    m_ds->setTeamNumber (ui->TeamNumberSpin->value());

    Settings::set ("Protocol", protocol);
    m_advancedSettings->updatePlaceholder();
}

void MainWindow::onRebootClicked() {
    if (!m_ds->networkAvailable())
        statusLabelAnimation();

    else
        m_ds->reboot();
}

void MainWindow::onRestartClicked() {
    if (!m_ds->canBeEnabled())
        statusLabelAnimation();

    else
        m_ds->restartCode();
}

void MainWindow::onWebsiteClicked() {
    QDesktopServices::openUrl (QUrl ("http://github.com/WinT-3794/QDriverStation"));
}

void MainWindow::onEnabledClicked() {
    /* Flash status label if we cannot enable the robot */
    if (!m_ds->canBeEnabled()) {
        onDisabledClicked();
        statusLabelAnimation();
        return;
    }

    setRobotEnabled (true);
    ui->EnableButton->setChecked (true);
    ui->EnableButton->setStyleSheet (CSS_ENABLED_CHECK);
    ui->DisableButton->setStyleSheet (CSS_DISABLED_UNCHECK);
}

void MainWindow::onDisabledClicked() {
    if (m_ds->controlMode() != DS_ControlDisabled)
        setRobotEnabled (false);

    ui->DisableButton->setChecked (true);
    ui->DisableButton->setStyleSheet (CSS_DISABLED_CHECK);
    ui->EnableButton->setStyleSheet (CSS_ENABLED_UNCHECK);
}

void MainWindow::onJoystickRemoved() {
    if (m_ds->controlMode() == DS_ControlTeleOp)
        onDisabledClicked();
}

void MainWindow::updateJoysticksTab (bool available) {
    m_joysticksWidget->setVisible (available);
    ui->NoJoystickWidget->setVisible (!available);
}

void MainWindow::onWindowModeChanged() {
    if (ui->WindowDocked->isChecked())
        setWindowMode (WindowMode::Docked);

    else
        setWindowMode (WindowMode::Normal);
}

void MainWindow::onRobotModeChanged (int mode) {
    Q_UNUSED (mode);
    m_ds->setControlMode (DS_ControlDisabled);
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
            m_ds->setControlMode (DS_ControlTest);

        else if (ui->TeleOp->isChecked())
            m_ds->setControlMode (DS_ControlTeleOp);

        else if (ui->Autonomous->isChecked())
            m_ds->setControlMode (DS_ControlAutonomous);

        else if (ui->Practice->isChecked())
            m_ds->startPractice (ui->PracticeCountdown->value(),
                                 ui->PracticeAutonomous->value(),
                                 ui->PracticeDelay->value(),
                                 ui->PracticeTeleOp->value(),
                                 ui->PracticeEndGame->value());
    }

    else
        m_ds->setControlMode (DS_ControlDisabled);
}

void MainWindow::updateLabelText (QLabel* label, QString text) {
    if (m_ds->networkAvailable() && !text.isEmpty())
        label->setText (text);

    else
        label->setText ("--.--");
}

void MainWindow::onCodeChanged (bool available) {
    ui->RobotCode->setChecked (available);
}

void MainWindow::onCommunicationsChanged (bool available) {
    ui->RobotCheck->setChecked (available);
    ui->Communications->setChecked (available);
}

void MainWindow::onControlModeChanged (DS_ControlMode mode) {
    if (mode == DS_ControlDisabled)
        ui->DisableButton->click();
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
    if (m_ds->canBeEnabled() && m_ds->controlMode() != DS_ControlEmergencyStop) {

        /* Get 'TeleOp Disabled' instead of 'Disabled' */
        if (m_ds->controlMode() == DS_ControlDisabled) {
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
        else
            ui->StatusLabel->setText (tr ("%1\nEnabled").arg (status));
    }

    else
        ui->StatusLabel->setText (status);
}

void MainWindow::onRamUsageChanged (int total, int used) {
    updateLabelText (ui->RamUsage, tr ("%1 MB / %2 MB").arg (used, total));
}

void MainWindow::onDiskUsageChanged (int total, int used) {
    updateLabelText (ui->DiskUsage, tr ("%1 MB / %2 MB").arg (used, total));
}

void MainWindow::scrollNetConsole() {
    ui->NetConsoleEdit->ensureCursorVisible();
}

void MainWindow::toggleStatusColor() {
    QPalette palette;
    QColor redColor = QColor (255, 33, 43);

    if (ui->StatusLabel->palette().windowText().color() != redColor)
        palette.setColor (QPalette::WindowText, redColor);

    ui->StatusLabel->setPalette (palette);

    if (!ui->Communications->isChecked())
        ui->CommunicationsLabel->setPalette (palette);

    if (!ui->RobotCode->isChecked())
        ui->CodeLabel->setPalette (palette);
}

void MainWindow::statusLabelAnimation() {
    for (int i = 0; i < 8; ++i)
        QTimer::singleShot (100 * i, Qt::PreciseTimer,
                            this, SLOT (toggleStatusColor()));
}
