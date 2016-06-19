/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#include "Sockets.h"
#include <QNetworkInterface>

/**
 * Sets the socket options for the given \a socket
 */
void CONFIGURE_SOCKET (QAbstractSocket* socket) {
    if (socket) {
        socket->setSocketOption (QAbstractSocket::LowDelayOption, 1);
        socket->setSocketOption (QAbstractSocket::MulticastLoopbackOption, 0);
    }
}

/**
 * Initializes the addresses, ports and sockets
 */
Sockets::Sockets() {
    m_fmsAddress = QHostAddress::Any;
    m_radioAddress = QHostAddress::Any;
    m_robotAddress = QHostAddress::Any;

    m_udpFmsSender = Q_NULLPTR;
    m_tcpFmsSender = Q_NULLPTR;
    m_udpRadioSender = Q_NULLPTR;
    m_tcpRadioSender = Q_NULLPTR;
    m_udpRobotSender = Q_NULLPTR;
    m_tcpRobotSender = Q_NULLPTR;
    m_udpFmsReceiver = Q_NULLPTR;
    m_tcpFmsReceiver = Q_NULLPTR;
    m_udpRadioReceiver = Q_NULLPTR;
    m_tcpRadioReceiver = Q_NULLPTR;
    m_udpRobotReceiver = Q_NULLPTR;
    m_tcpRobotReceiver = Q_NULLPTR;

    m_fmsOutputPort = DS_DISABLED_PORT;
    m_radioOutputPort = DS_DISABLED_PORT;
    m_robotOutputPort = DS_DISABLED_PORT;
}

/**
 * Deletes all the sockets
 */
Sockets::~Sockets() {
    delete m_udpFmsSender;
    delete m_tcpFmsSender;
    delete m_udpRadioSender;
    delete m_tcpRadioSender;
    delete m_udpRobotSender;
    delete m_tcpRobotSender;
    delete m_udpFmsReceiver;
    delete m_tcpFmsReceiver;
    delete m_udpRadioReceiver;
    delete m_tcpRadioReceiver;
    delete m_udpRobotReceiver;
    delete m_tcpRobotReceiver;
}

/**
 * Returns the FMS address. If the FMS protocol is based on UDP and we do not
 * know the FMS address, then we will broadcast generated FMS datagrams.
 */
QHostAddress Sockets::fmsAddress() const {
    if (m_fmsAddress.isNull() && m_udpFmsSender)
        return QHostAddress::Broadcast;

    return m_fmsAddress;
}

/**
 * Returns the radio address. If the radio protocol is based on UDP and we do
 * not know the radio address, then we will broadcast generated radio datagrams.
 */
QHostAddress Sockets::radioAddress() const {
    if (m_radioAddress.isNull() && m_udpRadioSender)
        return QHostAddress::Broadcast;

    return m_radioAddress;
}

/**
 * Returns the robot address. If the robot protocol is based on UDP and we do
 * not know the robot address, then we will broadcast generated robot datagrams.
 */
QHostAddress Sockets::robotAddress() const {
    if (m_robotAddress.isNull() && m_udpRobotSender)
        return QHostAddress::Broadcast;

    return m_robotAddress;
}

/**
 * Changes the port in which we receive data from the FMS
 */
void Sockets::setFMSInputPort (int port) {
    if (m_tcpFmsReceiver)
        m_tcpFmsReceiver->bind (DS_LISTENER, port, DS_BIND_FLAGS);

    if (m_udpFmsReceiver)
        m_udpFmsReceiver->bind (DS_LISTENER, port, DS_BIND_FLAGS);
}

/**
 * Changes the FMS destination port
 */
void Sockets::setFMSOutputPort (int port) {
    m_fmsOutputPort = port;
}

/**
 * Changes the port in which we receive data from the radio
 */
void Sockets::setRadioInputPort (int port) {
    if (m_tcpRadioReceiver)
        m_tcpRadioReceiver->bind (DS_LISTENER, port, DS_BIND_FLAGS);

    if (m_udpRadioReceiver)
        m_udpRadioReceiver->bind (DS_LISTENER, port, DS_BIND_FLAGS);
}

/**
 * Changes the port in which we receive data from the robot
 */
void Sockets::setRobotInputPort (int port) {
    if (m_tcpRobotReceiver)
        m_tcpRobotReceiver->bind (DS_LISTENER, port, DS_BIND_FLAGS);

    if (m_udpRobotReceiver)
        m_udpRobotReceiver->bind (DS_LISTENER, port, DS_BIND_FLAGS);
}

/**
 * Changes the radio destination robot
 */
void Sockets::setRadioOutputPort (int port) {
    m_radioOutputPort = port;
}

/**
 * Changes the robot destination robot
 */
void Sockets::setRobotOutputPort (int port) {
    m_robotOutputPort = port;
}

/**
 * Sends the given \a data to the FMS
 */
void Sockets::sendToFMS (const QByteArray& data) {
    if (m_tcpFmsSender)
        m_tcpFmsSender->write (data);

    if (m_udpFmsSender)
        m_udpFmsSender->writeDatagram (data, fmsAddress(), m_fmsOutputPort);
}

/**
 * Sends the given \a data to the robot
 */
void Sockets::sendToRobot (const QByteArray& data) {
    if (m_tcpRobotSender)
        m_tcpRobotSender->write (data);

    if (m_udpRobotSender)
        m_udpRobotSender->writeDatagram (data, robotAddress(), m_robotOutputPort);
}

/**
 * Sends the given \a data to the radio
 */
void Sockets::sendToRadio (const QByteArray& data) {
    if (m_tcpRadioSender)
        m_tcpRadioSender->write (data);

    if (m_udpRadioSender)
        m_udpRadioSender->writeDatagram (data, radioAddress(), m_radioOutputPort);
}

/**
 * Changes the set of sockets that will be used for FMS communications
 */
void Sockets::setFMSSocketType (DS::SocketType type) {
    delete m_udpFmsSender;
    delete m_tcpFmsSender;
    delete m_udpFmsReceiver;
    delete m_tcpFmsReceiver;

    m_udpFmsSender = Q_NULLPTR;
    m_tcpFmsSender = Q_NULLPTR;
    m_udpFmsReceiver = Q_NULLPTR;
    m_tcpFmsReceiver = Q_NULLPTR;

    if (type == DS::kSocketTypeTCP) {
        m_tcpFmsSender = new QTcpSocket (this);
        m_tcpFmsReceiver = new QTcpSocket (this);

        CONFIGURE_SOCKET (m_tcpFmsSender);
        CONFIGURE_SOCKET (m_tcpFmsReceiver);

        connect (m_tcpFmsReceiver, SIGNAL (readyRead()),
                 this,               SLOT (readFMSSocket()));
    }

    else {
        m_udpFmsSender = new QUdpSocket (this);
        m_udpFmsReceiver = new QUdpSocket (this);

        CONFIGURE_SOCKET (m_udpFmsSender);
        CONFIGURE_SOCKET (m_udpFmsReceiver);

        connect (m_udpFmsReceiver, SIGNAL (readyRead()),
                 this,               SLOT (readFMSSocket()));
    }
}

/**
 * Changes the set of sockets that will be used for radio communications
 */
void Sockets::setRadioSocketType (DS::SocketType type) {
    delete m_udpRadioSender;
    delete m_tcpRadioSender;
    delete m_udpRadioReceiver;
    delete m_tcpRadioReceiver;

    m_udpRadioSender = Q_NULLPTR;
    m_tcpRadioSender = Q_NULLPTR;
    m_udpRadioReceiver = Q_NULLPTR;
    m_tcpRadioReceiver = Q_NULLPTR;

    if (type == DS::kSocketTypeTCP) {
        m_tcpRadioSender = new QTcpSocket (this);
        m_tcpRadioReceiver = new QTcpSocket (this);

        CONFIGURE_SOCKET (m_tcpRadioSender);
        CONFIGURE_SOCKET (m_tcpRadioReceiver);

        connect (m_tcpRadioReceiver, SIGNAL (readyRead()),
                 this,                 SLOT (readRadioSocket()));
    }

    else {
        m_udpRadioSender = new QUdpSocket (this);
        m_udpRadioReceiver = new QUdpSocket (this);

        CONFIGURE_SOCKET (m_udpRadioSender);
        CONFIGURE_SOCKET (m_udpRadioReceiver);

        connect (m_udpRadioReceiver, SIGNAL (readyRead()),
                 this,                 SLOT (readRadioSocket()));
    }
}

/**
 * Changes the set of sockets that will be used for robot communications
 */
void Sockets::setRobotSocketType (DS::SocketType type) {
    delete m_udpRobotSender;
    delete m_tcpRobotSender;
    delete m_udpRobotReceiver;
    delete m_tcpRobotReceiver;

    m_udpRobotReceiver = Q_NULLPTR;
    m_tcpRobotReceiver = Q_NULLPTR;

    if (type == DS::kSocketTypeTCP) {
        m_tcpRobotSender = new QTcpSocket (this);
        m_tcpRobotReceiver = new QTcpSocket (this);

        CONFIGURE_SOCKET (m_tcpRobotSender);
        CONFIGURE_SOCKET (m_tcpRobotReceiver);

        connect (m_tcpRobotReceiver, SIGNAL (readyRead()),
                 this,                 SLOT (readRobotSocket()));
    }

    else {
        m_udpRobotSender = new QUdpSocket (this);
        m_udpRobotReceiver = new QUdpSocket (this);

        CONFIGURE_SOCKET (m_udpRobotSender);
        CONFIGURE_SOCKET (m_udpRobotReceiver);

        connect (m_udpRobotReceiver, SIGNAL (readyRead()),
                 this,                 SLOT (readRobotSocket()));
    }
}

/**
 * Changes the FMS address.
 * If the FMS address is set to an invalid value, then this class will
 * broadcast the data.
 */
void Sockets::setFMSAddress (const QHostAddress& address) {
    m_fmsAddress = address;
}

/**
 * Changes the radio address.
 * If the radio address is set to an invalid value, then this class will
 * broadcast the data.
 */
void Sockets::setRadioAddress (const QHostAddress& address) {
    m_radioAddress = address;
}

/**
 * Changes the robot address.
 * If the robot address is set to an invalid value, then this class will
 * broadcast the data.
 */
void Sockets::setRobotAddress (const QHostAddress& address) {
    m_robotAddress = address;
}

/**
 * Called when we receive data from the FMS
 */
void Sockets::readFMSSocket() {
    QByteArray data;
    QHostAddress address;

    if (m_tcpFmsReceiver) {
        data = DS::readSocket (m_tcpFmsReceiver);
        address = m_tcpFmsReceiver->peerAddress();
    }

    else if (m_udpFmsReceiver) {
        data = DS::readSocket (m_udpFmsReceiver);
        address = m_udpFmsReceiver->peerAddress();
    }

    setFMSAddress (address);
    emit fmsPacketReceived (data);
}

/**
 * Called when we receive data from the radio
 */
void Sockets::readRadioSocket() {
    QByteArray data;
    QHostAddress address;

    if (m_tcpRadioReceiver) {
        data = DS::readSocket (m_tcpRadioReceiver);
        address = m_tcpRadioReceiver->peerAddress();
    }

    else if (m_udpRadioReceiver) {
        data = DS::readSocket (m_udpRadioReceiver);
        address = m_udpRadioReceiver->peerAddress();
    }

    setRadioAddress (address);
    emit radioPacketReceived (data);
}

/**
 * Called when we receive data from the robot
 */
void Sockets::readRobotSocket() {
    QByteArray data;
    QHostAddress address;

    if (m_tcpRobotReceiver) {
        data = DS::readSocket (m_tcpRobotReceiver);
        address = m_tcpRobotReceiver->peerAddress();
    }

    else if (m_udpRobotReceiver) {
        data = DS::readSocket (m_udpRobotReceiver);
        address = m_udpRobotReceiver->peerAddress();
    }

    setRobotAddress (address);
    emit robotPacketReceived (data);
}
