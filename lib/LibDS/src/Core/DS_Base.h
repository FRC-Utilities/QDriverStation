/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#ifndef _LIB_DS_DRIVERSTATION_BASE_H
#define _LIB_DS_DRIVERSTATION_BASE_H

#include <Core/DS_Common.h>

/**
 * \brief Defines the base signals of the \c DriverStation and \c DS_Config
 *        classes
 */
class DS_Base : public DS {
    Q_OBJECT

  signals:
    /**
     * Emitted when the elapsed time since the robot has been enabled is
     * changed.
     *
     * \note This signal is emitted periodically
     * \note If you want to use this information in the UI, we recommend you to
     *       bind to the \c elapsedTimeChanged(QString) signal, since it
     *       already contains a pre-formatted time string with the appropriate
     *       time value(s).
     */
    void elapsedTimeChanged (int msecs);

    /**
     * Emitted when the elapsed time is calculated again
     *
     * \note This signal is emitted periodically
     */
    void elapsedTimeChanged (const QString& string);

    /**
     * Emitted when the robot or DS status is changed
     */
    void statusChanged (const QString& status);

    /**
     * Emitted when protocol knows if the target robot is simulated or not
     */
    void simulatedChanged (const bool isSimulation);

    /**
     * Emitted when the robot is enabled or disabled
     */
    void enabledChanged (const EnableStatus status);

    /**
     * Emitted when the team alliance is changed (be it by client or FMS)
     */
    void allianceChanged (const Alliance alliance);

    /**
     * Emitted when the team position (be it by client or FMS)
     */
    void positionChanged (const Position position);

    /**
     * Emitted when the robot code availability is changed
     */
    void codeStatusChanged (const CodeStatus statusChanged);

    /**
     * Emitted when the control mode of the robot is changed
     */
    void controlModeChanged (const ControlMode mode);

    /**
     * Emitted when the robot address is changed (be it by the user/client or
     * the library when it discovers the IP address of the robot).
     */
    void robotAddressChanged (const QString& address);

    /**
     * Emitted when the radio address is changed (normally by the user/client).
     */
    void radioAddressChanged (const QString& address);

    /**
     * Emitted when the communication status with the FMS (Field Management
     * System) is changed.
     */
    void fmsCommStatusChanged (const CommStatus statusChanged);

    /**
     * Emitted when the communication status with the robot is changed
     */
    void robotCommStatusChanged (const CommStatus statusChanged);

    /**
     * Emitted when the communication status with the radio is changed
     */
    void radioCommStatusChanged (const CommStatus statusChanged);

    /**
     * Emitted when the voltage status of the robot is changed, for example,
     * when the robot reports a brownout.
     */
    void voltageStatusChanged (const VoltageStatus statusChanged);

    /**
     * Emitted when the operation status of the robot is changed, for example,
     * when we connect to the robot (changed from \c kDisconnected to
     * \c kNormal), or when the robot is emergency stopped (\c kEmergencyStop).
     */
    void operationStatusChanged (const OperationStatus statusChanged);

    /**
     * Emitted when the PDP (Power Distribution Panel) firmware version is
     * detected.
     */
    void pdpVersionChanged (const QString& version);

    /**
     * Emitted when the PCM (Pneumatics Control Module) firmware version is
     * detected.
     */
    void pcmVersionChanged (const QString& version);

    /**
     * Emitted when the software version of the robot controller is detected.
     */
    void libVersionChanged (const QString& version);

    /**
     * Emitted when the voltage of the robot is changed
     */
    void voltageChanged (const qreal voltage);

    /**
     * Emitted when the voltage of the robot is changed, the QString is already
     * formatted by the library
     */
    void voltageChanged (const QString& voltageString);

    /**
     * Emitted when the team number is changed
     */
    void teamChanged (const int team);

    /**
     * Emitted when the robot reports a change in the CPU usage of the primary
     * robot controller.
     */
    void cpuUsageChanged (const int usage);

    /**
     * Emitted when the robot reports a change in the RAM usage of the primary
     * robot controller.
     */
    void ramUsageChanged (const int usage);

    /**
     * Emitted when the robot reports a change in the disk usage of the primary
     * robot controller.
     */
    void diskUsageChanged (const int usage);
};


#endif
