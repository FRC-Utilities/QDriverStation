/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#ifndef _LIB_DS_SOCKETS_H
#define _LIB_DS_SOCKETS_H

#include <Core/DS_Base.h>

class DriverStation;

/**
 * \brief Sends and receives data from the FMS, radio and robot targets
 *
 * This class also performs the host lookup processes for each target if it is
 * required (for example, when using mDNS targets). As a rule of thumb, this
 * class will broadcast generated packets if we do not know the target IP.
 *
 * This class is controlled directly by the \c DriverStation, which acts as a
 * man-in-the-middle between the loaded \c Protocol and the \c Sockets class.
 *
 * \note The packets can be sent either with UDP or TCP packets (as defined by
 *       the DS/protocol)
 */
class Sockets : public QObject {
    Q_OBJECT

  signals:
    void fmsPacketReceived (const QByteArray& data);
    void radioPacketReceived (const QByteArray& data);
    void robotPacketReceived (const QByteArray& data);

  public:
    explicit Sockets();

    QHostAddress fmsAddress() const;
    QHostAddress radioAddress() const;
    QHostAddress robotAddress() const;

  public slots:
    void performLookups();
    void setFMSInputPort (int port);
    void setFMSOutputPort (int port);
    void setRadioInputPort (int port);
    void setRobotInputPort (int port);
    void setRadioOutputPort (int port);
    void setRobotOutputPort (int port);
    void sendToFMS (const QByteArray& data);
    void sendToRobot (const QByteArray& data);
    void sendToRadio (const QByteArray& data);
    void setFMSSocketType (DS::SocketType type);
    void setRadioSocketType (DS::SocketType type);
    void setRobotSocketType (DS::SocketType type);
    void setFMSAddress (const QString& address);
    void setRadioAddress (const QString& address);
    void setRobotAddress (const QString& address);
    void setFMSAddress (const QHostAddress& address);
    void setRadioAddress (const QHostAddress& address);
    void setRobotAddress (const QHostAddress& address);

  private slots:
    void bindSockets();
    void readFMSSocket();
    void readRadioSocket();
    void readRobotSocket();
    void onFMSLookupFinished (const QHostInfo& info);
    void onRadioLookupFinished (const QHostInfo& info);
    void onRobotLookupFinished (const QHostInfo& info);

  private:
    int m_robotIterator;
    int m_fmsOutputPort;
    int m_radioOutputPort;
    int m_robotOutputPort;

    int m_fmsLookupId;
    int m_radioLookupId;
    int m_robotLookupId;

    int m_fmsInputPort;
    int m_robotInputPort;
    int m_radioInputPort;

    QHostAddress m_fmsAddress;
    QHostAddress m_robotAddress;
    QHostAddress m_radioAddress;

    DriverStation* m_driverStation;

    QUdpSocket* m_udpFmsSender;
    QTcpSocket* m_tcpFmsSender;
    QUdpSocket* m_udpRadioSender;
    QTcpSocket* m_tcpRadioSender;
    QUdpSocket* m_udpRobotSender;
    QTcpSocket* m_tcpRobotSender;
    QUdpSocket* m_udpFmsReceiver;
    QTcpSocket* m_tcpFmsReceiver;
    QUdpSocket* m_udpRadioReceiver;
    QTcpSocket* m_tcpRadioReceiver;
    QUdpSocket* m_udpRobotReceiver;
    QTcpSocket* m_tcpRobotReceiver;
};

#endif
