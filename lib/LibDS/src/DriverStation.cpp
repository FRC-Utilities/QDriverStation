/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#include "DriverStation.h"

#include "Core/Sockets.h"
#include "Core/Protocol.h"
#include "Core/Watchdog.h"
#include "Core/DS_Config.h"
#include "Core/NetConsole.h"
#include "Core/RobotLogger.h"

#include "Protocols/FRC_2014.h"
#include "Protocols/FRC_2015.h"
#include "Protocols/FRC_2016.h"

#include <QUrl>
#include <QThread>
#include <QDesktopServices>

/**
 * Formats the input message so that it looks nice on a text display widget
 */
static QString CONSOLE_MESSAGE (const QString& input) {
    return "<font color='#888'>** " + input + "</font>";
}

/**
 * Initializes the members of the DriverStation class
 */
DriverStation::DriverStation() {
    qDebug() << "Initializing DriverStation...";

    /* Initialize the protocol, but do not allow DS to send packets */
    m_init = false;
    m_running = false;
    m_protocol = Q_NULLPTR;

    /* Initialzie misc. variables */
    m_packetLoss = 0;
    m_fmsInterval = 1000;
    m_radioInterval = 1000;
    m_robotInterval = 1000;

    /* Initialize DS modules & watchdogs */
    m_sockets = new Sockets;
    m_console = new NetConsole;
    m_fmsWatchdog = new Watchdog;
    m_radioWatchdog = new Watchdog;
    m_robotWatchdog = new Watchdog;

    /* Initialize threads */
    m_watchdogThread = new QThread (this);
    m_secondaryThread = new QThread (this);

    /* Move modules to a highest priority thread */
    m_sockets->moveToThread (m_secondaryThread);
    m_console->moveToThread (m_secondaryThread);
    m_secondaryThread->start (QThread::HighestPriority);

    /* Move watchdogs to a high priority thread */
    m_fmsWatchdog->moveToThread (m_watchdogThread);
    m_radioWatchdog->moveToThread (m_watchdogThread);
    m_robotWatchdog->moveToThread (m_watchdogThread);
    m_watchdogThread->start (QThread::HighPriority);

    /* React when the sockets receive data from FMS, radio or robot */
    connect (m_sockets, SIGNAL (fmsPacketReceived   (QByteArray)),
             this,        SLOT (readFMSPacket       (QByteArray)));
    connect (m_sockets, SIGNAL (radioPacketReceived (QByteArray)),
             this,        SLOT (readRadioPacket     (QByteArray)));
    connect (m_sockets, SIGNAL (robotPacketReceived (QByteArray)),
             this,        SLOT (readRobotPacket     (QByteArray)));

    /* Sync DS signals with DS_Config signals */
    connect (config(), SIGNAL (allianceChanged (Alliance)),
             this,     SIGNAL (allianceChanged (Alliance)));
    connect (config(), SIGNAL (codeStatusChanged (CodeStatus)),
             this,     SIGNAL (codeStatusChanged (CodeStatus)));
    connect (config(), SIGNAL (controlModeChanged (ControlMode)),
             this,     SIGNAL (controlModeChanged (ControlMode)));
    connect (config(), SIGNAL (cpuUsageChanged (int)),
             this,     SIGNAL (cpuUsageChanged (int)));
    connect (config(), SIGNAL (diskUsageChanged (int)),
             this,     SIGNAL (diskUsageChanged (int)));
    connect (config(), SIGNAL (elapsedTimeChanged (int)),
             this,     SIGNAL (elapsedTimeChanged (int)));
    connect (config(), SIGNAL (elapsedTimeChanged (QString)),
             this,     SIGNAL (elapsedTimeChanged (QString)));
    connect (config(), SIGNAL (enabledChanged (EnableStatus)),
             this,     SIGNAL (enabledChanged (EnableStatus)));
    connect (config(), SIGNAL (fmsCommStatusChanged (CommStatus)),
             this,     SIGNAL (fmsCommStatusChanged (CommStatus)));
    connect (config(), SIGNAL (libVersionChanged (QString)),
             this,     SIGNAL (libVersionChanged (QString)));
    connect (config(), SIGNAL (operationStatusChanged (OperationStatus)),
             this,     SIGNAL (operationStatusChanged (OperationStatus)));
    connect (config(), SIGNAL (pcmVersionChanged (QString)),
             this,     SIGNAL (pcmVersionChanged (QString)));
    connect (config(), SIGNAL (pdpVersionChanged (QString)),
             this,     SIGNAL (pdpVersionChanged (QString)));
    connect (config(), SIGNAL (positionChanged (Position)),
             this,     SIGNAL (positionChanged (Position)));
    connect (config(), SIGNAL (radioAddressChanged (QString)),
             this,     SIGNAL (radioAddressChanged (QString)));
    connect (config(), SIGNAL (radioCommStatusChanged (CommStatus)),
             this,     SIGNAL (radioCommStatusChanged (CommStatus)));
    connect (config(), SIGNAL (ramUsageChanged (int)),
             this,     SIGNAL (ramUsageChanged (int)));
    connect (config(), SIGNAL (robotAddressChanged (QString)),
             this,     SIGNAL (robotAddressChanged (QString)));
    connect (config(), SIGNAL (robotCommStatusChanged (CommStatus)),
             this,     SIGNAL (robotCommStatusChanged (CommStatus)));
    connect (config(), SIGNAL (simulatedChanged (bool)),
             this,     SIGNAL (simulatedChanged (bool)));
    connect (config(), SIGNAL (statusChanged (QString)),
             this,     SIGNAL (statusChanged (QString)));
    connect (config(), SIGNAL (teamChanged (int)),
             this,     SIGNAL (teamChanged (int)));
    connect (config(), SIGNAL (voltageChanged (float)),
             this,     SIGNAL (voltageChanged (float)));
    connect (config(), SIGNAL (voltageChanged (QString)),
             this,     SIGNAL (voltageChanged (QString)));
    connect (config(), SIGNAL (voltageStatusChanged (VoltageStatus)),
             this,     SIGNAL (voltageStatusChanged (VoltageStatus)));

    /* Set default watchdog expiration times */
    m_fmsWatchdog->setExpirationTime (1000);
    m_radioWatchdog->setExpirationTime (1000);
    m_robotWatchdog->setExpirationTime (1000);

    /* Reset the DS values when one of the watchdogs is not feed */
    connect (m_fmsWatchdog,   SIGNAL (expired()), this, SLOT (resetFMS()));
    connect (m_radioWatchdog, SIGNAL (expired()), this, SLOT (resetRadio()));
    connect (m_robotWatchdog, SIGNAL (expired()), this, SLOT (resetRobot()));

    /* Notify client when the NetConsole receives a new message */
    connect (m_console, SIGNAL (newMessage (QString)),
             this,      SIGNAL (newMessage (QString)));

    qDebug() << "DriverStation initialized!";
}

DriverStation::~DriverStation() {
    /* Stop threads */
    m_watchdogThread->exit();
    m_secondaryThread->exit();

    /* Delete modules */
    delete m_sockets;
    delete m_console;
    delete m_fmsWatchdog;
    delete m_radioWatchdog;
    delete m_robotWatchdog;
}

/**
 * One instance to rule them all!
 */
DriverStation* DriverStation::getInstance() {
    static DriverStation instance;
    return &instance;
}

/**
 * Returns \c true if we have communications with the robot controller
 * and the robot code is running.
 */
bool DriverStation::canBeEnabled() {
    return isConnectedToRobot() && isRobotCodeRunning() && !isEmergencyStopped();
}

/**
 * Returns \c true if the DS is currently sending and receiving data
 */
bool DriverStation::running() const {
    return m_running;
}

/**
 * Returns \c true if the robot is currently in test mode
 */
bool DriverStation::isInTest() const {
    return controlMode() == kControlTest;
}

/**
 * Returns \c true if the robot is currently enabled
 */
bool DriverStation::isEnabled() const {
    return enableStatus() == kEnabled;
}

/**
 * Returns \c true if the robot is a simulated robot
 */
bool DriverStation::isSimulated() const {
    return config()->isSimulated();
}

/**
 * Returns \c true if the robot is currently in autonomous mode
 */
bool DriverStation::isInAutonomous() const {
    return controlMode() == kControlAutonomous;
}

/**
 * Returns \c true if the robot is currently in teleop mode
 */
bool DriverStation::isInTeleoperated() const {
    return controlMode() == kControlTeleoperated;
}

/**
 * Returns \c true if the Driver Station has established communications
 * with the FMS.
 */
bool DriverStation::isConnectedToFMS() const {
    return fmsCommStatus() == kCommsWorking;
}

/**
 * Returns \c true if the robot is experiencing a brownout (ouch!)
 */
bool DriverStation::isVoltageBrownout() const {
    return voltageStatus() == kVoltageBrownout;
}

/**
 * Returns \c true if the robot is emergency stopped
 */
bool DriverStation::isEmergencyStopped() const {
    return operationStatus() == kEmergencyStop;
}

/**
 * Returns \c true if the Driver Station has established communications
 * with the robot.
 */
bool DriverStation::isConnectedToRobot() const {
    return robotCommStatus() == kCommsWorking;
}

/**
 * Returns \c true if the Driver Station has established communications
 * with the radio.
 */
bool DriverStation::isConnectedToRadio() const {
    return radioCommStatus() == kCommsWorking;
}

/**
 * Returns \c true if the robot code is running
 */
bool DriverStation::isRobotCodeRunning() const {
    return robotCodeStatus() == kCodeRunning;
}

/**
 * Returns the path in which different DS-related files are stored
 */
QString DriverStation::filesPath() const {
    return DS_FILES_PATH();
}

/**
 * Returns the path in which application log files are stored
 */
QString DriverStation::appLoggerPath() const {
    return DS_LOGGER_PATH();
}

/**
 * Returns the path in which robot log files are stored
 */
QString DriverStation::robotLoggerPath() const {
    return DS_ROBOT_LOGGER_PATH();
}

/**
 * Returns the nominal battery voltage of the robot.
 * This value, along with the \c currentBatteryVoltage() function, can be
 * used to draw graphs or do other cool stuff.
 */
float DriverStation::maxBatteryVoltage() const {
    if (protocol())
        return protocol()->maxBatteryVoltage();

    return 14;
}

/**
 * Returns the current battery voltage of the robot
 */
float DriverStation::currentBatteryVoltage() const {
    return config()->voltage();
}

/**
 * Returns the nominal battery amperage of the robot.
 * This value, along with the \c currentBatteryVoltage() function, can be
 * used to calculate the power (in watts) used by the robot.
 *
 * You could do some cool stuff with this function, such as making a
 * dashboard similar to Tesla's car dashboard.
 */
float DriverStation::nominalBatteryAmperage() const {
    if (protocol())
        return protocol()->nominalBatteryAmperage();

    return 0;
}

/**
 * Returns the current team number
 */
int DriverStation::team() const {
    return config()->team();
}

/**
 * Returns the current CPU usage of the robot.
 * Value range is from 0 to 100.
 */
int DriverStation::cpuUsage() const {
    return config()->cpuUsage();
}

/**
 * Returns the current RAM usage of the robot.
 * Value range is from 0 to 100.
 */
int DriverStation::ramUsage() const {
    return config()->ramUsage();
}

/**
 * Returns the current disk usage of the robot.
 * Value range is from 0 to 100.
 */
int DriverStation::diskUsage() const {
    return config()->diskUsage();
}

/**
 * Returns the current packet loss percentage
 */
int DriverStation::packetLoss() const {
    return m_packetLoss;
}

/**
 * Returns the maximum number of POVs that a joystick can have
 */
int DriverStation::maxPOVCount() const {
    if (protocol())
        return protocol()->maxPOVCount();

    return 0;
}

/**
 * Returns the maximum number of axes that a joystick can have
 */
int DriverStation::maxAxisCount() const {
    if (protocol())
        return protocol()->maxAxisCount();

    return 0;
}

/**
 * Returns the maximum number of buttons that a joystick can have
 */
int DriverStation::maxButtonCount() const {
    if (protocol())
        return protocol()->maxButtonCount();

    return 0;
}

/**
 * Returns the maximum number of joysticks that the protocol can handle
 */
int DriverStation::maxJoystickCount() const {
    if (protocol())
        return protocol()->maxJoystickCount();

    return 0;
}

/**
 * Returns the number of axes registered with the given joystick.
 * \note This will only return the value supported by the protocol, to get
 *       the actual value, use the \c getRealNumAxes() function
 */
int DriverStation::getNumAxes (int joystick) {
    if (joysticks()->count() > joystick)
        return joysticks()->at (joystick)->numAxes;

    return 0;
}

/**
 * Returns the number of POVs registered with the given joystick.
 * \note This will only return the value supported by the protocol, to get
 *       the actual value, use the \c getRealNumPOVs() function
 */
int DriverStation::getNumPOVs (int joystick) {
    if (joysticks()->count() > joystick)
        return joysticks()->at (joystick)->numPOVs;

    return 0;
}

/**
 * Returns the number of buttons registered with the given joystick.
 * \note This will only return the value supported by the protocol, to get
 *       the actual value, use the \c getRealNumButtons() function
 */
int DriverStation::getNumButtons (int joystick) {
    if (joysticks()->count() > joystick)
        return joysticks()->at (joystick)->numButtons;

    return 0;
}

/**
 * Returns the number of axes registered with the given joystick, this value
 * can be greater than what the protocol supports. If you are interested
 * in getting only the number of axes used by the DS, use the \c getNumAxes()
 * function.
 */
int DriverStation::getRealNumAxes (int joystick) {
    if (joysticks()->count() > joystick)
        return joysticks()->at (joystick)->realNumAxes;

    return 0;
}

/**
 * Returns the number of POVs registered with the given joystick, this value
 * can be greater than what the protocol supports. If you are interested
 * in getting only the number of POVs used by the DS, use the \c getNumPOVs()
 * function.
 */
int DriverStation::getRealNumPOVs (int joystick) {
    if (joysticks()->count() > joystick)
        return joysticks()->at (joystick)->realNumPOVs;

    return 0;
}

/**
 * Returns the number of buttons registered with the given joystick, this value
 * can be greater than what the protocol supports. If you are interested
 * in getting only the number of buttons used by the DS, use the
 * \c getNumButtons() function.
 */
int DriverStation::getRealNumButtons (int joystick) {
    if (joysticks()->count() > joystick)
        return joysticks()->at (joystick)->realNumButtons;

    return 0;
}

/**
 * Returns the current number of joysticks registered with the DS.
 */
int DriverStation::joystickCount() {
    return joysticks()->count();
}

/**
 * Returns the actual joysticks and their properties
 */
DS_Joysticks* DriverStation::joysticks() {
    return &m_joysticks;
}

/**
 * Returns the current alliance (red or blue) of the robot.
 */
DS::Alliance DriverStation::alliance() const {
    return config()->alliance();
}

/**
 * Returns the current position (1, 2 or 3) of the robot.
 */
DS::Position DriverStation::position() const {
    return config()->position();
}

/**
 * Returns the current control mode (test, auto or teleop) of the robot.
 */
DS::ControlMode DriverStation::controlMode() const {
    return config()->controlMode();
}

/**
 * Returns the current client/FMS communication status.
 */
DS::CommStatus DriverStation::fmsCommStatus() const {
    return config()->fmsCommStatus();
}

/**
 * Returns the current enable status of the robot.
 */
DS::EnableStatus DriverStation::enableStatus() const {
    return config()->enableStatus();
}

/**
 * Returns the current radio/FMS communication status.
 */
DS::CommStatus DriverStation::radioCommStatus() const {
    return config()->radioCommStatus();
}

/**
 * Returns the current robot/FMS communication status.
 */
DS::CommStatus DriverStation::robotCommStatus() const {
    return config()->robotCommStatus();
}

/**
 * Returns the current status of the robot code
 */
DS::CodeStatus DriverStation::robotCodeStatus() const {
    return config()->robotCodeStatus();
}

/**
 * Returns the current voltage status (normal or brownout) of the robot.
 */
DS::VoltageStatus DriverStation::voltageStatus() const {
    return config()->voltageStatus();
}

/**
 * Returns the applied radio address (be it DS-set or user-set)
 */
QString DriverStation::radioAddress() const {
    if (customRadioAddress().isEmpty())
        return defaultRobotAddress();

    return customRadioAddress();
}

/**
 * Returns the applied robot address (be it DS-set or user-set)
 */
QString DriverStation::robotAddress() const {
    if (customRobotAddress().isEmpty()) {
        if (!m_sockets->robotAddress().isEmpty())
            return m_sockets->robotAddress();
        else
            return "";
    }

    return customRobotAddress();
}

/**
 * Returns the current status of the Driver Station
 */
QString DriverStation::generalStatus() const {
    /* No robot communication */
    if (!isConnectedToRobot())
        return tr ("No Robot Communication");

    /* Emergecy stopped */
    else if (isEmergencyStopped())
        return tr ("Emergency Stopped");

    /* No robot code */
    else if (!isRobotCodeRunning())
        return tr ("No Robot Code");

    /* Voltage brownout (ouch!) */
    else if (isVoltageBrownout())
        return tr ("Voltage Brownout");

    QString mode;
    QString enabled;

    /* Get control mode string */
    switch (controlMode()) {
    case kControlTest:
        mode = tr ("Test");
        break;
    case kControlAutonomous:
        mode = tr ("Autonomous");
        break;
    case kControlTeleoperated:
        mode = tr ("Teleoperated");
        break;
    }

    /* Get enabled string */
    switch (enableStatus()) {
    case kEnabled:
        enabled = tr ("Enabled");
        break;
    case kDisabled:
        enabled = tr ("Disabled");
        break;
    }

    /* Return %mode% %enabled% */
    return mode + " " + enabled;
}

/**
 * Returns the user-set radio address
 */
QString DriverStation::customRadioAddress() const {
    return m_customRadioAddress;
}

/**
 * Returns the user-set robot address
 */
QString DriverStation::customRobotAddress() const {
    return m_customRobotAddress;
}

/**
 * Returns the protocol-set radio address
 */
QString DriverStation::defaultRadioAddress() const {
    if (protocol())
        return protocol()->defaultRadioAddress();

    return getStaticIP (10, team(), 1);
}

/**
 * Returns the protocol-set robot address
 */
QString DriverStation::defaultRobotAddress() const {
    /* Return first item of the address list of the current protocol */
    if (protocol())
        if (protocol()->defaultRobotAddresses().count() > 0)
            return protocol()->defaultRobotAddresses().at (0);

    /* Return 10.xx.yy.2 */
    return getStaticIP (10, team(), 2);
}

/**
 * Returns the current operation status of the robot (normal or e-stop).
 */
DS::OperationStatus DriverStation::operationStatus() const {
    return config()->operationStatus();
}

/**
 * Returns a list with the included protocols of the library.
 * This function is meant to be used to generate your UI elements and
 * seamessly use them with the \c setProtocolType() function.
 */
QStringList DriverStation::protocols() const {
    QStringList list;
    list.append (tr ("FRC 2016"));
    list.append (tr ("FRC 2015"));
    list.append (tr ("FRC 2014"));
    return list;
}

/**
 * Returns a list with the possible alliances and positions.
 * This function is meant to be used to generate your UI elements and
 * seamessly use them with the \c setAllianceAndPosition() function.
 */
QStringList DriverStation::teamStations() const {
    QStringList list;
    list.append (tr ("Red 1"));
    list.append (tr ("Red 2"));
    list.append (tr ("Red 3"));
    list.append (tr ("Blue 1"));
    list.append (tr ("Blue 2"));
    list.append (tr ("Blue 3"));
    return list;
}

/**
 * Registers a new joystick with the given number of \a axes, \a buttons &
 * \a POVs hats.
 */
bool DriverStation::registerJoystick (int axes,
                                      int buttons,
                                      int povs) {
    qDebug() << "Trying to register joystick with"
             << axes    << "axes,"
             << buttons << "buttons and"
             << povs    << "POVs";

    /* Joystick has 0 axes, 0 buttons & 0 POVs, WTF? */
    if (axes <= 0 && buttons <= 0 && povs <= 0) {
        qCritical() << "Joystick has nothing! WTF?";
        qCritical() << "Abort joystick registration";
        return false;
    }

    /* Joystick limit reached */
    else if (joystickCount() + 1 > maxJoystickCount()) {
        qCritical() << "Too many joysticks!";
        qCritical() << "Abort joystick registration";
        return false;
    }

    /* Everything is OK, register the joystick */
    else {
        Joystick* joystick = new Joystick;

        /* Register given number of axes, buttons and POVs */
        joystick->realNumAxes = axes;
        joystick->realNumPOVs = povs;
        joystick->realNumButtons = buttons;

        /* Set number of axes, buttons and POVs according to protocol needs */
        joystick->numAxes = qMin (axes, maxAxisCount());
        joystick->numPOVs = qMin (povs, maxPOVCount());
        joystick->numButtons = qMin (buttons, maxButtonCount());

        /* Initialize joystick values */
        joystick->povs = new int [joystick->numPOVs];
        joystick->axes = new float [joystick->numAxes];
        joystick->buttons = new bool  [joystick->numButtons];

        /* Neutralize joystick values */
        for (int i = 0; i < joystick->numAxes; i++)
            joystick->axes [i] = 0;
        for (int i = 0; i < joystick->numPOVs; i++)
            joystick->povs [i] = -1;
        for (int i = 0; i < joystick->numButtons; i++)
            joystick->buttons [i] = false;

        /* That joystick, Scotty, status report! */
        qDebug() << "Joystick registered!";
        qDebug() << "Registered joystick values:"
                 << joystick->numAxes    << "axes"
                 << joystick->numButtons << "buttons"
                 << joystick->numButtons << "POVs";

        joysticks()->append (joystick);
    }

    qDebug() << "New joystick count is" << joystickCount();

    emit joystickCountChanged (joystickCount());
    return true;
}

/**
 * This function must be called in order to start DS operations
 */
void DriverStation::init() {
    if (!m_init) {
        m_init = true;

        resetFMS();
        resetRadio();
        resetRobot();
        sendFMSPacket();
        sendRadioPacket();
        sendRobotPacket();
        updatePacketLoss();

        emit statusChanged (generalStatus());
        DS_Schedule (200, this, SIGNAL (initialized()));

        qDebug() << "DS engine started!";
    }
}

/**
 * Reboots the robot controller
 */
void DriverStation::rebootRobot() {
    qDebug() << "Robot reboot triggered by DS...";

    if (protocol())
        protocol()->rebootRobot();
}

/**
 * Enables the robot directly
 */
void DriverStation::enableRobot() {
    setEnabled (kEnabled);
}

/**
 * Opens the application logs in an explorer window
 */
void DriverStation::openLogsPath() {
    QDesktopServices::openUrl (QUrl::fromLocalFile (appLoggerPath()));
}

/**
 * Disables the robot directly
 */
void DriverStation::disableRobot() {
    setEnabled (kDisabled);
}

/**
 * Clears all the registered joysticks
 */
void DriverStation::resetJoysticks() {
    qDebug() << "Clearing all joysticks";

    joysticks()->clear();

    if (!isConnectedToFMS())
        setEnabled (false);

    emit joystickCountChanged (joystickCount());
}

/**
 * Restarts the robot code
 */
void DriverStation::restartRobotCode() {
    qDebug() << "Robot code restart triggered by DS...";

    if (protocol())
        protocol()->restartRobotCode();
}

/**
 * Disables the robot and changes the control mode to \c kControlTest
 */
void DriverStation::switchToTestMode() {
    setEnabled (DS::kDisabled);
    setControlMode (DS::kControlTest);
}

/**
 * Disables the robot and changes the control mode to \c kControlAutonomous
 */
void DriverStation::switchToAutonomous() {
    setEnabled (DS::kDisabled);
    setControlMode (DS::kControlAutonomous);
}

/**
 * Disables the robot and changes the control mode to \c kControlTeleoperated
 */
void DriverStation::switchToTeleoperated() {
    setEnabled (DS::kDisabled);
    setControlMode (DS::kControlTeleoperated);
}

/**
 * Re-registers all joysticks based on the criteria specified by the new
 * protocol.
 */
void DriverStation::reconfigureJoysticks() {
    DS_Joysticks list = m_joysticks;
    resetJoysticks();

    qDebug() << "Re-generating joystick list based on protocol preferences";
    qDebug() << protocol()->name() << "supports"
             << maxJoystickCount() << "joysticks with:"
             << maxAxisCount() << "axes,"
             << maxButtonCount() << "buttons and"
             << maxPOVCount() << "POVs";

    foreach (Joystick* joystick, list) {
        registerJoystick (joystick->realNumAxes,
                          joystick->realNumButtons,
                          joystick->realNumPOVs);
    }
}

/**
 * Changes the team number
 */
void DriverStation::setTeam (int team) {
    config()->updateTeam (team);
}

/**
 * Changes the number IPs probed per expiration time.
 * Increasing this value decreases the time needed to detect the robot,
 * but also uses more memory and can over-saturate the LAN access-point.
 *
 * If the \c count is set to \c 0, then the scanner system will calculate
 * an appropiate scan rate based on IP list size.
 */
void DriverStation::setScanRate (int rate) {
    m_sockets->setScanRate (rate);
    if (m_init && (rate > 0 || m_sockets->customScanRate() > 0))
        calculateScanSpeed();
}

/**
 * Removes the joystick at the given \a id
 */
void DriverStation::removeJoystick (int id) {
    if (joystickCount() > id) {
        joysticks()->removeAt (id);

        if (!isConnectedToFMS())
            setEnabled (false);
    }

    emit joystickCountChanged (joystickCount());
}

/**
 * Changes the enabled \a status of the robot.
 * \note This value can be overwritten by the FMS system or the robot
 *       application itself.
 */
void DriverStation::setEnabled (bool enabled) {
    setEnabled (enabled ? DS::kEnabled : DS::kDisabled);
}

/**
 * Loads and configures the given \a protocol in the DS system
 */
void DriverStation::setProtocol (Protocol* protocol) {
    /* Used to separate protocol init messages */
    QString separator = "";

    /* Decommission the current protocol */
    if (m_protocol && protocol) {
        separator = " ";
        free (m_protocol);
        qDebug() << "Protocol" << m_protocol->name() << "decommissioned";
    }

    /* Re-assign the protocol, stop sending data */
    stop();
    m_protocol = protocol;

    /* Update DS config to match new protocol settings */
    if (m_protocol) {
        /* Update radio, FMS and robot socket types */
        m_sockets->setFMSSocketType   (m_protocol->fmsSocketType());
        m_sockets->setRadioSocketType (m_protocol->radioSocketType());
        m_sockets->setRobotSocketType (m_protocol->robotSocketType());

        /* Update radio, FMS and robot ports */
        m_sockets->setFMSInputPort    (m_protocol->fmsInputPort());
        m_sockets->setFMSOutputPort   (m_protocol->fmsOutputPort());
        m_sockets->setRadioInputPort  (m_protocol->radioInputPort());
        m_sockets->setRobotInputPort  (m_protocol->robotInputPort());
        m_sockets->setRadioOutputPort (m_protocol->radioOutputPort());
        m_sockets->setRobotOutputPort (m_protocol->robotOutputPort());

        /* Update NetConsole ports */
        m_console->setInputPort       (m_protocol->netconsoleInputPort());
        m_console->setOutputPort      (m_protocol->netconsoleOutputPort());

        /* Update packet sender intervals */
        m_fmsInterval = 1000 / m_protocol->fmsFrequency();
        m_radioInterval = 1000 / m_protocol->radioFrequency();
        m_robotInterval = 1000 / m_protocol->robotFrequency();

        /* Update the watchdog expiration times */
        m_fmsWatchdog->setExpirationTime (m_fmsInterval * 50);
        m_radioWatchdog->setExpirationTime (m_radioInterval * 50);
        m_robotWatchdog->setExpirationTime (m_robotInterval * 50);

        /* Make the intervals smaller to compensate for hardware delay */
        m_fmsInterval -= (float) m_fmsInterval * 0.1;
        m_radioInterval -= (float) m_radioInterval * 0.1;
        m_robotInterval -= (float) m_robotInterval * 0.1;

        /* Update joystick config. to match protocol requirements */
        reconfigureJoysticks();

        /* Release the kraken */
        start();
        resetFMS();
        resetRadio();
        resetRobot();

        /* Update IP lists */
        m_sockets->setAddressList (m_protocol->defaultRobotAddresses());

        /* Send a message telling that the protocol has been initialized */
        emit protocolChanged();
        emit newMessage (separator);
        emit newMessage (CONSOLE_MESSAGE (tr ("DS: %1 initialized")
                                          .arg (m_protocol->name())));

        /* Tell client how much time is needed for detecting robot */
        if (m_sockets->customScanRate() == 0)
            calculateScanSpeed();

        /* We're back in business */
        qDebug() << "Protocol" << protocol->name() << "ready for use";
    }
}

/**
 * If you are lazy enough to not wanting to use two function calls to
 * change the alliance & position of the robot, we've got you covered!
 */
void DriverStation::setTeamStation (int station) {
    switch ((TeamStation) station) {
    case kRed1:
        setPosition (kPosition1);
        setAlliance (kAllianceRed);
        break;
    case kRed2:
        setPosition (kPosition2);
        setAlliance (kAllianceRed);
        break;
    case kRed3:
        setPosition (kPosition3);
        setAlliance (kAllianceRed);
        break;
    case kBlue1:
        setPosition (kPosition1);
        setAlliance (kAllianceBlue);
        break;
    case kBlue2:
        setPosition (kPosition2);
        setAlliance (kAllianceBlue);
        break;
    case kBlue3:
        setPosition (kPosition3);
        setAlliance (kAllianceBlue);
        break;
    }
}

/**
 * Given the \c protocol, this function will initialize, load and configure
 * the defined protocol.
 *
 * This function is meant to be used in co-junction of the list outputted
 * by the \c protocols() function.
 */
void DriverStation::setProtocolType (int protocol) {
    if ((ProtocolType) protocol == kFRC2016)
        setProtocol (new FRC_2016);

    if ((ProtocolType) protocol == kFRC2015)
        setProtocol (new FRC_2015);

    if ((ProtocolType) protocol == kFRC2014)
        setProtocol (new FRC_2014);
}

/**
 * Updates the team \a alliance.
 * \note This value can be overwritten by the FMS system
 */
void DriverStation::setAlliance (Alliance alliance) {
    config()->updateAlliance (alliance);
}

/**
 * Updates the team \a position
 * \note This value can be overwritten by the FMS system
 */
void DriverStation::setPosition (Position position) {
    config()->updatePosition (position);
}

/**
 * Changes the control \a mode of the robot.
 * \note This value can be overwritten by the FMS system
 */
void DriverStation::setControlMode (ControlMode mode) {
    config()->updateControlMode (mode);
}

/**
 * Updates the \a angle of the given \a pov of the joystick with the
 * specified \a id
 */
void DriverStation::updatePOV (int id, int pov, int angle) {
    if (joysticks()->count() > abs (id)) {
        if (joysticks()->at (id)->numPOVs > pov) {
            joysticks()->at (id)->povs [abs (pov)] = angle;
            return;
        }
    }
}

/**
 * Changes the enabled \a status of the robot.
 * \note This value can be overwritten by the FMS system or the robot
 *       application itself.
 */
void DriverStation::setEnabled (EnableStatus status) {
    config()->updateEnabled (status);
}

/**
 * Updates the \a value of the given \a axis of the joystick with the
 * specified \a id
 */
void DriverStation::updateAxis (int id, int axis, float value) {
    if (joysticks()->count() > abs (id)) {
        if (joysticks()->at (id)->numAxes > axis) {
            joysticks()->at (id)->axes [abs (axis)] = value;
            return;
        }
    }
}

/**
 * Updates the \a pressed state of the given \a button of the joystick with
 * the specified \a id
 */
void DriverStation::updateButton (int id, int button, bool state) {
    if (joysticks()->count() > abs (id)) {
        if (joysticks()->at (id)->numButtons > button) {
            joysticks()->at (id)->buttons [abs (button)] = state;
            return;
        }
    }
}

/**
 * Forces the system to use the given \a address. This can be useful when
 * you already know the IP address of the robot, or you are simulating
 * your robot program with a specific virtual IP address.
 *
 * Using this function is discouraged, the LibDS is fast enough to remove
 * the need of defining the IP address of the robot by yourself.
 */
void DriverStation::setCustomRobotAddress (const QString& address) {
    m_customRobotAddress = address;
    m_sockets->setRobotAddress (customRobotAddress());
}

/**
 * Forces the system to look for the robot radio at the given \a address
 * instead of using the default radio address specified by the current
 * protocol.
 */
void DriverStation::setCustomRadioAddress (const QString& address) {
    m_sockets->setRadioAddress (address);
}

/**
 * Changes the operation \c status of the robot.
 * Possible values are:
 *   - \c kDisconnected
 *   - \c kNormal
 *   - \c kEmergencyStop
 *
 * If you are writting your own Driver Station client, we strongly
 * encourage you to implement the hability to e-stop the robot in your
 * custom client.
 */
void DriverStation::setOperationStatus (OperationStatus status) {
    config()->updateOperationStatus (status);
}

/**
 * Inhibits the DS to send and receive packets
 */
void DriverStation::stop() {
    m_running = false;
    qDebug() << "DS networking operations stopped";
}

/**
 * Allows the DS to send and receive packets
 */
void DriverStation::start() {
    m_running = true;
    qDebug() << "DS networking operations resumed";
}

/**
 * Called when the FMS watchdog expires
 */
void DriverStation::resetFMS() {
    if (protocol())
        protocol()->onFMSWatchdogExpired();

    config()->updateFMSCommStatus (kCommsFailing);
}

/**
 * Called when the radio watchdog expires
 */
void DriverStation::resetRadio() {
    if (protocol())
        protocol()->onRadioWatchdogExpired();

    config()->updateRadioCommStatus (kCommsFailing);
}

/**
 * Called when the robot watchdog expires
 */
void DriverStation::resetRobot() {
    if (protocol())
        protocol()->onRobotWatchdogExpired();

    config()->updateVoltage (0);
    config()->updateSimulated (false);
    config()->updateEnabled (kDisabled);
    config()->updateVoltageStatus (kVoltageNormal);
    config()->updateRobotCodeStatus (kCodeFailing);
    config()->updateRobotCommStatus (kCommsFailing);
    config()->updateOperationStatus (kNormal);
    config()->robotLogger()->registerWatchdogTimeout();

    if (customRobotAddress().isEmpty()) {
        m_sockets->setRobotAddress ("");
        m_sockets->refreshAddressList();
    }

    emit statusChanged (generalStatus());
}

/**
 * Generates and sends a new FMS packet
 */
void DriverStation::sendFMSPacket() {
    if (protocol() && running())
        m_sockets->sendToFMS (protocol()->generateFMSPacket());

    DS_Schedule (m_fmsInterval, this, SLOT (sendFMSPacket()));
}

/**
 * Generates and sends a new radio packet
 */
void DriverStation::sendRadioPacket() {
    if (protocol() && running())
        m_sockets->sendToRadio (protocol()->generateRadioPacket());

    DS_Schedule (m_radioInterval, this, SLOT (sendRadioPacket()));
}

/**
 * Generates and sends a new robot packet
 */
void DriverStation::sendRobotPacket() {
    if (protocol() && running())
        m_sockets->sendToRobot (protocol()->generateRobotPacket());

    DS_Schedule (m_robotInterval, this, SLOT (sendRobotPacket()));
}

/**
 * Calculates the current packet loss as a percent
 */
void DriverStation::updatePacketLoss() {
    float loss = 0;
    float sentPackets = 0;
    float recvPackets = 0;

    if (protocol()) {
        recvPackets = protocol()->receivedRobotPackets();
        sentPackets = protocol()->sentRobotPacketsSinceConnect();
    }

    if (recvPackets <= 0 || sentPackets <= 0) {
        if (recvPackets <= 0)
            loss = 100;
        if (sentPackets <= 0)
            loss = 0;
    }

    else if (recvPackets < sentPackets)
        loss = (recvPackets / sentPackets) * 100;

    m_packetLoss = (int) loss;
    DS_Schedule (250, this, SLOT (updatePacketLoss()));
}

/**
 * Calculates the time required to detect the robot (assuming it is in one
 * of the LAN networks of the client computer)
 */
void DriverStation::calculateScanSpeed() {
    QString ipsCount = CONSOLE_MESSAGE (tr ("DS: Probing %1 IPs per cycle"));
    QString scanTime = CONSOLE_MESSAGE (tr ("DS: It may take up to %1 seconds "
                                            "to detect your robot"));

    int time = m_sockets->addressList().count() / m_sockets->scanRate();
    time *= m_robotWatchdog->expirationTime() / 1000;

    /* It takes less than one second to detect the robot (in theory) */
    if (time <= 1)
        scanTime = CONSOLE_MESSAGE (tr ("DS: It should take less than 1 "
                                        "second to detect yout robot"));

    /* It takes 2 or more seconds to detect the robot */
    else
        scanTime = scanTime.arg (time);

    emit newMessage (ipsCount.arg (m_sockets->scanRate()));
    emit newMessage (scanTime);
}

/**
 * Instructs the current protocol to interpret the given \a data, which
 * was sent by the FMS.
 */
void DriverStation::readFMSPacket (const QByteArray& data) {
    if (protocol() && running()) {
        m_fmsWatchdog->reset();
        protocol()->readFMSPacket (data);
    }
}

/**
 * Instructs the current protocol to interpret the given \a data, which
 * was sent by the robot radio.
 */
void DriverStation::readRadioPacket (const QByteArray& data) {
    if (protocol() && running()) {
        m_radioWatchdog->reset();
        protocol()->readRadioPacket (data);
    }
}

/**
 * Instructs the current protocol to interpret the given \a data, which
 * was sent by the robot controller.
 */
void DriverStation::readRobotPacket (const QByteArray& data) {
    if (protocol() && running()) {
        m_robotWatchdog->reset();
        protocol()->readRobotPacket (data);
    }
}

/**
 * Returns a pointer to the \c DS_Config class, which is shared by the
 * \c DriverStation and the protocol
 */
DS_Config* DriverStation::config() const {
    return DS_Config::getInstance();
}

/**
 * Returns a pointer to the current loaded protocol. Always check if the
 * pointer is not \c NULL before using it!
 */
Protocol* DriverStation::protocol() const {
    return m_protocol;
}

/*
 * This comment is not procesed by Doxygen. If you are reading this, it is
 * because you are reading the code and trying to understand how it works.
 * Maybe you are a future maintainer of this project, trying to fix all the
 * mistakes that I made. Maybe you are a just interested in understandanding
 * how all this works. Eitherway, you are awesome, and I thank you for devoting
 * some of your time to this project.
 *
 * Hack on.
 * -- Alex Spataru
 */
