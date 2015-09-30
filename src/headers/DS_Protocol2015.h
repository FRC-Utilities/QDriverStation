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

#include "DS_Debug.h"
#include "DS_Protocol.h"

#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>

/**
 * Encapsulates the section headers that allow the robot to indetify the
 * data that we send to it
 */
namespace SectionHeaders
{
enum SectionHeaders {
    GeneralHeader = 0x01,
    JoystickHeader = 0x0c
};
}

/**
 * Encapsulates the constants of the operatio modes of the robot
 */
namespace OperationModes
{
enum Modes {
    Test = 0x05,
    Disabled = 0x00,
    Autonomous = 0x06,
    TeleOperated = 0x04,
    EmergencyStop = 0x80
};
}

/**
 * Encapsulates the constants that are used to represent the alliance and
 * position that the robot shall use
 */
namespace Alliances
{
enum Alliances {
    Red1 = 0x00,
    Red2 = 0x01,
    Red3 = 0x02,
    Blue1 = 0x03,
    Blue2 = 0x04,
    Blue3 = 0x05
};
}

/**
 * Encapsulates the constants of the robot status mode
 */
namespace RobotStatus
{
enum Status {
    Normal = 0x10,      /**< The robot will behave normally */
    RebootRobot = 0x18, /**< The roboRIO will reboot itself */
    RestartCode = 0x14  /**< The robot will restart the user code */
};
}

/**
 * Represents the ports used to communicate between the robot
 * and the Driver Station
 */
namespace Ports
{
enum Ports {
    RobotPort = 1110, /**< The port in which we send instructions to the robot */
    ClientPort = 1150 /**< The port in which we receive robot data */
};
}

/**
 * Encapsulates the possible codes that the roboRIO can emit regarding the
 * status of the robot program
 */
namespace ProgramStatus
{
enum ProgramStatus {
    Test = 0x08,
    TeleOp = 0x02,
    RoboRio = 0x10,
    Disabled = 0x01,
    UserCode = 0x20,
    NoProgram = 0x00,
    Autonomous = 0x04,
};
}

/**
 * Represents the location of the available data in a robot packet
 */
namespace RobotData
{
enum PacketOrder {
    Pong1 = 0x00,
    Pong2 = 0x01,
    ControlEcho = 0x03,
    RobotStatus = 0x04,
    VoltageMajor = 0x05,
    VoltageMinor = 0x06
};
}

/**
 * \class DS_Protocol2015
 * Implements the 2015 communication protocol
 */
class DS_Protocol2015 : public DS_Protocol
{
    Q_OBJECT

public:
    explicit DS_Protocol2015();

public slots:
    void reset();
    void reboot();
    int robotPort();
    int clientPort();
    void restartCode();
    QString robotAddress();
    QString radioAddress();
    void downloadRobotInformation();
    QByteArray generateClientPacket();
    QByteArray generateJoystickData();
    void readRobotData (QByteArray data);
    char getControlCode (DS_ControlMode mode);
    char getAllianceCode (DS_Alliance alliance);

private:
    int m_index;
    int m_status;
    bool m_justConnected;
    QNetworkAccessManager m_manager;

private slots:
    int getJoystickSize (DS_Joystick* joystick);
    void onDownloadFinished (QNetworkReply* reply);
};
