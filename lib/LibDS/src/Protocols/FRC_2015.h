/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#ifndef _LIB_DS_FRC_2015_H
#define _LIB_DS_FRC_2015_H

#include <QTime>
#include <Core/Protocol.h>

class FRC_2015 : public Protocol {
  public:
    FRC_2015();
    virtual QString name();

    /* Packet frequency definitions */
    virtual int fmsFrequency();
    virtual int robotFrequency();

    /* Network ports information */
    virtual int fmsInputPort();
    virtual int fmsOutputPort();
    virtual int robotInputPort();
    virtual int robotOutputPort();
    virtual int netconsoleInputPort();

    /* Joystick config */
    virtual int maxPOVCount();
    virtual int maxAxisCount();
    virtual int maxButtonCount();
    virtual int maxJoystickCount();

    /* Diagnostic functions */
    virtual void rebootRobot();
    virtual void restartRobotCode();
    virtual void onRobotWatchdogExpired();

    /* Battery information */
    virtual qreal nominalBatteryVoltage();
    virtual qreal nominalBatteryAmperage();

    /* Information regarding socket types */
    virtual DS::SocketType fmsSocketType();
    virtual DS::SocketType robotSocketType();

    /* Default addresses */
    virtual QString radioAddress();
    virtual QString robotAddress();

    /* Packet generation functions */
    virtual QByteArray getFMSPacket();
    virtual QByteArray getRobotPacket();

    /* Packet interpretation functions */
    virtual bool interpretFMSPacket (const QByteArray& data);
    virtual bool interpretRobotPacket (const QByteArray& data);

  protected:
    virtual QByteArray getTimezoneData();
    virtual QByteArray getJoystickData();

    virtual DS::Alliance getAlliance (DS_Byte station);
    virtual DS::Position getPosition (DS_Byte station);

    virtual void readExtended (const QByteArray& data);

    virtual DS_Byte getControlCode();
    virtual DS_Byte getRequestCode();
    virtual DS_Byte getFMSControlCode();
    virtual DS_Byte getTeamStationCode();
    virtual DS_Byte getJoystickSize (const DS::Joystick& joystick);

  private:
    bool m_restartCode;
    bool m_rebootRobot;
    bool m_sendDateTime;
};

#endif
