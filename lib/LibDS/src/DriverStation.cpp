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

#include <QTimer>
#include <QSound>
#include <QApplication>

//==================================================================================================
// Core components
//==================================================================================================

#include "LibDS/Core/NetConsole.h"
#include "LibDS/Core/ElapsedTimer.h"

//==================================================================================================
// Protocols
//==================================================================================================

#include "LibDS/Protocols/FRC_2014.h"
#include "LibDS/Protocols/FRC_2015.h"
#include "LibDS/Protocols/FRC_2016.h"

//==================================================================================================
// Library class
//==================================================================================================

#include "LibDS/DriverStation.h"

//==================================================================================================
// Joystick requirements
//==================================================================================================

const int MAX_AXES      = 0x0C; // Support for 12 axes
const int MAX_POVS      = 0x0C; // Support for 12 POVs
const int MAX_BUTTONS   = 0x18; // Support for 24 buttons
const int MAX_JOYSTICKS = 0x06; // Support for 6 joysticks

//==================================================================================================
// Joystick error messages/warnings
//==================================================================================================

const QString ERR_POV_CNT     = "<font color=#FFF959><b>WARNING:</b></font> "
                                "<font color=#FFFFFF>Joystick %1 has exceed "
                                "the POV range (0 to %2 POVs). "
                                "Joystick %1 has %3 POVs.</font>";
const QString ERR_AXIS_CNT    = "<font color=#FFF959><b>WARNING:</b></font> "
                                "<font color=#FFFFFF>Joystick %1 has exceed "
                                "the axis range (0 to %2 axes). "
                                "Joystick %1 has %3 axes.</font>";
const QString ERR_BUTTON_CNT  = "<font color=#FFF959><b>WARNING:</b></font> "
                                "<font color=#FFFFFF>Joystick %1 has exceed "
                                "the button range (0 to %2 buttons). "
                                "Joystick %1 has %3 buttons.</font>";
const QString ERR_JOYSTICKS   = "<font color=#FFF959><b>WARNING:</b></font> "
                                "<font color=#FFFFFF>Reached maximum number of "
                                "joysticks (%1), further joysticks will be "
                                "ignored.</font>";

//==================================================================================================
// Ugly hacks
//==================================================================================================

template <typename T>
T* GET_INSTANCE() {
    static T instance;
    return &instance;
}

//==================================================================================================
// DriverStation::DriverStation
//==================================================================================================

DriverStation::DriverStation() {
    qRegisterMetaType <DS::CAN_Information> ("CAN_Information");
    qRegisterMetaType <DS::Joystick>        ("Joystick");
    qRegisterMetaType <DS::Alliance>        ("Alliance");
    qRegisterMetaType <DS::CommStatus>      ("CommStatus");
    qRegisterMetaType <DS::ControlMode>     ("ControlMode");
    qRegisterMetaType <DS::RumbleRequest>   ("RumbleRequest");

    m_init = false;
    m_practiceRunning = false;
    m_practiceInterrupted = true;

    /* Initialize private members */
    m_protocol    = Q_NULLPTR;
    m_netConsole  = new DS_Core::NetConsole (this);
    m_elapsedTime = new DS_Core::ElapsedTime (this);

    /* Lamda-functions for auto-updating robot status */
    connect (this, &DriverStation::codeChanged,
    [ = ] (bool has_code) {
        Q_UNUSED (has_code);
        emit robotStatusChanged (getRobotStatus());
    });
    connect (this, &DriverStation::communicationsChanged,
    [ = ] (DS::CommStatus status) {
        Q_UNUSED (status);
        emit robotStatusChanged (getRobotStatus());
    });
    connect (this, &DriverStation::controlModeChanged,
    [ = ] (DS::ControlMode mode) {
        Q_UNUSED (mode);
        emit robotStatusChanged (getRobotStatus());
    });
    connect (this, &DriverStation::enabledChanged,
    [ = ] (bool enabled) {
        Q_UNUSED (enabled);
        emit robotStatusChanged (getRobotStatus());
    });

    /* Configure signals/slots */
    connect (m_elapsedTime, &DS_Core::ElapsedTime::elapsedTimeChanged,
             this,          &DriverStation::elapsedTimeChanged);
    connect (m_netConsole,  &DS_Core::NetConsole::newMessage,
             this,          &DriverStation::newMessage);
}

//==================================================================================================
// DriverStation::getInstance
//==================================================================================================

DriverStation* DriverStation::getInstance() {
    static DriverStation instance;
    return &instance;
}

//==================================================================================================
// DriverStation::getPSC
//==================================================================================================

int DriverStation::getPSC() {
    if (protocolLoaded())
        return currentProtocol()->getPSC();

    return 0;
}

//==================================================================================================
// DriverStation::isInitialized
//==================================================================================================

bool DriverStation::isInitialized() {
    return m_init;
}

//==================================================================================================
// DriverStation::protocolLoaded
//==================================================================================================

bool DriverStation::protocolLoaded() {
    return currentProtocol() != Q_NULLPTR;
}

//==================================================================================================
// DriverStation::canBeEnabled
//==================================================================================================

bool DriverStation::canBeEnabled() {
    return robotHasCode() && isConnected() && !isEmergencyStopped();
}

//==================================================================================================
// DriverStation::alliances
//==================================================================================================

QStringList DriverStation::alliances() {
    QStringList list;
    list.append (tr ("Red 1"));
    list.append (tr ("Red 2"));
    list.append (tr ("Red 3"));
    list.append (tr ("Blue 1"));
    list.append (tr ("Blue 2"));
    list.append (tr ("Blue 3"));
    return list;
}

//==================================================================================================
// DriverStation::protocols
//==================================================================================================

QStringList DriverStation::protocols() {
    QStringList list;
    list.append (tr ("FRC 2016"));
    list.append (tr ("FRC 2015"));
    list.append (tr ("FRC 2014"));
    return list;
}

//==================================================================================================
// DriverStation::radioAddress
//==================================================================================================

QString DriverStation::radioAddress() {
    if (protocolLoaded())
        return currentProtocol()->radioIPs().at (0);

    return "";
}

//==================================================================================================
// DriverStation::robotAddress
//==================================================================================================

QString DriverStation::robotAddress() {
    if (protocolLoaded())
        return currentProtocol()->robotAddress();

    return "";
}

//==================================================================================================
// DriverStation::defaultRobotAddress
//==================================================================================================

QString DriverStation::defaultRobotAddress() {
    if (protocolLoaded())
        return currentProtocol()->additionalRobotIPs().at (0);

    return "";
}

//==================================================================================================
// DriverStation::defaultRobotAddresses
//==================================================================================================

QStringList DriverStation::defaultRobotAddresses() {
    if (protocolLoaded())
        return currentProtocol()->robotIPs();

    return QStringList ("");
}

//==================================================================================================
// DriverStation::controlMode
//==================================================================================================

DS::ControlMode DriverStation::controlMode() {
    if (protocolLoaded())
        return currentProtocol()->controlMode();

    return DS::kControlInvalid;
}

//==================================================================================================
// DriverStation::joysticks
//==================================================================================================

QList<DS::Joystick>* DriverStation::joysticks() {
    return &m_joysticks;
}

//==================================================================================================
// DriverStation::currentProtocol
//==================================================================================================

DS_Core::AbstractProtocol* DriverStation::currentProtocol() {
    return m_protocol;
}

//==================================================================================================
// DriverStation::robotHasCode
//==================================================================================================

bool DriverStation::robotHasCode() {
    if (protocolLoaded())
        return currentProtocol()->hasCode();

    return false;
}

//==================================================================================================
// DriverStation::isEnabled
//==================================================================================================

bool DriverStation::isEnabled() {
    if (protocolLoaded())
        return currentProtocol()->isEnabled();

    return false;
}

//==================================================================================================
// DriverStation::isPractice
//==================================================================================================

bool DriverStation::isPractice() {
    return !m_practiceInterrupted && m_practiceRunning;
}

//==================================================================================================
// DriverStation::isTest
//==================================================================================================

bool DriverStation::isTest() {
    return controlMode() == DS::kControlTest;
}

//==================================================================================================
// DriverStation::isAutonomous
//==================================================================================================

bool DriverStation::isAutonomous() {
    return controlMode() == DS::kControlAutonomous;
}

//==================================================================================================
// DriverStation::isTeleoperated
//==================================================================================================

bool DriverStation::isTeleoperated() {
    return controlMode() == DS::kControlTeleoperated;
}

//==================================================================================================
// DriverStation::isEmergencyStopped
//==================================================================================================

bool DriverStation::isEmergencyStopped() {
    if (isConnected())
        return currentProtocol()->isEmergencyStopped();

    return false;
}

//==================================================================================================
// DriverStation::isConnected
//==================================================================================================

bool DriverStation::isConnected() {
    if (protocolLoaded())
        return currentProtocol()->isConnectedToRobot();

    return false;
}

//==================================================================================================
// DriverStation::joystickCount
//==================================================================================================

int DriverStation::joystickCount() {
    return joysticks()->count();
}

//==================================================================================================
// DriverStation::team
//==================================================================================================

int DriverStation::team() {
    if (protocolLoaded())
        return currentProtocol()->team();

    return 0;
}

//==================================================================================================
// DriverStation::acceptsConsoleCommands
//==================================================================================================

bool DriverStation::acceptsConsoleCommands() {
    if (protocolLoaded())
        return currentProtocol()->acceptsConsoleCommands();

    return false;
}

//==================================================================================================
// DriverStation::init
//==================================================================================================

void DriverStation::init() {
    if (!m_init) {
        m_init = true;

        QTimer::singleShot (100, this, SIGNAL (initialized()));
        QTimer::singleShot (100, this,   SLOT (resetEverything()));

        DS::log (DS::kLibLevel, "DS Initialized");
    }
}

//==================================================================================================
// DriverStation::rebootRobot
//==================================================================================================

void DriverStation::rebootRobot() {
    if (protocolLoaded())
        currentProtocol()->reboot();
}

//==================================================================================================
// DriverStation::restartCode
//==================================================================================================

void DriverStation::restartCode() {
    if (protocolLoaded())
        currentProtocol()->restartCode();
}

//==================================================================================================
// DriverStation::enableRobot
//==================================================================================================

void DriverStation::enableRobot() {
    setEnabled (true);
}

//==================================================================================================
// DriverStation::disableRobot
//==================================================================================================

void DriverStation::disableRobot() {
    setEnabled (false);
}

//==================================================================================================
// DriverStation::setEnabled
//==================================================================================================

void DriverStation::setEnabled (bool enabled) {
    if (protocolLoaded()) {
        currentProtocol()->setEnabled (enabled);

        if (enabled)
            m_elapsedTime->resetTimer();

        else {
            m_practiceRunning = false;
            m_practiceInterrupted = true;

            m_elapsedTime->stopTimer();
        }
    }
}

//==================================================================================================
// DriverStation::sendConsoleCommand
//==================================================================================================

void DriverStation::sendCommand (QString command) {
    m_netConsole->sendCommand (command);
}

//==================================================================================================
// DriverStation::setPSC
//==================================================================================================

void DriverStation::setPSC (int limit) {
    if (protocolLoaded())
        currentProtocol()->setPSC (limit);
}

//==================================================================================================
// DriverStation::setEmergencyStop
//==================================================================================================

void DriverStation::setEmergencyStop (bool stop) {
    if (protocolLoaded())
        currentProtocol()->setEmergencyStop (stop);
}

//==================================================================================================
// DriverStation::startTest
//==================================================================================================

void DriverStation::startTest (bool enabled) {
    setControlMode (DS::kControlTest);
    setEnabled (enabled);
}

//==================================================================================================
// DriverStation::startAutonomous
//==================================================================================================

void DriverStation::startAutonomous (bool enabled) {
    setControlMode (DS::kControlAutonomous);
    setEnabled (enabled);
}

//==================================================================================================
// DriverStation::startTeleoperated
//==================================================================================================

void DriverStation::startTeleoperated (bool enabled) {
    setControlMode (DS::kControlTeleoperated);
    setEnabled (enabled);
}

//==================================================================================================
// DriverStation::startPractice
//==================================================================================================

void DriverStation::startPractice (int countdown, int autonomous, int delay, int teleop,
                                   int endgame) {
    /* This can be used by the client to update the status text */
    emit robotStatusChanged (tr ("Practice Match"));

    /* A practice session is already under progress */
    if (isPractice())
        return;

    /* Allow the client to switch modes when the timers expire */
    else {
        m_practiceRunning = true;
        m_practiceInterrupted = false;
    }

    /* Transform the times from seconds to milliseconds */
    delay *= 1000;
    teleop *= 1000;
    endgame *= 1000;
    countdown *= 1000;
    autonomous *= 1000;

    /* Calculate the time frames */
    quint32 _startAutonomous = countdown;
    quint32 _startTeleoperated = _startAutonomous + delay + autonomous;
    quint32 _startEndGame = _startTeleoperated + delay + teleop - endgame;
    quint32 _stop = _startEndGame + endgame;

    /* Configure the sound/media timers */
    QTimer::singleShot (_startAutonomous,   Qt::PreciseTimer, this, SLOT (playAutonomousStart()));
    QTimer::singleShot (_startTeleoperated, Qt::PreciseTimer, this, SLOT (playTeleopStart()));
    QTimer::singleShot (_stop,              Qt::PreciseTimer, this, SLOT (playEndGame()));
    QTimer::singleShot (_startEndGame,      Qt::PreciseTimer, this, SLOT (playEndGameApproaching()));
}

//==================================================================================================
// DriverStation::setTeamNumber
//==================================================================================================

void DriverStation::setTeamNumber (int team) {
    if (protocolLoaded())
        currentProtocol()->setTeam (team);

    emit teamChanged (team);
}

//==================================================================================================
// DriverStation::setAlliance
//==================================================================================================

void DriverStation::setAlliance (int alliance) {
    if (protocolLoaded())
        currentProtocol()->setAlliance ((DS::Alliance) alliance);
}


//==================================================================================================
// DriverStation::setProtocol
//==================================================================================================

void DriverStation::setProtocol (DS_Core::AbstractProtocol* protocol) {
    if (protocol == Q_NULLPTR) {
        DS::log (DS::kWarnLevel, "Cannot load NULL protocol");
        return;
    }

    /* Disable the current protocol and pass settings from one protocol to another */
    if (protocolLoaded()) {
        protocol->setTeam (currentProtocol()->team());
        protocol->setRobotAddress (robotAddress());

        currentProtocol()->stop();
        currentProtocol()->disconnect();
    }

    /* Re-assign protocol */
    m_protocol = protocol;

    /* Re-connect protocol signals/slots */
    connect (currentProtocol(), &DS_Core::AbstractProtocol::emergencyStopped,
             this,              &DriverStation::emergencyStopped);
    connect (currentProtocol(), &DS_Core::AbstractProtocol::codeChanged,
             this,              &DriverStation::codeChanged);
    connect (currentProtocol(), &DS_Core::AbstractProtocol::radioCommChanged,
             this,              &DriverStation::radioChanged);
    connect (currentProtocol(), &DS_Core::AbstractProtocol::communicationsChanged,
             this,              &DriverStation::communicationsChanged);
    connect (currentProtocol(), &DS_Core::AbstractProtocol::controlModeChanged,
             this,              &DriverStation::controlModeChanged);
    connect (currentProtocol(), &DS_Core::AbstractProtocol::diskUsageChanged,
             this,              &DriverStation::diskUsageChanged);
    connect (currentProtocol(), &DS_Core::AbstractProtocol::ramUsageChanged,
             this,              &DriverStation::ramUsageChanged);
    connect (currentProtocol(), &DS_Core::AbstractProtocol::cpuUsageChanged,
             this,              &DriverStation::cpuUsageChanged);
    connect (currentProtocol(), &DS_Core::AbstractProtocol::voltageChanged,
             this,              &DriverStation::voltageChanged);
    connect (currentProtocol(), &DS_Core::AbstractProtocol::voltageBrownoutChanged,
             this,              &DriverStation::voltageBrownoutChanged);
    connect (currentProtocol(), &DS_Core::AbstractProtocol::CANInfoReceived,
             this,              &DriverStation::CANInfoReceived);
    connect (currentProtocol(), &DS_Core::AbstractProtocol::fmsChanged,
             this,              &DriverStation::fmsChanged);
    connect (currentProtocol(), &DS_Core::AbstractProtocol::enabledChanged,
             this,              &DriverStation::enabledChanged);

    /* Start the protocol & re-configure the library modules */
    m_netConsole->setBindFlags    (currentProtocol()->ncBindFlags());
    m_netConsole->setInputPort    (currentProtocol()->netConsoleInputPort());
    m_netConsole->setOutputPort   (currentProtocol()->netConsoleOutputPort());
    m_netConsole->setAcceptsInput (currentProtocol()->acceptsConsoleCommands());

    emit protocolChanged();
}

//==================================================================================================
// DriverStation::setProtocol
//==================================================================================================

void DriverStation::setProtocol (int protocol) {
    if (protocol == kFRCProtocol2016)
        setProtocol (GET_INSTANCE<DS_Protocols::FRC_Protocol2016>());

    if (protocol == kFRCProtocol2015)
        setProtocol (GET_INSTANCE<DS_Protocols::FRC_Protocol2015>());

    if (protocol == kFRCProtocol2014)
        setProtocol (GET_INSTANCE<DS_Protocols::FRC_Protocol2014>());
}

//==================================================================================================
// DriverStation::setControlMode
//==================================================================================================

void DriverStation::setControlMode (int mode) {
    if (protocolLoaded())
        currentProtocol()->setControlMode ((DS::ControlMode) mode);
}

//==================================================================================================
// DriverStation::setRobotAddress
//==================================================================================================

void DriverStation::setRobotAddress (QString address) {
    if (protocolLoaded())
        currentProtocol()->setRobotAddress (address);
}

//==================================================================================================
// DriverStation::resetJoysticks
//==================================================================================================

void DriverStation::resetJoysticks() {
    joysticks()->clear();

    if (protocolLoaded())
        currentProtocol()->onJoysticksChanged();

    emit joystickCountChanged();
}

//==================================================================================================
// DriverStation::updateJoystickPOV
//==================================================================================================

void DriverStation::updateJoystickPOV (int js, int hat, int angle) {
    if (js < joysticks()->count() && js >= 0 && hat >= 0)
        joysticks()->at (js).POVs [hat] = angle;
}

//==================================================================================================
// DriverStation::addJoystick
//==================================================================================================

bool DriverStation::addJoystick (int axes, int buttons, int POVs) {
    if (axes <= 0 && buttons <= 0 && POVs <= 0)
        return false;

    /* The DS has exceeded the supported number of joysticks */
    if (joysticks()->count() >= MAX_JOYSTICKS) {
        DS::log  (DS::kLibLevel, "Reached maximum number of joysticks");
        DS::log  (DS::kLibLevel, "Ignoring future joysticks");
        DS::sendMessage (ERR_JOYSTICKS.arg (MAX_JOYSTICKS));
        return false;
    }

    /* The input joysticks has too many POVs */
    else if (POVs > MAX_POVS) {
        DS::log (DS::kLibLevel, "Input device has exceeded POV range");
        DS::sendMessage (ERR_POV_CNT
                         .arg (joysticks()->count())
                         .arg (MAX_POVS)
                         .arg (POVs));
        return false;
    }

    /* The input joysticks has too many axes */
    else if (axes > MAX_AXES) {
        DS::log  (DS::kLibLevel, "Input device has exceeded axis range");
        DS::sendMessage (ERR_AXIS_CNT
                         .arg (joysticks()->count())
                         .arg (MAX_AXES)
                         .arg (axes));
        return false;
    }

    /* The input joysticks has too many buttons */
    else if (buttons > MAX_BUTTONS) {
        DS::log  (DS::kLibLevel, "Input device has exceeded button range");
        DS::sendMessage (ERR_AXIS_CNT
                         .arg (joysticks()->count())
                         .arg (MAX_BUTTONS)
                         .arg (buttons));
        return false;
    }

    /* Everything OK, register the joystick */
    else {
        DS::Joystick js;

        js.numAxes = axes;
        js.numPOVs = POVs;
        js.numButtons = buttons;

        js.axes = new float  [axes];
        js.POVs = new int  [POVs];
        js.buttons = new bool [buttons];

        for (int i = 0; i < js.numAxes; i++)
            js.axes [i] = 0;

        for (int i = 0; i < js.numPOVs; i++)
            js.POVs [i] = -1;

        for (int i = 0; i < js.numButtons; i++)
            js.buttons [i] = false;

        joysticks()->append (js);

        DS::log (DS::kLibLevel, QString ("Registered joystick with: "
                                         "%1 axes, "
                                         "%2 buttons and "
                                         "%3 POVs")
                 .arg (QString::number (js.numAxes))
                 .arg (QString::number (js.numButtons))
                 .arg (QString::number (js.numPOVs)));

        DS::log (DS::kLibLevel, QString ("Current number of joysticks is %1")
                 .arg (QString::number (joysticks()->count())));
    }

    if (protocolLoaded())
        currentProtocol()->onJoysticksChanged();

    emit joystickCountChanged();
    return true;
}

//==================================================================================================
// DriverStation::updateJoystickAxis
//==================================================================================================

void DriverStation::updateJoystickAxis (int js, int axis, float value) {
    if (js < joysticks()->count() && js >= 0 && axis >= 0)
        joysticks()->at (js).axes [axis] = value;
}

//==================================================================================================
// DriverStation::updateJoystickButton
//==================================================================================================

void DriverStation::updateJoystickButton (int js, int button, bool pressed) {
    if (js < joysticks()->count() && js >= 0 && button >= 0)
        joysticks()->at (js).buttons [button] = pressed;
}

//==================================================================================================
// DriverStation::getRobotStatus
//==================================================================================================

QString DriverStation::getRobotStatus() {
    if (protocolLoaded() && currentProtocol()->isConnectedToRobot()) {
        if (currentProtocol()->isEmergencyStopped())
            return tr ("Emergency Stopped");

        if (!currentProtocol()->hasCode())
            return tr ("No Robot Code");

        return QString ("%1 %2")
               .arg (DS::controlModeString (controlMode()))
               .arg (isEnabled() ? tr ("Enabled") : tr ("Disabled"));
    }

    return tr ("No Robot Communication");
}

//==================================================================================================
// DriverStation::resetEverything
//==================================================================================================

void DriverStation::resetEverything() {
    m_elapsedTime->resetTimer();
    m_elapsedTime->stopTimer();

    if (isPractice()) {
        m_practiceRunning = false;
        m_practiceInterrupted = true;
    }

    emit codeChanged           (false);
    emit elapsedTimeChanged    ("00:00.0");
    emit communicationsChanged (DS::kFailing);
    emit robotStatusChanged    (getRobotStatus());

    DS::log (DS::kLibLevel, "Reseting to initial values...");
}

//==================================================================================================
// DriverStation::resetElapsedTimer
//==================================================================================================

void DriverStation::resetElapsedTimer (DS::ControlMode mode) {
    Q_UNUSED (mode);
    emit robotStatusChanged (getRobotStatus());
}

//==================================================================================================
// DriverStation::playEndGameApproaching
//==================================================================================================

void DriverStation::playEndGameApproaching() {
    if (isPractice())
        QSound::play (":/LibDS/Start of End Game_normalized.wav");
}

//==================================================================================================
// DriverStation::playEndGame
//==================================================================================================

void DriverStation::playEndGame() {
    if (isPractice()) {
        setEnabled (false);
        QSound::play (":/LibDS/Match End_normalized.wav");
    }
}

//==================================================================================================
// DriverStation::playTeleopStart
//==================================================================================================

void DriverStation::playTeleopStart() {
    if (isPractice()) {
        setControlMode (DS::kControlTeleoperated);
        QSound::play (":/LibDS/Start Teleop_normalized.wav");

        setEnabled (true);
        emit robotStatusChanged (tr ("Teleoperated Practice"));
    }
}

//==================================================================================================
// DriverStation::playAutonomousStart
//==================================================================================================

void DriverStation::playAutonomousStart() {
    if (isPractice()) {
        setControlMode (DS::kControlAutonomous);
        QSound::play (":/LibDS/Start Auto_normalized.wav");

        setEnabled (true);
        emit robotStatusChanged (tr ("Autonomous Practice"));
    }
}
