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

#pragma once
#ifndef _QDRIVER_STATION_MAINWINDOW_H
#define _QDRIVER_STATION_MAINWINDOW_H

#include <QProxyStyle>
#include <QStyleFactory>

#include <SmartWindow.h>
#include <ui_MainWindow.h>
#include <DriverStation.h>

class Updater;
class CustomTabStyle;
class JoysticksWidget;
class AdvancedSettings;

/**
 * @class MainWindow
 * @brief The user interface of the application
 *
 * The \c MainWindow provides a simple way for the end-user to control the
 * application and the robot without needing to worry about all the tiny,
 * pesky details that go under the hood of any modern application.
 */
class MainWindow : public SmartWindow
{
    Q_OBJECT

public:
    explicit MainWindow ();
    ~MainWindow ();

private:
    /**
     * The DriverStation library, used for actually controling the robot
     */
    DriverStation* m_ds;

    /**
     * Represents the UI of the dialog
     */
    Ui::MainWindow* ui;

    /**
     * Used for showing the icons (which are in fact text) horizontally
     * on the tab widgets, which use FontAwesome for the icons
     */
    CustomTabStyle* m_tabStyle;

    /**
     * Used for showing the current axis and button values of a seleted
     * joystick. We obtain joystick input with SDL
     */
    JoysticksWidget* m_joysticksWidget;

    /**
     * Configures the application style and allows the user to input a
     * custom network address for the robot (which is useful when
     * the robot does not want to cooperate)
     */
    AdvancedSettings* m_advancedSettings;

    /**
     * The auto updater object
     */
    Updater* m_updater;

private slots:
    /**
     * @internal
     * Configures the window so that it reacts to various events throughout the
     * application
     */
    void connectSlots ();

    /**
     * @internal
     * Ensures that the widgets are configured correctly and that their
     * appearance is updated according to the current state of the application
     */
    void configureWidgetAppearance ();

    /**
     * @internal
     * Reads settings to get the time frames to use when using the practice
     * feature of the DriverStation
     */
    void readSettings ();

    /**
     * @internal
     * Updates the colors of some labels which use a custom font or are under
     * the influence of a stylesheet
     */
    void redrawWidgets ();

    /**
     * @internal
     * Updates the values of the battery and CPU progress bars only if the
     * main tab is currently selected.
     */
    void updatePcStatusWidgets ();

    /**
     * @internal
     * Changes the robot alliance and position when the user changes the
     * alliance from the combobox
     */
    void onStationChanged (int station);

    /**
     * @internal
     * Changes the Driver Station protocol when the user changes the
     * protocol from the combobox
     */
    void onProtocolChanged (int protocol);

    /**
     * @internal
     * Copies the output of the NetConsole to the system clipboard
     */
    void onCopyClicked ();

    /**
     * @internal
     * Instructs the DriverStation library to reboot the robot
     */
    void onRebootClicked ();

    /**
     * @internal
     * Instructs the DriverStation library to restart the robot code
     */
    void onRestartClicked ();

    /**
     * @internal
     * Opens the Open DriverStation website in a web browser
     */
    void onWebsiteClicked ();

    /**
     * @internal
     * If the robot can be enabled, it changes its operation mode to the one
     * desired by the user and updates the appearance of the 'Enable'
     * and 'Disable' buttons.
     *
     * If the robot cannot be enabled, it calls a function that creates a
     * 'flashing' effect in the status label to get user attention.
     */
    void onEnabledClicked ();

    /**
     * @internal
     * Disables the robot and updates the appearance of the 'Enable' and
     * 'Disable' buttons
     */
    void onDisabledClicked ();

    /**
     * @internal
     * Called when SDL detects a joystick removal. If the DriverStation reports
     * that the robot is currently in Operator Control (TeleOp), it disables
     * the robot for safety reasons.
     */
    void onJoystickRemoved ();

    /**
     * @internal
     * Shows or hides the "No Joysticks Found" widget when the joystick status
     * changes
     */
    void updateJoysticksTab (bool available);

    /**
     * @internal
     * Changes the display mode of the window based on which button is checked
     */
    void onWindowModeChanged ();

    /**
     * @internal
     * Disables the robot when the user changes the robot operation mode
     */
    void onRobotModeChanged (int mode);

    /**
     * @internal
     * Saves the settings when a value of the practice times is changed
     */
    void onPracticeValuesChanged ();

    /**
     * @internal
     * Quits the running Dashboard and opens the selected Dashboard from the
     * 'Dashboard Type' combo. The \c db parameter must be equal to one of the
     * Dashboard types found in the 'Dashboards' enum in Dashboard.h
     *
     * @param db
     */
    void setDashboard (int dashboard);

    /**
     * @internal
     * Changes the team number used by the DriverStation and updates the UI
     */
    void setTeamNumber (int team);

    /**
     * @internal
     * Enables or disables the robot based on the value of \a enabled.
     * When \a enables is set to \c true, the function will change the operation
     * mode of the robot to match the operation mode desired by the user.
     *
     * @param enabled
     */
    void setRobotEnabled (bool enabled);

    /**
     * @internal
     * Changes the text of the specified label if we have a working connection
     * with the robot.
     *
     * If the robot communication fails, the function will change the text of
     * the label to '--.--'
     */
    void updateLabelText (QLabel* label, QString text);

    /**
     * @internal
     * Changes the appearance of the 'Robot Code' LED based on the value of the
     * \a available parameter
     */
    void onCodeChanged (bool available);

    /**
     * @internal
     * Changes the appearance of the 'Communications' LED based on the value of
     * the \a available parameter
     */
    void onCommunicationsChanged (bool available);

    /**
     * @internal
     * Changes the appearance of the window controls when the DriverStation
     * detects that the control mode was not changed by the user (e.g. when
     * the robot is disconnected or powered off)
     */
    void onControlModeChanged (DS_ControlMode mode);

    /**
     * @internal
     * Changes the checked state of the radio checkbox in the diagnostics tab
     * based on the value of the \a available parameter
     */
    void onRadioChanged (bool available);

    /**
     * @internal
     * Updates the text of the voltage label based on the \a voltage parameter
     *
     * @note The text will be set to '--.--' if there are are no communications
     */
    void onVoltageChanged (QString voltage);

    /**
     * @internal
     * Updates the text of the Library version label based on the \a version
     * parameter.
     *
     * @note The text will be set to '--.--' if there are are no communications
     */
    void onLibVersionChanged (QString version);

    /**
     * @internal
     * Updates the text of the roboRIO version label based on the \a version
     * parameter.
     *
     * @note The text will be set to '--.--' if there are are no communications
     */
    void onRioVersionChanged (QString version);

    /**
     * @internal
     * Updates the text of the status label based on the operation mode of the
     * robot. The function is pretty darn smart and can decide whenever to
     * append 'Enabled' or 'Disabled' depending on the operation mode and status
     * of the robot.
     */
    void onRobotStatusChanged (QString status);

    /**
     * @internal
     * Updates the value of the RAM usage label based on the \a total and
     * \a used parameters.
     *
     * @note The text will be set to '--.--' if there are are no communications
     */
    void onRamUsageChanged (int percent);

    /**
     * @internal
     * Updates the value of the disk usage label based on the \a total and
     * \a used parameters.
     *
     * @note The text will be set to '--.--' if there are are no communications
     */
    void onDiskUsageChanged (int percent);

    /**
     * @internal
     * Scrolls the NetConsole widget automatically when a message is added
     */
    void scrollNetConsole ();

    /**
     * @internal
     * Toggles the color of the status label between white and red.
     * Used for displaying an animation that draws the attention of the user
     * to the status label.
     */
    void toggleStatusColor ();

    /**
     * @internal
     * Switches the color of the status label between white a red repeatedly for
     * a limited amount of time. Used to draw the attention of the user when
     * there is a problem with the robot and it cannot be enabled.
     */
    void statusLabelAnimation ();

protected:
    /**
     * Reacts when the user presses a key
     */
    void keyPressEvent (QKeyEvent* e);

    /**
     * Reacts when the user releases a key
     */
    void keyReleaseEvent (QKeyEvent* e);
};

/**
 * Quick and dirty hack for having horizontal text on a vertical tab bar
 * Stolen from: http://www.qtcentre.org/threads/13293-QTabWidget-customization
 */
class CustomTabStyle : public QProxyStyle
{
public:
    explicit CustomTabStyle ()
    {
        setBaseStyle (QStyleFactory::create ("Fusion"));
    }

    QSize sizeFromContents (ContentsType type, const QStyleOption* option,
                            const QSize& size, const QWidget* widget) const
    {
        QSize s = QProxyStyle::sizeFromContents (type, option, size, widget);

        if (type == QStyle::CT_TabBarTab) {
            s.transpose ();
            s.setHeight (s.width ());
        }

        return s;
    }

    void drawControl (ControlElement element,
                      const QStyleOption* option,
                      QPainter* painter,
                      const QWidget* widget) const
    {
        if (element == CE_TabBarTabLabel) {
            if (const QStyleOptionTab* tab =
                    qstyleoption_cast <const QStyleOptionTab*> (option)) {
                QStyleOptionTab opt (*tab);
                opt.shape = QTabBar::RoundedNorth;
                QProxyStyle::drawControl (element, &opt, painter, widget);
                return;
            }
        }

        QProxyStyle::drawControl (element, option, painter, widget);
    }
};

#endif
