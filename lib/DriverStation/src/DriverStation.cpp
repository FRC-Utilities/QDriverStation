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

#include <QTimer>
#include <DriverStation.h>

#include "Times.h"
#include "Packets.h"
#include "VersionAnalyzer.h"
#include "NetworkDiagnostics.h"

DriverStation* DriverStation::m_instance = nullptr;

DriverStation::DriverStation()
{
    m_code = false;
    m_init = false;
    m_oldConnection = false;
    m_justConnected = false;
    m_justDisconnected = false;

    m_status = DS_Ok;
    m_alliance = DS_Red1;
    m_controlMode = DS_Disabled;

    m_versionAnalyzer = new DS_VersionAnalyzer();
    m_netDiagnostics = new DS_NetworkDiagnostics();

    connect (m_versionAnalyzer, SIGNAL (libVersionChanged (QString)),
             this,              SIGNAL (libVersionChanged (QString)));
    connect (m_versionAnalyzer, SIGNAL (pcmVersionChanged (QString)),
             this,              SIGNAL (pcmVersionChanged (QString)));
    connect (m_versionAnalyzer, SIGNAL (pdpVersionChanged (QString)),
             this,              SIGNAL (pdpVersionChanged (QString)));
    connect (m_versionAnalyzer, SIGNAL (rioVersionChanged (QString)),
             this,              SIGNAL (rioVersionChanged (QString)));
}

DriverStation* DriverStation::getInstance()
{
    if (m_instance == nullptr)
        m_instance = new DriverStation();

    return m_instance;
}

bool DriverStation::canBeEnabled()
{
    return m_netDiagnostics->roboRioIsAlive() && m_code;
}

QStringList DriverStation::alliances()
{
    QStringList list;

    list.append ("Red 1");
    list.append ("Red 2");
    list.append ("Red 3");
    list.append ("Blue 1");
    list.append ("Blue 2");
    list.append ("Blue 3");

    return list;
}

DS_ControlMode DriverStation::operationMode()
{
    return m_controlMode;
}

NetConsole* DriverStation::netConsole()
{
    return NetConsole::getInstance();
}

QString DriverStation::roboRioAddress()
{
    return m_netDiagnostics->roboRioIpAddress();
}

QString DriverStation::radioAddress()
{
    return m_netDiagnostics->radioIpAddress();
}

void DriverStation::init()
{
    if (!m_init) {
        /* Send signals to force object update */
        emit codeChanged (false);
        emit networkChanged (false);
        emit voltageChanged (0.000);
        emit libVersionChanged ("");
        emit rioVersionChanged ("");
        emit pdpVersionChanged ("");
        emit ramUsageChanged (0, 0);
        emit diskUsageChanged (0, 0);

        /* Begin DS/Communication loop */
        checkConnection();
        sendPacketsToRobot();

        /* Start the elapsed time loop */
        updateElapsedTime();
        emit elapsedTimeChanged ("00:00.0");

        /* Initialize the NetConsole */
        netConsole()->init();

        /* Ensure that this code will only run once */
        m_init = true;
    }
}

void DriverStation::reboot()
{

}

void DriverStation::restartCode()
{

}

void DriverStation::setTeamNumber (int team)
{
    m_netDiagnostics->setTeamNumber (team);
}

void DriverStation::setAlliance (DS_Alliance alliance)
{
    m_alliance = alliance;
}

void DriverStation::setCustomAddress (QString address)
{
    m_netDiagnostics->setCustomAddress (address);
}

void DriverStation::setControlMode (DS_ControlMode mode)
{
    if (m_controlMode != mode && mode != DS_Disabled)
        m_time.restart();

    m_controlMode = mode;
}

void DriverStation::putJoystickData (DS_JoystickData joystickData)
{
    Q_UNUSED (joystickData);
}

void DriverStation::startPractice (int countdown,
                                   int autonomous,
                                   int delay,
                                   int teleop,
                                   int endgame)
{
    Q_UNUSED (countdown);
    Q_UNUSED (autonomous);
    Q_UNUSED (delay);
    Q_UNUSED (teleop);
    Q_UNUSED (endgame);
}

QString DriverStation::getStatus()
{
    if (!m_netDiagnostics->roboRioIsAlive())
        return tr ("No Robot Communication");

    else if (!m_code)
        return tr ("No Robot Code");

    return DS_GetControlModeString (m_controlMode);
}

void DriverStation::checkConnection()
{
    m_netDiagnostics->refresh();

    m_justConnected = m_netDiagnostics->roboRioIsAlive() && !m_oldConnection;
    m_justDisconnected = !m_netDiagnostics->roboRioIsAlive() && m_oldConnection;

    if (m_justDisconnected) {
        m_code = false;
        emit codeChanged (m_code);
        emit networkChanged (false);
    }

    else if (m_justConnected) {
        m_code = !true;
        emit codeChanged (m_code);
        emit networkChanged (true);
        m_versionAnalyzer->downloadRobotInformation (roboRioAddress());
    }

    m_oldConnection = m_netDiagnostics->roboRioIsAlive();

    if (m_radioStatus != m_netDiagnostics->robotRadioIsAlive()) {
        m_radioStatus = m_netDiagnostics->robotRadioIsAlive();

        emit radioChanged (m_radioStatus);
    }

    QTimer::singleShot (Times::TestConnectionInterval,
                        this, SLOT (checkConnection()));
}

void DriverStation::sendPacketsToRobot()
{
    emit robotStatusChanged (getStatus());

    if (m_netDiagnostics->roboRioIsAlive()) {
        DS_SendCommonControlPacket (m_status,
                                    m_alliance,
                                    m_controlMode,
                                    roboRioAddress());
    }

    QTimer::singleShot (Times::ControlPacketInterval,
                        this, SLOT (sendPacketsToRobot()));
}

void DriverStation::updateElapsedTime()
{
    /* We are a bunch of lazy motherfuckers, and we only work when required */
    if (operationMode() != DS_Disabled) {

        /* Get milliseconds, seconds and minutes */
        int time = m_time.elapsed();
        int nSeconds = time / 1000;
        int nMinutes = nSeconds / 60;

        /* Avoid having "01:72.0" instead of "01:12.0" */
        while (nSeconds >= 60)
            nSeconds -= 60;

        /* Create strings */
        QString seconds = QString ("%1").arg (nSeconds);
        QString minutes = QString ("%1").arg (nMinutes);
        QString milliseconds = QString ("%1").arg (time);

        /* Prepend 0 if seconds are less than 10 */
        if (nSeconds < 10)
            seconds = "0" + seconds;

        /* Prepend 0 if minutes are less than 10 */
        if (nMinutes < 10)
            minutes = "0" + minutes;

        /* Notify other objects that the elapsed time has changed */
        emit elapsedTimeChanged (QString ("%1:%2.%3")
                                 .arg (minutes)
                                 .arg (seconds)
                                 .arg (milliseconds.at (
                                           milliseconds.length() - 1)));
    }

    QTimer::singleShot (Times::ElapsedTimeInterval,
                        this, SLOT (updateElapsedTime()));
}
