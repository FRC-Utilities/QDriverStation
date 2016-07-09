/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#ifndef _LIB_DS_FRC_2014_H
#define _LIB_DS_FRC_2014_H

#include <Core/Protocol.h>
#include <Utilities/CRC32.h>

/**
 * \brief Implements the FRC 2009-2010 communication protocol
 */
class FRC_2014 : public Protocol {
  public:
    FRC_2014();
    virtual QString name();

    /* Packet frequency definitions */
    virtual int fmsFrequency();
    virtual int robotFrequency();

    /* Network ports information */
    virtual int fmsInputPort();
    virtual int fmsOutputPort();
    virtual int robotInputPort();
    virtual int robotOutputPort();

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
    virtual DS_Byte getAlliance();
    virtual DS_Byte getPosition();
    virtual DS_Byte getDigitalInput();
    virtual DS_Byte getOperationCode();
    virtual QByteArray getJoystickData();
    virtual DS::Alliance getAlliance (DS_Byte byte);
    virtual DS::Position getPosition (DS_Byte byte);


  private:
    bool m_resync;
    bool m_restartCode;
    bool m_rebootRobot;

    CRC32 m_crc32;
};

#endif
