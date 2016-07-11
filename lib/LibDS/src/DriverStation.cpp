/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

//------------------------------------------------------------------------------
// Class includes
//------------------------------------------------------------------------------

#include "DriverStation.h"

//------------------------------------------------------------------------------
// Import core section
//------------------------------------------------------------------------------

#include "Core/Logger.h"
#include "Core/Sockets.h"
#include "Core/Protocol.h"
#include "Core/Watchdog.h"
#include "Core/DS_Config.h"
#include "Core/NetConsole.h"

//------------------------------------------------------------------------------
// Import protocols
//------------------------------------------------------------------------------

#include "Protocols/FRC_2014.h"
#include "Protocols/FRC_2015.h"
#include "Protocols/FRC_2016.h"

//------------------------------------------------------------------------------
// Import other Qt Dependencies
//------------------------------------------------------------------------------

#include <QDir>
#include <QUrl>
#include <QFileDialog>
#include <QDesktopServices>

/**
 * Formats the input message so that it looks nice on a console display widget
 */
static QString CONSOLE_MESSAGE (const QString& input) {
    return "<font color='#888'>** " + input + "</font>";
}

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

    /* Initialize custom addresses */
    m_customFMSAddress = "";
    m_customRadioAddress = "";
    m_customRobotAddress = "";

    /* Initialize DS modules & watchdogs */
    m_sockets = new Sockets;
    m_console = new NetConsole;
    m_fmsWatchdog = new Watchdog;
    m_radioWatchdog = new Watchdog;
    m_robotWatchdog = new Watchdog;

    /* React when the sockets receive data from FMS, radio or robot */
    connect (m_sockets, SIGNAL (fmsPacketReceived   (QByteArray)),
             this,        SLOT (readFMSPacket       (QByteArray)));
    connect (m_sockets, SIGNAL (radioPacketReceived (QByteArray)),
             this,        SLOT (readRadioPacket     (QByteArray)));
    connect (m_sockets, SIGNAL (robotPacketReceived (QByteArray)),
             this,        SLOT (readRobotPacket     (QByteArray)));

    /* Begin the lookup process when the app initializes the DS */
    connect (this, SIGNAL (initialized()), m_sockets, SLOT (performLookups()));

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
    connect (config(), SIGNAL (voltageChanged (qreal)),
             this,     SIGNAL (voltageChanged (qreal)));
    connect (config(), SIGNAL (voltageChanged (QString)),
             this,     SIGNAL (voltageChanged (QString)));
    connect (config(), SIGNAL (voltageStatusChanged (VoltageStatus)),
             this,     SIGNAL (voltageStatusChanged (VoltageStatus)));

    /* Update the robot, radio & FMS IPs when the team number is changed */
    connect (config(), SIGNAL (teamChanged (int)),
             this,     SLOT   (updateAddresses (int)));

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
    stop();
    config()->logger()->saveLogs();
}

/**
 * Returns the one and only instance of the \c DriverStation class.
 * The \c DriverStation class must be singleton to ensure the safe operation
 * of the robot and also to ensure availability to the input and output sockets
 * used by each protocol.
 */
DriverStation* DriverStation::getInstance() {
    static DriverStation instance;
    return &instance;
}

/**
 * Calls the custom Qt message handler of the LibDS
 */
void DriverStation::logger (QtMsgType type,
                            const QMessageLogContext& context,
                            const QString& data) {
    DS_Config::getInstance()->logger()->messageHandler (type,
            context,
            data);
}

/**
 * Returns \c true if we have communications with the robot controller
 * and the robot code is running.
 *
 * \note This function will return \c false if the robot is emergency stopped.
 */
bool DriverStation::canBeEnabled() {
    return isConnectedToRobot() && isRobotCodeRunning() && !isEmergencyStopped();
}

/**
 * Returns \c true if the DS is currently allowed to send and receive data.
 * This function is called whenever we send a packet or when we receive a
 * packet.
 *
 * \note If this function returns \c false, then the DS will ignore all incoming
 *       packets and inhibit packet sending
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
 * Returns the path in which application log files are stored
 */
QString DriverStation::logsPath() const {
    return config()->logger()->logsPath();
}

/**
 * Returns the current JSON document as a variant list
 */
QVariant DriverStation::logVariant() const {
    return logDocument().toVariant();
}

/**
 * Returns a list with all the robot logs saved to the logs path
 */
QStringList DriverStation::availableLogs() const {
    return config()->logger()->availableLogs();
}

/**
 * Returns the current JSON log document
 */
QJsonDocument DriverStation::logDocument() const {
    return m_logDocument;
}

/**
 * Returns the nominal battery voltage of the robot.
 * This value, along with the \c currentBatteryVoltage() function, can be
 * used to draw graphs or do other cool stuff.
 *
 * \note If there is no protocol loaded, this function will return 0
 */
qreal DriverStation::maxBatteryVoltage() const {
    if (protocol())
        return protocol()->nominalBatteryVOltage();

    return 0;
}

/**
 * Returns the current battery voltage of the robot
 */
qreal DriverStation::currentBatteryVoltage() const {
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
qreal DriverStation::nominalBatteryAmperage() const {
    if (protocol())
        return protocol()->nominalBatteryAmperage();

    return 0;
}

/**
 * Returns the current team number, which can be used by the client application.
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
 * Returns the current packet loss percentage (from 0 to 100).
 * \note This value is updated every 250 milliseconds.
 */
int DriverStation::packetLoss() const {
    return m_packetLoss;
}

/**
 * Returns the maximum number of POVs that a joystick can have
 * \note If the protocol is invalid, this function will return 0
 */
int DriverStation::maxPOVCount() const {
    if (protocol())
        return protocol()->maxPOVCount();

    return 0;
}

/**
 * Returns the maximum number of axes that a joystick can have
 * \note If the protocol is invalid, this function will return 0
 */
int DriverStation::maxAxisCount() const {
    if (protocol())
        return protocol()->maxAxisCount();

    return 0;
}

/**
 * Returns the maximum number of buttons that a joystick can have
 * \note If the protocol is invalid, this function will return 0
 */
int DriverStation::maxButtonCount() const {
    if (protocol())
        return protocol()->maxButtonCount();

    return 0;
}

/**
 * Returns the maximum number of joysticks that the protocol can handle
 * \note If the protocol is invalid, this function will return 0
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
 * Returns the applied FMS address (be it DS-set or user-set)
 */
QString DriverStation::fmsAddress() const {
    if (customFMSAddress().isEmpty())
        return defaultFMSAddress();

    return customFMSAddress();
}

/**
 * Returns the applied radio address (be it DS-set or user-set)
 */
QString DriverStation::radioAddress() const {
    if (customRadioAddress().isEmpty())
        return defaultRadioAddress();

    return customRadioAddress();
}

/**
 * Returns the applied robot address (be it DS-set or user-set)
 */
QString DriverStation::robotAddress() const {
    if (customRobotAddress().isEmpty())
        return defaultRobotAddress();

    return customRobotAddress();
}

/**
 * Returns the current status of the Driver Station.
 * This string is used in the large label below the status lights of the DS.
 */
QString DriverStation::generalStatus() const {
    /* No robot communication */
    if (!isConnectedToRobot())
        return tr ("No Robot Communication");

    /* No robot code */
    else if (!isRobotCodeRunning())
        return tr ("No Robot Code");

    /* Voltage brownout (ouch!) */
    else if (isVoltageBrownout())
        return tr ("Voltage Brownout");

    /* Emergecy stopped */
    else if (isEmergencyStopped())
        return tr ("Emergency Stopped");

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
 * Returns the user-set FMS address
 */
QString DriverStation::customFMSAddress() const {
    return m_customFMSAddress;
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
 * Returns the protocol-set robot address.
 * \note If the protocol is invalid, this function will return an empty string
 */
QString DriverStation::defaultFMSAddress() const {
    if (protocol())
        return protocol()->fmsAddress();

    return "";
}

/**
 * Returns the protocol-set radio address.
 * \note If the protocol is invalid, this function will return an empty string
 */
QString DriverStation::defaultRadioAddress() const {
    if (protocol())
        return protocol()->radioAddress();

    return "";
}

/**
 * Returns the protocol-set robot address.
 * \note If the protocol is invalid, this function will return an empty string
 */
QString DriverStation::defaultRobotAddress() const {
    if (protocol())
        return protocol()->robotAddress();

    return "";
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
 *
 * \note If joystick registration fails, this function will return \c false.
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
        joystick->axes = new qreal [joystick->numAxes];
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
 * This function must be called in order to start DS operations.
 * It is recommended to call this function AFTER your UI has been initialized,
 * so that the UI can react to the signals sent by the \c DriverStation.
 *
 * \note You can safely call this function several times, however, it will
 *       only have effect the first time you call it.
 */
void DriverStation::init() {
    if (!m_init) {
        m_init = true;

        config()->logger()->registerInitialEvents();

        resetFMS();
        resetRadio();
        resetRobot();
        sendFMSPacket();
        sendRadioPacket();
        sendRobotPacket();
        updatePacketLoss();

        DS_Schedule (250, this, SLOT (finishInit()));

        qDebug() << "DS engine started!";
    }
}

/**
 * Shows an open file dialog and lets the user select a DS log file to load...
 */
void DriverStation::browseLogs() {
    QString filter = "*." + config()->logger()->extension();
    QString file = QFileDialog::getOpenFileName (Q_NULLPTR,
                                                 tr ("Select a log file..."),
                                                 logsPath(),
                                                 filter);

    if (!file.isEmpty())
        openLog (file);
}

/**
 * Reboots the robot controller (if a protocol is loaded)
 */
void DriverStation::rebootRobot() {
    if (protocol()) {
        protocol()->rebootRobot();
        qDebug() << "Robot reboot triggered by DS...";
    }
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
    QDesktopServices::openUrl (QUrl::fromLocalFile (logsPath()));
}

/**
 * Disables the robot directly
 */
void DriverStation::disableRobot() {
    setEnabled (kDisabled);
}

/**
 * Removes all the registered joysticks
 */
void DriverStation::resetJoysticks() {
    qDebug() << "Clearing all joysticks";

    joysticks()->clear();

    if (!isConnectedToFMS())
        setEnabled (false);

    emit joystickCountChanged (joystickCount());
}

/**
 * Changes the team number to the given \a team.
 * \note Calling this function will trigger a re-evaluation of the FMS, radio
         and robot IPs (only if communications have not been established yet)
 */
void DriverStation::setTeam (int team) {
    config()->updateTeam (team);
}

/**
 * Restarts the robot code (if a protocol is loaded)
 */
void DriverStation::restartRobotCode() {
    if (protocol()) {
        protocol()->restartRobotCode();
        qDebug() << "Robot code restart triggered by DS...";
    }
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
 *
 * \note If the number of buttons of a joystick exceeds the number of buttons
 *       supported by the protocol, the \c DriverStation will register only
 *       the ammount of buttons supported by the protocol. The same process
 *       takes place for axes and POVs.
 */
void DriverStation::reconfigureJoysticks() {
    DS_Joysticks list = m_joysticks;
    resetJoysticks();

    qDebug() << "Re-generating joystick list based on protocol preferences";
    qDebug() << protocol()->name() << "supports"
             << maxJoystickCount() << "joysticks with:"
             << maxAxisCount()     << "axes,"
             << maxButtonCount()   << "buttons and"
             << maxPOVCount()      << "POVs";

    foreach (Joystick* joystick, list) {
        registerJoystick (joystick->realNumAxes,
                          joystick->realNumButtons,
                          joystick->realNumPOVs);
    }
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
 * If you are lazy enough to not wanting to use two function calls to
 * change the alliance & position of the robot, we've got you covered!
 *
 * Actually, this function is available so that the UI elements (such as a
 * combobox with team stations) can interact directly with the \c DriverStation
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
 * Opens the given log file \a file and parses it as a JSON document
 */
void DriverStation::openLog (const QString& file) {
    m_logDocument = config()->logger()->openLog (file);
    emit logFileChanged();
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
 * Loads and configures the given \a protocol with the LibDS system.
 *
 * \note Ths DS networking operations will be stopped while configuring the
 *       new protocol. Once the protocol has been configured, the DS networking
 *       operations will be resumed automatically.
 * \note All joysticks will be reconfigured based on the standards set by the
 *       new \a protocol.
 */
void DriverStation::setProtocol (Protocol* protocol) {
    /* Decommission the current protocol */
    if (m_protocol && protocol) {
        qDebug() << "Protocol" << m_protocol->name() << "decommissioned";

        emit newMessage (CONSOLE_MESSAGE (tr ("DS: %1 terminated")
                                          .arg (m_protocol->name())));

        free (m_protocol);
    }

    /* Re-assign the protocol, stop sending data */
    stop();
    m_protocol = protocol;

    /* Update DS config to match new protocol settings */
    if (m_protocol) {
        qDebug() << "Configuring new protocol...";

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
        m_console->setInputPort (m_protocol->netconsoleInputPort());
        m_console->setOutputPort (m_protocol->netconsoleOutputPort());

        /* Update packet sender intervals */
        m_fmsInterval = 1000 / m_protocol->fmsFrequency();
        m_radioInterval = 1000 / m_protocol->radioFrequency();
        m_robotInterval = 1000 / m_protocol->robotFrequency();

        /* Update the watchdog expiration times */
        m_fmsWatchdog->setExpirationTime (m_fmsInterval * 50);
        m_radioWatchdog->setExpirationTime (m_radioInterval * 50);
        m_robotWatchdog->setExpirationTime (m_robotInterval * 50);

        /* Make the intervals smaller to compensate for hardware delay */
        m_fmsInterval -= static_cast<qreal> (m_fmsInterval) * 0.1;
        m_radioInterval -= static_cast<qreal> (m_radioInterval) * 0.1;
        m_robotInterval -= static_cast<qreal> (m_robotInterval) * 0.1;

        /* Update joystick config. to match protocol requirements */
        reconfigureJoysticks();

        /* Set protocol addresses */
        updateAddresses();

        /* Release the kraken */
        start();
        resetFMS();
        resetRadio();
        resetRobot();

        /* Send a message telling that the protocol has been initialized */
        emit protocolChanged();
        emit newMessage (CONSOLE_MESSAGE (tr ("DS: %1 initialized")
                                          .arg (m_protocol->name())));

        /* We're back in business */
        qDebug() << "Protocol" << protocol->name() << "ready for use";
    }
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
 *
 * \note If the given joystick is invalid, this function will silently ignore
 *       your request
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
 *
 * \note This value can be overwritten by the FMS system or the robot
 *       application itself.
 */
void DriverStation::setEnabled (EnableStatus status) {
    config()->updateEnabled (status);
}

/**
 * Updates the \a value of the given \a axis of the joystick with the
 * specified \a id
 *
 * \note If the given joystick is invalid, this function will silently ignore
 *       your request
 */
void DriverStation::updateAxis (int id, int axis, qreal value) {
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
 *
 * \note If the given joystick is invalid, this function will silently ignore
 *       your request
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
 * Forces the system to look for the FMS at the given \a address
 * instead of using the default FMS address specified by the current
 * protocol.
 */
void DriverStation::setCustomFMSAddress (const QString& address) {
    m_customFMSAddress = address;
    m_sockets->setFMSAddress (fmsAddress());
}

/**
 * Forces the system to look for the robot radio at the given \a address
 * instead of using the default radio address specified by the current
 * protocol.
 */
void DriverStation::setCustomRadioAddress (const QString& address) {
    m_customRadioAddress = address;
    m_sockets->setRadioAddress (radioAddress());
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
    m_sockets->setRobotAddress (robotAddress());
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
    if (protocol()) {
        protocol()->resetLossCounter();
        protocol()->onRobotWatchdogExpired();
    }

    config()->updateVoltage (0);
    config()->updateSimulated (false);
    config()->updateEnabled (kDisabled);
    config()->updateOperationStatus (kNormal);
    config()->updateVoltageStatus (kVoltageNormal);
    config()->updateRobotCodeStatus (kCodeFailing);
    config()->updateRobotCommStatus (kCommsFailing);

    emit statusChanged (generalStatus());
}

/**
 * Notifies the UI that the DS has been initialized
 */
void DriverStation::finishInit() {
    emit initialized();
    emit statusChanged (generalStatus());
}

/**
 * Generates and sends a new FMS packet only if we are already connected to
 * the FMS
 */
void DriverStation::sendFMSPacket() {
    if (protocol() && running() && isConnectedToFMS())
        m_sockets->sendToFMS (protocol()->generateFMSPacket());

    DS_Schedule (m_fmsInterval, this, SLOT (sendFMSPacket()));
}

/**
 * Ensures that the IP addresses are updated when the application changes the
 * team number.
 */
void DriverStation::updateAddresses() {
    m_sockets->setFMSAddress (fmsAddress());
    m_sockets->setRadioAddress (radioAddress());
    m_sockets->setRobotAddress (robotAddress());
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
    qreal loss = 0;
    qreal sentPackets = 0;
    qreal recvPackets = 0;

    /* Protocol is valid, get the data from its counters */
    if (protocol()) {
        recvPackets = protocol()->recvRobotPacketsSinceConnect();
        sentPackets = protocol()->sentRobotPacketsSinceConnect();
    }

    /* Calculate packet loss */
    if (recvPackets > 0 && sentPackets > 0)
        loss = (1 - (recvPackets / sentPackets)) * 100;
    else if (!isConnectedToRobot())
        loss = 100;

    /* Update packet loss */
    m_packetLoss = static_cast<int> (loss);
    config()->logger()->registerPacketLoss (m_packetLoss);

    /* Schedule next loss calculation */
    DS_Schedule (250, this, SLOT (updatePacketLoss()));
}

/**
 * This overloaded function is called when the team number is changed
 * (hence the \c int in the argument).
 *
 * As its name suggests, the input value is \a unused, this function was
 * implemented to avoid possible errors in the Qt signal/slot system, while
 * maintaining the functionality of the original \c updateAddresses() function
 */
void DriverStation::updateAddresses (int unused) {
    Q_UNUSED (unused);
    updateAddresses();
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
 * \c DriverStation and the protocol.
 *
 * The point of having a config class is to allow the protocol to change the
 * status of the \c DriverStation and the LibDS system without increasing the
 * complexity of the \c Protocol and \c DriverStation classes, whilst ensuring
 * that values are the same throughout the LibDS and that signals are only
 * fired when necessary.
 */
DS_Config* DriverStation::config() const {
    return DS_Config::getInstance();
}

/**
 * Returns a pointer to the current loaded protocol.
 * \warning Always check if the pointer is not \c NULL before using it!
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
