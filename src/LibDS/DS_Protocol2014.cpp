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

#include "LibDS/DS_Protocol2014.h"

#define CONTROL_DISABLED 0x40
#define CONTROL_NO_COMM  0x04
#define CONTROL_TELEOP   0x60
#define CONTROL_TEST     0x62
#define CONTROL_AUTO     0x70
#define CONTROL_ESTOP    0x00
#define STATUS_REBOOT    0x80
#define ALLIANCE_RED1    0x36
#define PORT_ROBOT       1110
#define PORT_CLIENT      1150

DS_Protocol2014::DS_Protocol2014() {
    reset();
}

void DS_Protocol2014::reset() {
    /* TODO */

    p_robotCode = false;
    p_robotCommunication = false;

    /* Notify other objects */
    emit codeChanged (p_robotCode);
    emit voltageChanged (QString (""));
    emit communicationsChanged (p_robotCommunication);

    /* Disable the robot, avoid bad surprises */
    setControlMode (DS_ControlNoCommunication);
}

void DS_Protocol2014::reboot() {
    /* TODO */
}

int DS_Protocol2014::robotPort() {
    return PORT_ROBOT;
}

int DS_Protocol2014::clientPort() {
    return PORT_CLIENT;
}

void DS_Protocol2014::restartCode() {
    /* TODO */
}

QString DS_Protocol2014::robotAddress() {
    if (p_robotAddress.isEmpty())
        return DS_GetStaticIp (p_team, 2);

    return p_robotAddress;
}

QString DS_Protocol2014::radioAddress() {
    if (p_radioAddress.isEmpty())
        return DS_GetStaticIp (p_team, 1);

    return p_radioAddress;
}

void DS_Protocol2014::downloadRobotInformation() {

}

QByteArray DS_Protocol2014::generateClientPacket() {
    QByteArray data;

    /* TODO */

    return data;
}

QByteArray DS_Protocol2014::generateJoystickData() {
    QByteArray data;

    /* TODO */

    return data;
}

void DS_Protocol2014::readRobotData (QByteArray data) {
    Q_UNUSED (data);

    /* TODO */

    emit packetReceived();
}

char DS_Protocol2014::getControlCode (DS_ControlMode mode) {
    switch (mode) {
    case DS_ControlTest:
        return CONTROL_TEST;
        break;
    case DS_ControlTeleOp:
        return CONTROL_TELEOP;
        break;
    case DS_ControlDisabled:
        return CONTROL_DISABLED;
        break;
    case DS_ControlAutonomous:
        return CONTROL_AUTO;
        break;
    case DS_ControlEmergencyStop:
        return CONTROL_ESTOP;
    case DS_ControlNoCommunication:
        return CONTROL_NO_COMM;
        break;
    }

    return CONTROL_DISABLED;
}

DS_ControlMode DS_Protocol2014::getControlMode (char byte) {
    switch (byte) {
    case CONTROL_DISABLED:
        return DS_ControlDisabled;
        break;
    case CONTROL_TELEOP:
        return DS_ControlTeleOp;
        break;
    case CONTROL_TEST:
        return DS_ControlTest;
        break;
    case CONTROL_AUTO:
        return DS_ControlAutonomous;
    case CONTROL_NO_COMM:
        return DS_ControlNoCommunication;
        break;
    }

    return DS_ControlEmergencyStop;
}

char DS_Protocol2014::getAllianceCode (DS_Alliance alliance) {
    Q_UNUSED (alliance);

    /* TODO */

    return ALLIANCE_RED1;
}
