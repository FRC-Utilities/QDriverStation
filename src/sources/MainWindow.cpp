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
#include <QTimer>
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

#define _ENABLED_SELECTED "color: rgb(33, 255, 43);"
#define _ENABLED_NOT_SELECTED "color: rgb(0, 43, 0);"
#define _DISABLED_SELECTED "color: rgb(255, 33, 43); border-left: 0px;"
#define _DISABLED_NOT_SELECTED "color: rgb(43, 0, 0); border-left: 0px;"

#if defined __WIN32 || defined __WIN64
#define _NETCONSOLE_FONT QFont ("Consolas", 10)
#else
#define _NETCONSOLE_FONT QFont ("Inconsolata", 13)
#endif

MainWindow::MainWindow() {
    m_ui = new Ui::MainWindow;
    m_ui->setupUi (this);

    setUseFixedSize (true);
    setPromptOnQuit (true);

    connectSlots();
    configureWidgetAppearance();

    setTeamNumber (InitTasks::getTeamNumber());
    InitTasks::executeFirstRunTasks();

    DriverStation::getInstance()->init();
    GamepadManager::getInstance()->init();
}

MainWindow::~MainWindow() {
    delete m_ui;
    delete m_joysticksWidget;
    delete m_advancedSettings;
}

void MainWindow::connectSlots() {
    updatePcStatusWidgets();

    connect (m_ui->Website,           SIGNAL (clicked()),
             this,                    SLOT   (onWebsiteClicked()));
    connect (m_ui->EnableButton,      SIGNAL (clicked()),
             this,                    SLOT   (onEnabledClicked()));
    connect (m_ui->DisableButton,     SIGNAL (clicked()),
             this,                    SLOT   (onDisabledClicked()));
    connect (m_ui->RebootRioButton,   SIGNAL (clicked()),
             this,                    SLOT   (onRebootClicked()));
    connect (m_ui->WindowDocked,      SIGNAL (clicked()),
             this,                    SLOT   (onWindowModeChanged()));
    connect (m_ui->WindowNormal,      SIGNAL (clicked()),
             this,                    SLOT   (onWindowModeChanged()));
    connect (m_ui->RestartCodeButton, SIGNAL (clicked()),
             this,                    SLOT   (onRestartClicked()));
    connect (m_ui->RobotModeGroup,    SIGNAL (buttonClicked (int)),
             this,                    SLOT   (onRobotModeChanged (int)));
    connect (m_ui->TeamNumberSpin,    SIGNAL (valueChanged  (int)),
             this,                    SLOT   (setTeamNumber (int)));

    /* Joystick information tab */
    m_joysticksWidget = new JoysticksWidget (m_ui->JoysticksTab);
    m_ui->JoysticksTab->layout()->addWidget (m_joysticksWidget);
    updateJoysticksTab (false);
    connect (m_joysticksWidget, SIGNAL (joystickRemoved()),
             this,              SLOT   (onJoystickRemoved()));
    connect (m_joysticksWidget, SIGNAL (statusChanged (bool)),
             this,              SLOT   (updateJoysticksTab (bool)));
    connect (m_joysticksWidget, SIGNAL (statusChanged (bool)),
             m_ui->Joysticks,   SLOT   (setChecked (bool)));

    /* DriverStation to MainWindow */
    m_ds = DriverStation::getInstance();
    m_ui->StationCombo->addItems (m_ds->alliances());
    connect (m_ds, SIGNAL (codeChanged (bool)),
             this, SLOT   (onCodeChanged (bool)));
    connect (m_ds, SIGNAL (networkChanged (bool)),
             this, SLOT   (onNetworkChanged (bool)));
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

    /* DriverStation to MainWindow UI */
    connect (m_ds,                 SIGNAL (newNetConsoleMessage (QString)),
             m_ui->NetConsoleEdit, SLOT   (append               (QString)));
    connect (m_ds,                 SIGNAL (elapsedTimeChanged   (QString)),
             m_ui->ElapsedTime,    SLOT   (setText              (QString)));

    /* MainWindow UI to Driver Station */
    connect (m_ui->ChartButton,    SIGNAL (clicked()),
             m_ds,                 SLOT   (showLogWindow()));
    connect (m_ui->NetConsoleEdit, SIGNAL (textChanged()),
             this,                 SLOT   (scrollNetConsole()));

    /* Dashboards */
    QPointer<Dashboard> dash = Dashboard::getInstance();
    m_ui->DbCombo->addItems (dash->getAvailableDashboards());
    m_ui->DbCombo->setCurrentIndex (dash->getCurrentDashboard());
    connect (m_ui->DbCombo, SIGNAL (currentIndexChanged (int)), this,
             SLOT (setDashboard (int)));

    /* Load and apply saved settings */
    readSettings();
    connect (m_ui->PracticeDelay,      SIGNAL (valueChanged (int)),
             this,                     SLOT   (onPracticeValuesChanged()));
    connect (m_ui->PracticeTeleOp,     SIGNAL (valueChanged (int)),
             this,                     SLOT   (onPracticeValuesChanged()));
    connect (m_ui->PracticeEndGame,    SIGNAL (valueChanged (int)),
             this,                     SLOT   (onPracticeValuesChanged()));
    connect (m_ui->PracticeCountdown,  SIGNAL (valueChanged (int)),
             this,                     SLOT   (onPracticeValuesChanged()));
    connect (m_ui->PracticeAutonomous, SIGNAL (valueChanged (int)),
             this,                     SLOT   (onPracticeValuesChanged()));
    connect (m_ui->StationCombo,       SIGNAL (currentIndexChanged (int)),
             this,                     SLOT   (onStationChanged    (int)));

    /* Advanced settings window */
    m_advancedSettings = new AdvancedSettings();
    connect (m_ui->AdvancedSettings, SIGNAL (clicked()),
             m_advancedSettings,     SLOT   (show()));
    connect (m_ui->SettingsButton,   SIGNAL (clicked()),
             m_advancedSettings,     SLOT   (show()));
    connect (m_advancedSettings,     SIGNAL (updateColors()),
             this,                   SLOT   (updateLabelColors()));
}

void MainWindow::configureWidgetAppearance() {
    m_ui->TeleOp->setChecked (true);
    m_ui->DisableButton->setChecked (true);
    m_ui->WindowDocked->setChecked (isDocked());

    /* Rotate the text (or icons) in the tabs */
    CustomTabStyle* style = new CustomTabStyle;
    m_ui->LeftTab->tabBar()->setStyle (style);
    m_ui->RightTab->tabBar()->setStyle (style);

    /* Set the icon size of the tabs and buttons */
    QFont fontAwesome;
    fontAwesome.setFamily ("FontAwesome");
    fontAwesome.setPointSize (centralWidget()->font().pointSize() * 1.4);
    m_ui->LeftTab->setFont (fontAwesome);
    m_ui->RightTab->setFont (fontAwesome);
    m_ui->CopyButton->setFont (fontAwesome);
    m_ui->ClearButton->setFont (fontAwesome);
    m_ui->UtilsWidget->setFont (fontAwesome);
    m_ui->WindowNormal->setFont (fontAwesome);
    m_ui->WindowDocked->setFont (fontAwesome);

    /* Set the voltage icon and plug icon fonts */
    QFont smallIconFont;
    QFont largeIconFont;
    smallIconFont.setFamily ("FontAwesome");
    largeIconFont.setFamily ("FontAwesome");
    smallIconFont.setPointSize (centralWidget()->font().pointSize() * 1.2);
    largeIconFont.setPointSize (centralWidget()->font().pointSize() * 4.0);
    m_ui->PlugIcon->setFont (smallIconFont);
    m_ui->CanIcon->setFont (largeIconFont);
    m_ui->VoltageIcon->setFont (largeIconFont);
    m_ui->NoJoystickIcon->setFont (largeIconFont);

    /* Restore the default application font to the widgets inside tabs */
    QFont defaultFont;
    defaultFont.setFamily (centralWidget()->font().family());
    defaultFont.setPointSize (centralWidget()->font().pointSize());
    m_ui->AboutTab->setFont (defaultFont);
    m_ui->PowerTab->setFont (defaultFont);
    m_ui->SetupTab->setFont (defaultFont);
    m_ui->MessagesTab->setFont (defaultFont);
    m_ui->JoysticksTab->setFont (defaultFont);
    m_ui->OperationTab->setFont (defaultFont);
    m_ui->DiagnosticsTab->setFont (defaultFont);

    /* Make some labels have a bold and bigger font */
    QFont boldFont;
    boldFont.setBold (true);
    boldFont.setPointSize (boldFont.pointSize() * 1.4);
    m_ui->AppName->setFont (boldFont);
    m_ui->TeamLabel->setFont (boldFont);
    m_ui->TeamNumber->setFont (boldFont);
    m_ui->StatusLabel->setFont (boldFont);
    m_ui->ElapsedTime->setFont (boldFont);
    m_ui->VoltageLabel->setFont (boldFont);

    /* Change the palette of the items with a custom font to match app colors */
    updateLabelColors();

    /* Configure the 'About' tab */
    m_ui->AppName->setText (AssemblyInfo::name());
    m_ui->DsVersion->setText (AssemblyInfo::version());
    m_ui->AppVersion->setText (QString ("%1 (%2)").arg (AssemblyInfo::version(),
                               AssemblyInfo::buildDateTime()));

    /* Set appearance of the 'Enable' and 'Disable' buttons */
    m_ui->DisableButton->setStyleSheet (_DISABLED_SELECTED);
    m_ui->EnableButton->setStyleSheet (_ENABLED_NOT_SELECTED);

    if (m_ui->DisableButton->width() > m_ui->EnableButton->width())
        m_ui->EnableButton->resize (m_ui->DisableButton->width(),
                                    m_ui->EnableButton->height());
    else
        m_ui->DisableButton->resize (m_ui->EnableButton->width(),
                                     m_ui->DisableButton->height());

    /* Configure the NetConsole */
    m_ui->NetConsoleEdit->setFont (_NETCONSOLE_FONT);
    connect (m_ui->ClearButton,            SIGNAL (clicked()),
             m_ui->NetConsoleEdit,         SLOT   (clear()));
    connect (m_ui->CopyButton,             SIGNAL (clicked()),
             this,                      SLOT   (onCopyClicked()));

    /* Configure the info frame */
    int height = m_ui->StatusLabel->height() * 1.2;
    m_ui->StatusLabel->setMinimumHeight (height);
    m_ui->StatusLabel->setMaximumHeight (height);
    m_ui->InfoFrame->setMinimumWidth (m_ui->StatusLabel->width() * 1.8);
    m_ui->InfoFrame->setMaximumWidth (m_ui->StatusLabel->width() * 1.8);
}

void MainWindow::readSettings() {
    /* Read practice values */
    int d = Settings::get ("Practice Delay", 1).toInt();
    int t = Settings::get ("Practice TeleOp", 100).toInt();
    int e = Settings::get ("Practice End Game", 20).toInt();
    int c = Settings::get ("Practice Countdown", 5).toInt();
    int a = Settings::get ("Practice Autonomous", 15).toInt();

    /* Apply practice values */
    m_ui->PracticeDelay->setValue (d);
    m_ui->PracticeTeleOp->setValue (t);
    m_ui->PracticeEndGame->setValue (e);
    m_ui->PracticeCountdown->setValue (c);
    m_ui->PracticeAutonomous->setValue (a);

    /* Read team station values */
    int station = Settings::get ("Station", 0).toInt();

    /* Apply saved team station without crashing app if something goes wrong */
    if (m_ui->StationCombo->count() > (station + 1))
        m_ui->StationCombo->setCurrentIndex (station);
    else
        m_ui->StationCombo->setCurrentIndex (0);
}

void MainWindow::updateLabelColors() {
    QPalette p;
    m_ui->Test->setPalette (p);
    m_ui->TeleOp->setPalette (p);
    m_ui->AppName->setPalette (p);
    m_ui->Practice->setPalette (p);
    m_ui->PlugIcon->setPalette (p);
    m_ui->CodeLabel->setPalette (p);
    m_ui->TeamLabel->setPalette (p);
    m_ui->Autonomous->setPalette (p);
    m_ui->TeamNumber->setPalette (p);
    m_ui->StatusLabel->setPalette (p);
    m_ui->ElapsedTime->setPalette (p);
    m_ui->VoltageIcon->setPalette (p);
    m_ui->WindowNormal->setPalette (p);
    m_ui->WindowDocked->setPalette (p);
    m_ui->VoltageLabel->setPalette (p);
    m_ui->JoysticksLabel->setPalette (p);
    m_ui->CommunicationsLabel->setPalette (p);
}

void MainWindow::updatePcStatusWidgets() {
    if (m_ui->LeftTab->currentIndex() == 0) {
        int usage = CpuUsage::getUsage();
        int level = Battery::currentLevel();

        if (usage < 0 || usage > 100)
            usage = 0;

        if (level < 0 || level > 100)
            level = 0;

        m_ui->PcCpuProgress->setValue (usage);
        m_ui->PcBatteryProgress->setValue (level);
        m_ui->PlugIcon->setVisible (Battery::isPlugged());
        m_ui->PcCpuProgress->setToolTip (tr ("CPU usage: %1%").arg (usage));
        m_ui->PcBatteryProgress->setToolTip (tr ("Battery level: %1%").arg (level));
    }

    QTimer::singleShot (1000, Qt::CoarseTimer,
                        this, SLOT (updatePcStatusWidgets()));
}

void MainWindow::onCopyClicked() {
    qApp->clipboard()->setText (m_ui->NetConsoleEdit->toPlainText());
    m_ui->NetConsoleEdit->append ("<font color=\"#aaa\"><p>"
                                  "INFO: NetConsole output copied to clipboard"
                                  "</p></font>");
}

void MainWindow::onStationChanged (int station) {
    Settings::set ("Station", station);
    m_ds->setAlliance ((DS_Alliance) station);
}

void MainWindow::onRebootClicked() {
    if (!m_network)
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
    m_ui->EnableButton->setChecked (true);
    m_ui->EnableButton->setStyleSheet (_ENABLED_SELECTED);
    m_ui->DisableButton->setStyleSheet (_DISABLED_NOT_SELECTED);
}

void MainWindow::onDisabledClicked() {
    if (m_ds->controlMode() != DS_ControlDisabled)
        setRobotEnabled (false);

    m_ui->DisableButton->setChecked (true);
    m_ui->DisableButton->setStyleSheet (_DISABLED_SELECTED);
    m_ui->EnableButton->setStyleSheet (_ENABLED_NOT_SELECTED);
}

void MainWindow::onJoystickRemoved() {
    if (m_ds->controlMode() == DS_ControlTeleOp)
        onDisabledClicked();
}

void MainWindow::updateJoysticksTab (bool available) {
    m_joysticksWidget->setVisible (available);
    m_ui->NoJoystickWidget->setVisible (!available);
}

void MainWindow::onWindowModeChanged() {
    if (m_ui->WindowDocked->isChecked())
        setWindowMode (WindowMode::Docked);

    else
        setWindowMode (WindowMode::Normal);
}

void MainWindow::onRobotModeChanged (int mode) {
    Q_UNUSED (mode);
    m_ds->setControlMode (DS_ControlDisabled);
}

void MainWindow::onPracticeValuesChanged() {
    Settings::set ("Practice Delay", m_ui->PracticeDelay->value());
    Settings::set ("Practice TeleOp", m_ui->PracticeTeleOp->value());
    Settings::set ("Practice End Game", m_ui->PracticeEndGame->value());
    Settings::set ("Practice Countdown", m_ui->PracticeCountdown->value());
    Settings::set ("Practice Autonomous", m_ui->PracticeAutonomous->value());
}

void MainWindow::setDashboard (int dashboard) {
    Settings::set ("Dashboard", dashboard);
    Dashboard::getInstance()->reloadDashboard();
}

void MainWindow::setTeamNumber (int team) {
    Settings::set ("Team ID", team);
    m_ui->TeamNumberSpin->setValue (team);
    m_ui->TeamNumber->setText (QString ("%1").arg (team));

    m_ds->setTeamNumber (team);
    m_advancedSettings->setTeamNumber (team);
}

void MainWindow::setRobotEnabled (bool enabled) {
    if (enabled) {
        if (m_ui->Test->isChecked())
            m_ds->setControlMode (DS_ControlTest);

        else if (m_ui->TeleOp->isChecked())
            m_ds->setControlMode (DS_ControlTeleOp);

        else if (m_ui->Autonomous->isChecked())
            m_ds->setControlMode (DS_ControlAutonomous);

        else if (m_ui->Practice->isChecked())
            m_ds->startPractice (m_ui->PracticeCountdown->value(),
                                 m_ui->PracticeAutonomous->value(),
                                 m_ui->PracticeDelay->value(),
                                 m_ui->PracticeTeleOp->value(),
                                 m_ui->PracticeEndGame->value());
    }

    else
        m_ds->setControlMode (DS_ControlDisabled);
}

void MainWindow::updateLabelText (QLabel* label, QString text) {
    if (m_network && !text.isEmpty())
        label->setText (text);

    else
        label->setText ("--.--");
}

void MainWindow::onCodeChanged (bool available) {
    m_ui->RobotCode->setChecked (available);
}

void MainWindow::onNetworkChanged (bool available) {
    m_network = available;
    m_ui->RobotCheck->setChecked (available);
    m_ui->Communications->setChecked (available);
}

void MainWindow::onControlModeChanged (DS_ControlMode mode) {
    if (mode == DS_ControlDisabled)
        m_ui->DisableButton->click();
}

void MainWindow::onRadioChanged (bool available) {
    m_ui->DsRadioCheck->setChecked (available);
}

void MainWindow::onVoltageChanged (QString voltage) {
    updateLabelText (m_ui->VoltageLabel, tr ("%1 V").arg (voltage));
}

void MainWindow::onLibVersionChanged (QString version) {
    updateLabelText (m_ui->LibVersion, version);
}

void MainWindow::onRioVersionChanged (QString version) {
    updateLabelText (m_ui->RioVersion, version);
}

void MainWindow::onRobotStatusChanged (QString status) {
    if (m_ds->canBeEnabled() && m_ds->controlMode() != DS_ControlEmergencyStop) {

        /* Get 'TeleOp Disabled' instead of 'Disabled' */
        if (m_ds->controlMode() == DS_ControlDisabled) {
            QString mode;

            if (m_ui->Test->isChecked())
                mode = m_ui->Test->text();

            else if (m_ui->TeleOp->isChecked())
                mode = m_ui->TeleOp->text();

            else if (m_ui->Autonomous->isChecked())
                mode = m_ui->Autonomous->text();

            else if (m_ui->Practice->isChecked())
                mode = m_ui->Practice->text();

            m_ui->StatusLabel->setText (QString ("%1\nDisabled").arg (mode));
        }

        /* Append 'Enabled' to the current operation mode */
        else
            m_ui->StatusLabel->setText (tr ("%1\nEnabled").arg (status));
    }

    else
        m_ui->StatusLabel->setText (status);
}

void MainWindow::onRamUsageChanged (int total, int used) {
    updateLabelText (m_ui->RamUsage, tr ("%1 MB / %2 MB").arg (used, total));
}

void MainWindow::onDiskUsageChanged (int total, int used) {
    updateLabelText (m_ui->DiskUsage, tr ("%1 MB / %2 MB").arg (used, total));
}

void MainWindow::scrollNetConsole() {
    m_ui->NetConsoleEdit->ensureCursorVisible();
}

void MainWindow::toggleStatusColor() {
    QPalette palette;
    QColor redColor = QColor (255, 33, 43);

    if (m_ui->StatusLabel->palette().windowText().color() != redColor)
        palette.setColor (QPalette::WindowText, redColor);

    m_ui->StatusLabel->setPalette (palette);

    if (!m_ui->Communications->isChecked())
        m_ui->CommunicationsLabel->setPalette (palette);

    if (!m_ui->RobotCode->isChecked())
        m_ui->CodeLabel->setPalette (palette);
}

void MainWindow::statusLabelAnimation() {
    for (int i = 0; i < 8; ++i)
        QTimer::singleShot (100 * i, Qt::PreciseTimer,
                            this, SLOT (toggleStatusColor()));
}
