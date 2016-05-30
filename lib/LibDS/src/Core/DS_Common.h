/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#ifndef _LIB_DS_COMMON_H
#define _LIB_DS_COMMON_H

#include <QtMath>
#include <QDebug>
#include <QObject>
#include <QDateTime>
#include <QStringList>

#define DISABLED_PORT 0
#define JoystickList QList<DS::Joystick*>

/**
 * Returns the location in where application files (e.g. logs) are stored
 */
extern QString DS_FILES_PATH();

/**
 * Returns the location in where the log files are stored
 */
extern QString DS_LOGGER_PATH();

/**
 * Returns the localtion in which robot/protocol log files are stored
 */
extern QString DS_ROBOT_LOGGER_PATH();

/**
 * This function implements our own logger engine. Feel free to enable
 * it if you want!
 *
 * To enable it, write this in your \c main() function:
 * \c qInstallMessageHandler (DS_MESSAGE_HANDLER)
 */
extern void DS_MESSAGE_HANDLER (QtMsgType type,
                                const QMessageLogContext& context,
                                const QString& data);

/**
 * Holds the common data types shared between the \c DriverStation
 * and the \c Protocol classes of this library. This data must be registered
 * inside a QObject class in order to allow full support for QML applications.
 */
class DS : public QObject
{
    Q_OBJECT
    Q_ENUMS (Alliance)
    Q_ENUMS (Position)
    Q_ENUMS (SocketType)
    Q_ENUMS (CodeStatus)
    Q_ENUMS (CommStatus)
    Q_ENUMS (ControlMode)
    Q_ENUMS (EnableStatus)
    Q_ENUMS (OperationStatus)

public:
    /**
     * @brief The ControlMode enum
     * Represents the available control modes of the robot
     */
    enum ControlMode {
        kControlTest,
        kControlAutonomous,
        kControlTeleoperated,
    };
    Q_ENUM (ControlMode)

    /**
     * @brief The Alliance enum
     * Represents the team alliance.
     * The robot program may use this information to change its behavior
     * (e.g adjust its sensors, change dashboard values, etc.)
     */
    enum Alliance {
        kAllianceRed,
        kAllianceBlue,
    };
    Q_ENUM (Alliance)

    /**
     * @brief The Position enum
     * Represents the different team positions for each alliance.
     * Again, the robot program may use this data to change its behavior.
     */
    enum Position {
        kPosition1,
        kPosition2,
        kPosition3,
    };
    Q_ENUM (Position)

    /**
     * @brief The CommStatus enum
     * Represents the communication status between the client and the robot or
     * the client and the radio.
     */
    enum CommStatus {
        kCommsWorking,
        kCommsFailing,
    };
    Q_ENUM (CommStatus)

    /**
     * @brief The CodeStatus enum
     * Represents the status of the robot code. The DS is only interested if
     * the code is running or not (in order to be able to operate the robot).
     */
    enum CodeStatus {
        kCodeRunning,
        kCodeFailing,
    };
    Q_ENUM (CodeStatus)

    /**
     * @brief The EnableStatus enum
     * Represents the enabled state of the robot.
     */
    enum EnableStatus {
        kEnabled,
        kDisabled,
    };
    Q_ENUM (EnableStatus)

    /**
     * @brief the OperationStatus enum
     * Represents the operation status of the robot.
     */
    enum OperationStatus {
        kOperationNormal,
        kOperationEmergencyStop,
    };
    Q_ENUM (OperationStatus)

    /**
     * @brief the VoltageStatus enum
     * Represents the status of the robot power supply.
     * When the battery level is low or there are too many power spikes,
     * the robot may report a voltage brownout.
     */
    enum VoltageStatus {
        kVoltageNormal,
        kVoltageBrownout,
    };
    Q_ENUM (VoltageStatus)

    /**
     * Represents the socket types that can be used by the DS modules
     */
    enum SocketType {
        kSocketTypeUDP,
        kSocketTypeTCP,
    };
    Q_ENUM (SocketType)

    /**
     * Represents a joystick and its respective properties
     */
    struct Joystick {
        int numAxes = 0;
        int numPOVs = 0;
        int numButtons = 0;
        int realNumAxes = 0;
        int realNumPOVs = 0;
        int realNumButtons = 0;

        int* povs;
        float* axes;
        bool* buttons;
    };

    /**
     * Returns a calculated IP address based on the team address.
     *
     * For example:
     *    - \c getStaticIp ( 10, 3794, 1) would return \c 10.37.94.1
     *    - \c getStaticIp (177, 3794, 2) would return \c 177.37.94.2
     *    - \c getStaticIp ( 10,  118, 3) would return \c 10.01.18.3
     *    - And so on...
     */
    static inline QString getStaticIP (int net, int team, int host)
    {
        QString string = QString ("%1").arg (team);
        switch (string.length()) {
        case 1:
            string = "00.0" + string;
            break;
        case 2:
            string = "00." + string;
            break;
        case 3:
            string = QString ("0%1.%2%3").arg (string.at (0),
                                               string.at (1),
                                               string.at (2));
            break;
        case 4:
            string = QString ("%1%2.%3%4").arg (string.at (0),
                                                string.at (1),
                                                string.at (2),
                                                string.at (3));
            break;
        }
        return QString ("%1.%2.%3").arg (QString::number (net),
                                         string,
                                         QString::number (host));
    }

    /**
     * Returns the current timezone as a string
     */
    static inline QString timezone()
    {
        return QDateTime::currentDateTime().toString ("t");
    }
};

Q_DECLARE_METATYPE (DS::Alliance)
Q_DECLARE_METATYPE (DS::Position)
Q_DECLARE_METATYPE (DS::SocketType)
Q_DECLARE_METATYPE (DS::CodeStatus)
Q_DECLARE_METATYPE (DS::CommStatus)
Q_DECLARE_METATYPE (DS::ControlMode)
Q_DECLARE_METATYPE (DS::EnableStatus)
Q_DECLARE_METATYPE (DS::OperationStatus)

#endif
