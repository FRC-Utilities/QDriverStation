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

class Sockets : public QObject {
    Q_OBJECT

  signals:
    void fmsPacketReceived (const QByteArray& data);
    void radioPacketReceived (const QByteArray& data);
    void robotPacketReceived (const QByteArray& data);

  public:
    Sockets();
    ~Sockets();

    QHostAddress fmsAddress() const;
    QHostAddress radioAddress() const;
    QHostAddress robotAddress() const;

  public slots:
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
    void readFMSSocket();
    void readRadioSocket();
    void readRobotSocket();
    void onFMSLookupFinished (const QHostInfo& info);
    void onRadioLookupFinished (const QHostInfo& info);
    void onRobotLookupFinished (const QHostInfo& info);

  private:
    QHostAddress getAddress (const QHostInfo& info);

  private:
    int m_robotIterator;
    int m_fmsOutputPort;
    int m_radioOutputPort;
    int m_robotOutputPort;

    QHostAddress m_fmsAddress;
    QHostAddress m_robotAddress;
    QHostAddress m_radioAddress;

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
