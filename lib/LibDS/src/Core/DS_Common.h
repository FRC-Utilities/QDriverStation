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
#include <QObject>
#include <QDateTime>
#include <QStringList>

#define DISABLED_PORT 0
#define JoystickList QList<DS::Joystick*>

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
        kControlTest         = 0,
        kControlAutonomous   = 1,
        kControlTeleoperated = 2,
    };

    /**
     * @brief The Alliance enum
     * Represents the team alliance.
     * The robot program may use this information to change its behavior
     * (e.g adjust its sensors, change dashboard values, etc.)
     */
    enum Alliance {
        kAllianceRed  = 0,
        kAllianceBlue = 1,
    };

    /**
     * @brief The Position enum
     * Represents the different team positions for each alliance.
     * Again, the robot program may use this data to change its behavior.
     */
    enum Position {
        kPosition1 = 0,
        kPosition2 = 1,
        kPosition3 = 2,
    };

    /**
     * @brief The CommStatus enum
     * Represents the communication status between the client and the robot or
     * the client and the radio.
     */
    enum CommStatus {
        kCommsWorking = 0,
        kCommsFailing = 1,
    };

    /**
     * @brief The CodeStatus enum
     * Represents the status of the robot code. The DS is only interested if
     * the code is running or not (in order to be able to operate the robot).
     */
    enum CodeStatus {
        kCodeRunning = 0,
        kCodeFailing = 1,
    };

    /**
     * @brief The EnableStatus enum
     * Represents the enabled state of the robot.
     */
    enum EnableStatus {
        kEnabled  = 0,
        kDisabled = 1,
    };

    /**
     * @brief the OperationStatus enum
     * Represents the operation status of the robot.
     */
    enum OperationStatus {
        kOperationNormal        = 0,
        kOperationEmergencyStop = 1,
    };

    /**
     * @brief the VoltageStatus enum
     * Represents the status of the robot power supply.
     * When the battery level is low or there are too many power spikes,
     * the robot may report a voltage brownout.
     */
    enum VoltageStatus {
        kVoltageNormal   = 0,
        kVoltageBrownout = 1,
    };

    /**
     * Represents the socket types that can be used by the DS modules
     */
    enum SocketType {
        kSocketTypeUDP = 0,
        kSocketTypeTCP = 1,
    };

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
    static QString getStaticIP (int net, int team, int host)
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
    static QString timezone()
    {
        switch (QDateTime::currentDateTime().offsetFromUtc() / 3600) {
        case -11:
            return "BST11BDT";
            break;
        case -10:
            return "HST10HDT";
            break;
        case -9:
            return "AST9ADT";
            break;
        case -8:
            return "PST8PDT";
            break;
        case -7:
            return "MST7MDT";
            break;
        case -6:
            return "CST6CDT";
            break;
        case -5:
            return "EST5EDT";
            break;
        case -4:
            return "AST4ADT";
            break;
        case -3:
            return "GRNLNDST3GRNLNDDT";
            break;
        case -2:
            return "FALKST2FALKDT";
            break;
        case -1:
            return "AZOREST1AZOREDT";
            break;
        case 0:
            return "GMT0BST";
            break;
        case 1:
            return "NFT-1DFT";
            break;
        case 2:
            return "WET-2WET";
            break;
        case 3:
            return "MEST-3MEDT";
            break;
        case 4:
            return "WST-4WDT";
            break;
        case 5:
            return "PAKST-5PAKDT";
            break;
        case 6:
            return "TASHST-6TASHDT";
            break;
        case 7:
            return "THAIST-7THAIDT";
            break;
        case 8:
            return "WAUST-8WAUDT";
            break;
        case 9:
            return "JST-9JSTDT";
            break;
        case 10:
            return "EET-10EETDT";
            break;
        case 11:
            return "MET-11METDT";
            break;
        case 12:
            return "NZST-12NZDT";
            break;
        }

        return "GMT0BST";
    }
};

#endif
