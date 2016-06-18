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

class DS_Base : public DS {
    Q_OBJECT

  signals:
    /**
     * Emitted when the elapsed time since the robot has been enabled is
     * changed
     */
    void elapsedTimeChanged (int msecs);

    /**
     * Emitted when the elapsed time is calculated again
     */
    void elapsedTimeChanged (QString string);

    /**
     * Emitted when the robot or DS status is changed
     */
    void statusChanged (QString status);

    /**
     * Emitted when protocol knows if the target robot is simulated or not
     */
    void simulatedChanged (bool isSimulation);

    /**
     * Emitted when the robot is enabled or disabled
     */
    void enabledChanged (EnableStatus status);

    /**
     * Emitted when the team alliance is changed (be it by client or FMS)
     */
    void allianceChanged (Alliance alliance);

    /**
     * Emitted when the team position (be it by client or FMS)
     */
    void positionChanged (Position position);

    /**
     * Emitted when the robot code availability is changed
     */
    void codeStatusChanged (CodeStatus statusChanged);

    /**
     * Emitted when the control mode of the robot is changed
     */
    void controlModeChanged (ControlMode mode);

    /**
     * Emitted when the robot address is changed (be it by the user/client or
     * the library when it discovers the IP address of the robot).
     */
    void robotAddressChanged (QString address);

    /**
     * Emitted when the radio address is changed (normally by the user/client).
     */
    void radioAddressChanged (QString address);

    /**
     * Emitted when the communication status with the FMS (Field Management
     * System) is changed.
     */
    void fmsCommStatusChanged (CommStatus statusChanged);

    /**
     * Emitted when the communication status with the robot is changed
     */
    void robotCommStatusChanged (CommStatus statusChanged);

    /**
     * Emitted when the communication status with the radio is changed
     */
    void radioCommStatusChanged (CommStatus statusChanged);

    /**
     * Emitted when the voltage status of the robot is changed, for example,
     * when the robot reports a brownout.
     */
    void voltageStatusChanged (VoltageStatus statusChanged);

    /**
     * Emitted when the operation status of the robot is changed, for example,
     * when we connect to the robot (changed from \c kDisconnected to
     * \c kNormal), or when the robot is emergency stopped (\c kEmergencyStop).
     */
    void operationStatusChanged (OperationStatus statusChanged);

    /**
     * Emitted when the PDP (Power Distribution Panel) firmware version is
     * detected.
     */
    void pdpVersionChanged (QString version);

    /**
     * Emitted when the PCM (Pneumatics Control Module) firmware version is
     * detected.
     */
    void pcmVersionChanged (QString version);

    /**
     * Emitted when the software version of the robot controller is detected.
     */
    void libVersionChanged (QString version);

    /**
     * Emitted when the voltage of the robot is changed
     */
    void voltageChanged (float voltage);

    /**
     * Emitted when the voltage of the robot is changed, the QString is already
     * formatted by the library
     */
    void voltageChanged (QString voltageString);

    /**
     * Emitted when the team number is changed
     */
    void teamChanged (int team);

    /**
     * Emitted when the robot reports a change in the CPU usage of the primary
     * robot controller.
     */
    void cpuUsageChanged (int usage);

    /**
     * Emitted when the robot reports a change in the RAM usage of the primary
     * robot controller.
     */
    void ramUsageChanged (int usage);

    /**
     * Emitted when the robot reports a change in the disk usage of the primary
     * robot controller.
     */
    void diskUsageChanged (int usage);
};


#endif
