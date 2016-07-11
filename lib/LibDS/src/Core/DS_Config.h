/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#ifndef _LIB_DS_PRIVATE_CONFIG_H
#define _LIB_DS_PRIVATE_CONFIG_H

#include <Core/DS_Base.h>

class Logger;
class QElapsedTimer;

/**
 * \brief Updates the variables shared across the LibDS classes
 *
 * This class updates the variables of the \c DriverStation class without
 * exposing potentionally dangerous/private functions to the client.
 *
 * This class is meant to be used only by protocols, we could define these
 * functions directly with each protocol, but this could increase memory usage
 * and - if not managed correctly - cause a lot of issues with the event system
 * of Qt.
 */
class DS_Config : public DS_Base {
    Q_OBJECT
    friend class DriverStation;

  public:
    static DS_Config* getInstance();

    int team() const;
    int cpuUsage() const;
    int ramUsage() const;
    int diskUsage() const;
    qreal voltage() const;
    bool isEnabled() const;
    bool isSimulated() const;
    Alliance alliance() const;
    Position position() const;
    bool isFMSAttached() const;
    QString libVersion() const;
    QString pcmVersion() const;
    QString pdpVersion() const;
    bool isEmergencyStopped() const;
    bool isRobotCodeRunning() const;
    bool isConnectedToRadio() const;
    bool isConnectedToRobot() const;
    ControlMode controlMode() const;
    CommStatus fmsCommStatus() const;
    EnableStatus enableStatus() const;
    CommStatus radioCommStatus() const;
    CommStatus robotCommStatus() const;
    CodeStatus robotCodeStatus() const;
    VoltageStatus voltageStatus() const;
    OperationStatus operationStatus() const;

  public slots:
    void updateTeam (int team);
    void setRobotCode (bool code);
    void setEnabled (bool enabled);
    void updateCpuUsage (int usage);
    void updateRamUsage (int usage);
    void updateDiskUsage (int usage);
    void setBrownout (bool brownout);
    void setEmergencyStop (bool estop);
    void updateVoltage (qreal voltage);
    void updateSimulated (bool simulated);
    void updateAlliance (Alliance alliance);
    void updatePosition (Position position);
    void updateControlMode (ControlMode mode);
    void updateLibVersion (const QString& version);
    void updatePcmVersion (const QString& version);
    void updatePdpVersion (const QString& version);
    void updateEnabled (EnableStatus statusChanged);
    void updateFMSCommStatus (CommStatus statusChanged);
    void updateRadioCommStatus (CommStatus statusChanged);
    void updateRobotCommStatus (CommStatus statusChanged);
    void updateRobotCodeStatus (CodeStatus statusChanged);
    void updateVoltageStatus (VoltageStatus statusChanged);
    void updateOperationStatus (OperationStatus statusChanged);

  private slots:
    void updateElapsedTime();

  protected:
    DS_Config();
    Logger* logger();

  private:
    int m_team;
    int m_cpuUsage;
    int m_ramUsage;
    int m_diskUsage;
    qreal m_voltage;

    Alliance m_alliance;
    Position m_position;
    QString m_libVersion;
    QString m_pcmVersion;
    QString m_pdpVersion;
    CodeStatus m_codeStatus;
    ControlMode m_controlMode;
    CommStatus m_fmsCommStatus;
    EnableStatus m_enableStatus;
    CommStatus m_radioCommStatus;
    CommStatus m_robotCommStatus;
    VoltageStatus m_voltageStatus;
    OperationStatus m_operationStatus;

    bool m_simulated;
    bool m_timerEnabled;

    QElapsedTimer* m_timer;
    Logger* m_logger;
};

#endif
