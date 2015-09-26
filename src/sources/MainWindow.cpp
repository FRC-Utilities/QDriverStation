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

MainWindow::MainWindow()
{
    ui.setupUi (this);
    setUseFixedSize (true);
    setPromptOnQuit (true);

    connectSlots();
    configureWidgetAppearance();

    setTeamNumber (InitTasks::getTeamNumber());
    InitTasks::executeFirstRunTasks();

    DriverStation::getInstance()->init();
    GamepadManager::getInstance()->init();
}

MainWindow::~MainWindow()
{
    delete m_advancedSettings;
}

void MainWindow::connectSlots()
{
    updatePcStatusWidgets();

    connect (ui.Website,           SIGNAL (clicked()),
             this,                 SLOT   (onWebsiteClicked()));
    connect (ui.EnableButton,      SIGNAL (clicked()),
             this,                 SLOT   (onEnabledClicked()));
    connect (ui.DisableButton,     SIGNAL (clicked()),
             this,                 SLOT   (onDisabledClicked()));
    connect (ui.RebootRioButton,   SIGNAL (clicked()),
             this,                 SLOT   (onRebootClicked()));
    connect (ui.WindowDocked,      SIGNAL (clicked()),
             this,                 SLOT   (onWindowModeChanged()));
    connect (ui.WindowNormal,      SIGNAL (clicked()),
             this,                 SLOT   (onWindowModeChanged()));
    connect (ui.RestartCodeButton, SIGNAL (clicked()),
             this,                 SLOT   (onRestartClicked()));
    connect (ui.RobotModeGroup,    SIGNAL (buttonClicked (int)),
             this,                 SLOT   (onRobotModeChanged (int)));
    connect (ui.TeamNumberSpin,    SIGNAL (valueChanged  (int)),
             this,                 SLOT   (setTeamNumber (int)));

    /* Joystick information tab */
    QPointer<JoysticksWidget> j = new JoysticksWidget (ui.JoysticksTab);
    ui.JoysticksTab->layout()->addWidget (j);
    connect (j, SIGNAL (joystickRemoved()), this, SLOT (onJoystickRemoved()));
    connect (j, SIGNAL (statusChanged (bool)),
             ui.Joysticks, SLOT (setChecked (bool)));

    /* DriverStation */
    m_ds = DriverStation::getInstance();
    ui.StationCombo->addItems (m_ds->alliances());
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
    connect (m_ds, SIGNAL (elapsedTimeChanged (QString)),
             ui.ElapsedTime, SLOT (setText (QString)));
    connect (m_ds, SIGNAL (newMessage (QString)),
             ui.NetConsoleEdit, SLOT (append (QString)));
    connect (ui.NetConsoleEdit, SIGNAL (textChanged()),
             this,              SLOT   (scrollNetConsole()));

    /* Dashboards */
    QPointer<Dashboard> dash = Dashboard::getInstance();
    ui.DbCombo->addItems (dash->getAvailableDashboards());
    ui.DbCombo->setCurrentIndex (dash->getCurrentDashboard());
    connect (ui.DbCombo, SIGNAL (currentIndexChanged (int)), this,
             SLOT (setDashboard (int)));

    /* Load and apply saved settings */
    readSettings();
    connect (ui.PracticeDelay,      SIGNAL (valueChanged (int)),
             this,                  SLOT   (onPracticeValuesChanged()));
    connect (ui.PracticeTeleOp,     SIGNAL (valueChanged (int)),
             this,                  SLOT   (onPracticeValuesChanged()));
    connect (ui.PracticeEndGame,    SIGNAL (valueChanged (int)),
             this,                  SLOT   (onPracticeValuesChanged()));
    connect (ui.PracticeCountdown,  SIGNAL (valueChanged (int)),
             this,                  SLOT   (onPracticeValuesChanged()));
    connect (ui.PracticeAutonomous, SIGNAL (valueChanged (int)),
             this,                  SLOT   (onPracticeValuesChanged()));
    connect (ui.StationCombo,       SIGNAL (currentIndexChanged (int)),
             this,                  SLOT   (onStationChanged    (int)));

    /* Advanced settings window */
    m_advancedSettings = new AdvancedSettings();
    connect (ui.AdvancedSettings, SIGNAL (clicked()),
             m_advancedSettings,  SLOT   (show()));
    connect (ui.SettingsButton,   SIGNAL (clicked()),
             m_advancedSettings,  SLOT   (show()));
    connect (m_advancedSettings,  SIGNAL (updateColors()),
             this,                SLOT   (updateLabelColors()));
}

void MainWindow::configureWidgetAppearance()
{
    ui.TeleOp->setChecked (true);
    ui.DisableButton->setChecked (true);
    ui.CloseWidget->setVisible (isDocked());
    ui.WindowDocked->setChecked (isDocked());

    /* Make some labels have a bold and bigger font */
    QFont font;
    font.setBold (true);
    font.setPointSize (font.pointSize() * 1.4);
    ui.AppName->setFont (font);
    ui.TeamLabel->setFont (font);
    ui.TeamNumber->setFont (font);
    ui.StatusLabel->setFont (font);
    ui.ElapsedTime->setFont (font);
    ui.VoltageLabel->setFont (font);

    updateLabelColors();

    /* Configure the 'About' tab */
    ui.AppName->setText (AssemblyInfo::name());
    ui.DsVersion->setText (AssemblyInfo::version());
    ui.AppVersion->setText (QString ("%1 (%2)").arg (AssemblyInfo::version(),
                            AssemblyInfo::buildDateTime()));

    /* Set appearance of the 'Enable' and 'Disable' buttons */
    ui.DisableButton->setStyleSheet (_DISABLED_SELECTED);
    ui.EnableButton->setStyleSheet (_ENABLED_NOT_SELECTED);

    if (ui.DisableButton->width() > ui.EnableButton->width())
        ui.EnableButton->resize (ui.DisableButton->width(),
                                 ui.EnableButton->height());
    else
        ui.DisableButton->resize (ui.EnableButton->width(),
                                  ui.DisableButton->height());

    /* Configure the NetConsole */
    ui.NetConsoleEdit->setFont (_NETCONSOLE_FONT);
    connect (ui.ClearButton,            SIGNAL (clicked()),
             ui.NetConsoleEdit,         SLOT   (clear()));
    connect (ui.CopyButton,             SIGNAL (clicked()),
             this,                      SLOT   (onCopyClicked()));

    /* Configure the info frame */
    ui.InfoFrame->setMinimumWidth (ui.StatusLabel->width() * 1.8);
    ui.InfoFrame->setMaximumWidth (ui.StatusLabel->width() * 1.8);
}

void MainWindow::readSettings()
{
    /* Read practice values */
    int d = Settings::get ("Practice Delay", 1).toInt();
    int t = Settings::get ("Practice TeleOp", 100).toInt();
    int e = Settings::get ("Practice End Game", 20).toInt();
    int c = Settings::get ("Practice Countdown", 5).toInt();
    int a = Settings::get ("Practice Autonomous", 15).toInt();

    /* Apply practice values */
    ui.PracticeDelay->setValue (d);
    ui.PracticeTeleOp->setValue (t);
    ui.PracticeEndGame->setValue (e);
    ui.PracticeCountdown->setValue (c);
    ui.PracticeAutonomous->setValue (a);

    /* Read team station values */
    int station = Settings::get ("Station", 0).toInt();

    /* Apply saved team station without crashing app if something goes wrong */
    if (ui.StationCombo->count() > (station + 1))
        ui.StationCombo->setCurrentIndex (station);
    else
        ui.StationCombo->setCurrentIndex (0);
}

void MainWindow::updateLabelColors()
{
    QPalette p;
    ui.Test->setPalette (p);
    ui.TeleOp->setPalette (p);
    ui.AppName->setPalette (p);
    ui.Practice->setPalette (p);
    ui.CodeLabel->setPalette (p);
    ui.TeamLabel->setPalette (p);
    ui.Autonomous->setPalette (p);
    ui.TeamNumber->setPalette (p);
    ui.StatusLabel->setPalette (p);
    ui.ElapsedTime->setPalette (p);
    ui.VoltageLabel->setPalette (p);
    ui.JoysticksLabel->setPalette (p);
    ui.CommunicationsLabel->setPalette (p);
}

void MainWindow::updatePcStatusWidgets()
{
    if (ui.LeftTab->currentIndex() == 0) {
        int usage = CpuUsage::getUsage();
        int level = Battery::currentLevel();

        if (usage < 0 || usage > 100)
            usage = 0;

        if (level < 0 || level > 100)
            level = 0;

        ui.PcCpuProgress->setValue (usage);
        ui.PcBatteryProgress->setValue (level);
        ui.PlugIcon->setVisible (Battery::isPlugged());
        ui.PcCpuProgress->setToolTip (tr ("CPU usage: %1%").arg (usage));
        ui.PcBatteryProgress->setToolTip (tr ("Battery level: %1%").arg (level));
    }

    QTimer::singleShot (1000, Qt::CoarseTimer,
                        this, SLOT (updatePcStatusWidgets()));
}

void MainWindow::onCopyClicked()
{
    qApp->clipboard()->setText (ui.NetConsoleEdit->toPlainText());
    ui.NetConsoleEdit->append ("<font color=\"#aaa\"><p>"
                               "INFO: NetConsole ouput copied to clipboard"
                               "</p></font>");
}

void MainWindow::onStationChanged (const int& station)
{
    Settings::set ("Station", station);
    m_ds->setAlliance ((DS_Alliance) station);
}

void MainWindow::onRebootClicked()
{
    if (!m_network)
        statusLabelAnimation();

    else
        m_ds->reboot();
}

void MainWindow::onRestartClicked()
{
    if (!m_ds->canBeEnabled())
        statusLabelAnimation();

    else
        m_ds->restartCode();
}

void MainWindow::onWebsiteClicked()
{
    QDesktopServices::openUrl (QUrl ("http://github.com/WinT-3794/QDriverStation"));
}

void MainWindow::onEnabledClicked()
{
    /* Flash status label if we cannot enable the robot */
    if (!m_ds->canBeEnabled()) {
        onDisabledClicked();
        statusLabelAnimation();
        return;
    }

    setRobotEnabled (true);
    ui.EnableButton->setChecked (true);
    ui.EnableButton->setStyleSheet (_ENABLED_SELECTED);
    ui.DisableButton->setStyleSheet (_DISABLED_NOT_SELECTED);
}

void MainWindow::onDisabledClicked()
{
    if (m_ds->controlMode() != DS_ControlDisabled)
        setRobotEnabled (false);

    ui.DisableButton->setChecked (true);
    ui.DisableButton->setStyleSheet (_DISABLED_SELECTED);
    ui.EnableButton->setStyleSheet (_ENABLED_NOT_SELECTED);
}

void MainWindow::onJoystickRemoved()
{
    if (m_ds->operationMode() == DS_ControlTeleOp)
        onDisabledClicked();
}

void MainWindow::onWindowModeChanged()
{
    if (ui.WindowDocked->isChecked()) {
        ui.CloseWidget->setVisible (true);
        setWindowMode (WindowMode::Docked);
    }

    else {
        ui.CloseWidget->setVisible (false);
        setWindowMode (WindowMode::Normal);
    }
}

void MainWindow::onRobotModeChanged (const int& mode)
{
    Q_UNUSED (mode);
    m_ds->setControlMode (DS_ControlDisabled);
}

void MainWindow::onPracticeValuesChanged()
{
    Settings::set ("Practice Delay", ui.PracticeDelay->value());
    Settings::set ("Practice TeleOp", ui.PracticeTeleOp->value());
    Settings::set ("Practice End Game", ui.PracticeEndGame->value());
    Settings::set ("Practice Countdown", ui.PracticeCountdown->value());
    Settings::set ("Practice Autonomous", ui.PracticeAutonomous->value());
}

void MainWindow::setDashboard (const int& dashboard)
{
    Settings::set ("Dashboard", dashboard);
    Dashboard::getInstance()->reloadDashboard();
}

void MainWindow::setTeamNumber (const int& team)
{
    Settings::set ("Team ID", team);
    ui.TeamNumberSpin->setValue (team);
    ui.TeamNumber->setText (QString ("%1").arg (team));

    m_ds->setTeamNumber (team);
    m_advancedSettings->setTeamNumber (team);
}

void MainWindow::setRobotEnabled (const bool& enabled)
{
    if (enabled) {
        if (ui.Test->isChecked())
            m_ds->setControlMode (DS_ControlTest);

        else if (ui.TeleOp->isChecked())
            m_ds->setControlMode (DS_ControlTeleOp);

        else if (ui.Autonomous->isChecked())
            m_ds->setControlMode (DS_ControlAutonomous);

        else if (ui.Practice->isChecked())
            m_ds->startPractice (ui.PracticeCountdown->value(),
                                 ui.PracticeAutonomous->value(),
                                 ui.PracticeDelay->value(),
                                 ui.PracticeTeleOp->value(),
                                 ui.PracticeEndGame->value());
    }

    else
        m_ds->setControlMode (DS_ControlDisabled);
}

void MainWindow::updateLabelText (QLabel* label, QString text)
{
    if (m_network)
        label->setText (text);

    else
        label->setText ("--.--");
}

void MainWindow::onCodeChanged (const bool& available)
{
    ui.RobotCode->setChecked (available);
}

void MainWindow::onNetworkChanged (const bool& available)
{
    m_network = available;
    ui.RobotCheck->setChecked (available);
    ui.Communications->setChecked (available);
}

void MainWindow::onControlModeChanged (DS_ControlMode mode)
{
    if (mode == DS_ControlDisabled)
        ui.DisableButton->click();
}

void MainWindow::onRadioChanged (const bool& available)
{
    ui.DsRadioCheck->setChecked (available);
}

void MainWindow::onVoltageChanged (const QString& voltage)
{
    updateLabelText (ui.VoltageLabel, tr ("%1 V").arg (voltage));
}

void MainWindow::onLibVersionChanged (const QString& version)
{
    updateLabelText (ui.LibVersion, version);
}

void MainWindow::onRioVersionChanged (const QString& version)
{
    updateLabelText (ui.RioVersion, version);
}

void MainWindow::onRobotStatusChanged (const QString& status)
{
    if (m_ds->canBeEnabled() && m_ds->operationMode() != DS_ControlEmergencyStop) {

        /* Get 'TeleOp Disabled' instead of 'Disabled' */
        if (m_ds->operationMode() == DS_ControlDisabled) {
            QString mode;

            if (ui.Test->isChecked())
                mode = ui.Test->text();

            else if (ui.TeleOp->isChecked())
                mode = ui.TeleOp->text();

            else if (ui.Autonomous->isChecked())
                mode = ui.Autonomous->text();

            else if (ui.Practice->isChecked())
                mode = ui.Practice->text();

            ui.StatusLabel->setText (QString ("%1\nDisabled").arg (mode));
        }

        /* Append 'Enabled' to the current operation mode */
        else
            ui.StatusLabel->setText (tr ("%1\nEnabled").arg (status));
    }

    /* Robot is in E-Stop or something is not good... */
    else
        ui.StatusLabel->setText (status);
}

void MainWindow::onRamUsageChanged (const int& total, const int& used)
{
    updateLabelText (ui.RamUsage, tr ("%1 MB / %2 MB").arg (used, total));
}

void MainWindow::onDiskUsageChanged (const int& total, const int& used)
{
    updateLabelText (ui.DiskUsage, tr ("%1 MB / %2 MB").arg (used, total));
}

void MainWindow::scrollNetConsole()
{
    ui.NetConsoleEdit->ensureCursorVisible();
}

void MainWindow::toggleStatusColor()
{
    QPalette palette;
    QColor redColor = QColor (255, 33, 43);

    if (ui.StatusLabel->palette().windowText().color() != redColor)
        palette.setColor (QPalette::WindowText, redColor);

    ui.StatusLabel->setPalette (palette);
}

void MainWindow::statusLabelAnimation()
{
    for (int i = 0; i < 8; ++i)
        QTimer::singleShot (100 * i, Qt::PreciseTimer,
                            this, SLOT (toggleStatusColor()));
}
