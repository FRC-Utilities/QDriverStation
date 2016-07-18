/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#include "Sockets.h"

#include <QHostInfo>
#include <DriverStation.h>
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
 * Returns the string used to display an IP in the console
 */
QString GET_CONSOLE_IP (const QHostAddress& address) {
    QString ip = address.toString();

    if (ip.isEmpty())
        ip = "Auto";

    return ip;
}

Sockets::Sockets() {
    /* Assign invalid addresses */
    m_fmsAddress = QHostAddress ("");
    m_radioAddress = QHostAddress ("");
    m_robotAddress = QHostAddress ("");

    /* Assign null pointers to every socket */
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

    /* Initialize variables used for lookups */
    m_driverStation = Q_NULLPTR;

    /* Assign the initial ports */
    m_fmsOutputPort = DS_DISABLED_PORT;
    m_radioOutputPort = DS_DISABLED_PORT;
    m_robotOutputPort = DS_DISABLED_PORT;
}

/**
 * Returns the FMS address.
 */
QHostAddress Sockets::fmsAddress() const {
    return m_fmsAddress;
}

/**
 * Returns the radio address.
 */
QHostAddress Sockets::radioAddress() const {
    return m_radioAddress;
}

/**
 * Returns the robot address. If the robot address is not known, try using the
 * local IP address (e.g. for simulations)
 */
QHostAddress Sockets::robotAddress() const {
    if (m_robotAddress.isNull())
        return QHostAddress::LocalHost;

    return m_robotAddress;
}
/**
 * If any of the IPs used during the communications is not known,
 * this function will ensure that the DS performs a lookup periodically
 * until all addresses are known.
 *
 * This is especially useful when we are dealing with dynamic IPs, such as
 * when the robot uses a mDNS address.
 */
void Sockets::performLookups() {
    /* Assign the driver station pointer */
    if (!m_driverStation)
        m_driverStation = DriverStation::getInstance();

    /* Perform a FMS lookup */
    if (m_fmsAddress.isNull() && !m_driverStation->isConnectedToFMS())
        QHostInfo::lookupHost (m_driverStation->fmsAddress(),
                               this,
                               SLOT (onFMSLookupFinished (QHostInfo)));

    /* Perform a radio lookup */
    if (m_radioAddress.isNull() && !m_driverStation->isConnectedToRadio())
        QHostInfo::lookupHost (m_driverStation->radioAddress(),
                               this,
                               SLOT (onRadioLookupFinished (QHostInfo)));

    /* Perform a robot lookup */
    if (m_robotAddress.isNull() && !m_driverStation->isConnectedToRobot())
        QHostInfo::lookupHost (m_driverStation->robotAddress(),
                               this,
                               SLOT (onRobotLookupFinished (QHostInfo)));

    /* Wait and perform the lookup again */
    DS_Schedule (2000, this, SLOT (performLookups()));
}

/**
 * Changes the port in which we receive data from the FMS
 */
void Sockets::setFMSInputPort (int port) {
    m_fmsInputPort = port;
    bindSockets();
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
    m_radioInputPort = port;
    bindSockets();
}

/**
 * Changes the port in which we receive data from the robot
 */
void Sockets::setRobotInputPort (int port) {
    m_robotInputPort = port;
    bindSockets();
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
    if (data.isEmpty())
        return;

    if (m_tcpFmsSender)
        m_tcpFmsSender->write (data);

    else if (m_udpFmsSender)
        m_udpFmsSender->writeDatagram (data, fmsAddress(), m_fmsOutputPort);
}

/**
 * Sends the given \a data to the robot
 */
void Sockets::sendToRobot (const QByteArray& data) {
    if (data.isEmpty())
        return;

    if (m_tcpRobotSender)
        m_tcpRobotSender->write (data);

    else if (m_udpRobotSender)
        m_udpRobotSender->writeDatagram (data, robotAddress(), m_robotOutputPort);
}

/**
 * Sends the given \a data to the radio
 */
void Sockets::sendToRadio (const QByteArray& data) {
    if (data.isEmpty())
        return;

    if (m_tcpRadioSender)
        m_tcpRadioSender->write (data);

    else if (m_udpRadioSender)
        m_udpRadioSender->writeDatagram (data, radioAddress(), m_radioOutputPort);
}

/**
 * Changes the set of sockets that will be used for FMS communications
 */
void Sockets::setFMSSocketType (DS::SocketType type) {
    /* Destroy all FMS sockets */
    delete m_udpFmsSender;
    delete m_tcpFmsSender;
    delete m_udpFmsReceiver;
    delete m_tcpFmsReceiver;

    /* Assign a null pointer to all sockets, so that we do not crash */
    m_udpFmsSender = Q_NULLPTR;
    m_tcpFmsSender = Q_NULLPTR;
    m_udpFmsReceiver = Q_NULLPTR;
    m_tcpFmsReceiver = Q_NULLPTR;

    /* FMS comms. will be done with TCP from now on */
    if (type == DS::kSocketTypeTCP) {
        m_tcpFmsSender = new QTcpSocket (this);
        m_tcpFmsReceiver = new QTcpSocket (this);

        CONFIGURE_SOCKET (m_tcpFmsSender);
        CONFIGURE_SOCKET (m_tcpFmsReceiver);

        connect (m_tcpFmsReceiver, SIGNAL (readyRead()),
                 this,               SLOT (readFMSSocket()));
    }

    /* FMS comms. will be done with UDP from now on */
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
    /* Destroy all radio sockets */
    delete m_udpRadioSender;
    delete m_tcpRadioSender;
    delete m_udpRadioReceiver;
    delete m_tcpRadioReceiver;

    /* Assign a null pointer to all sockets, so that we do not crash */
    m_udpRadioSender = Q_NULLPTR;
    m_tcpRadioSender = Q_NULLPTR;
    m_udpRadioReceiver = Q_NULLPTR;
    m_tcpRadioReceiver = Q_NULLPTR;

    /* Radio comms. will be done with TCP from now on */
    if (type == DS::kSocketTypeTCP) {
        m_tcpRadioSender = new QTcpSocket (this);
        m_tcpRadioReceiver = new QTcpSocket (this);

        CONFIGURE_SOCKET (m_tcpRadioSender);
        CONFIGURE_SOCKET (m_tcpRadioReceiver);

        connect (m_tcpRadioReceiver, SIGNAL (readyRead()),
                 this,                 SLOT (readRadioSocket()));
    }

    /* Radio comms. will be done with UDP from now on */
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
    /* Destroy all robot sockets */
    delete m_udpRobotSender;
    delete m_tcpRobotSender;
    delete m_udpRobotReceiver;
    delete m_tcpRobotReceiver;

    /* Assign a null pointer to all sockets, so that we do not crash */
    m_udpRobotReceiver = Q_NULLPTR;
    m_tcpRobotReceiver = Q_NULLPTR;

    /* Robot comms. will be done with TCP from now on */
    if (type == DS::kSocketTypeTCP) {
        m_tcpRobotSender = new QTcpSocket (this);
        m_tcpRobotReceiver = new QTcpSocket (this);

        CONFIGURE_SOCKET (m_tcpRobotSender);
        CONFIGURE_SOCKET (m_tcpRobotReceiver);

        connect (m_tcpRobotReceiver, SIGNAL (readyRead()),
                 this,                 SLOT (readRobotSocket()));
    }

    /* Robot comms. will be done with UDP from now on */
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
void Sockets::setFMSAddress (const QString& address) {
    QHostAddress ip = QHostAddress (address);

    if (ip.isNull() && !address.isEmpty())
        qDebug() << "FMS address" << address << "scheduled for lookup";

    setFMSAddress (ip);
}

/**
 * Changes the radio address.
 * If the radio address is set to an invalid value, then this class will
 * broadcast the data.
 */
void Sockets::setRadioAddress (const QString& address) {
    QHostAddress ip = QHostAddress (address);

    if (ip.isNull() && !address.isEmpty())
        qDebug() << "FMS address" << address << "scheduled for lookup";

    setRadioAddress (ip);
}

/**
 * Changes the robot address.
 * If the robot address is set to an invalid value, then this class will
 * broadcast the data.
 */
void Sockets::setRobotAddress (const QString& address) {
    QHostAddress ip = QHostAddress (address);

    if (ip.isNull() && !address.isEmpty())
        qDebug() << "Robot address" << address << "scheduled for lookup";

    setRobotAddress (ip);
}

/**
 * Changes the FMS address to the given \a address
 */
void Sockets::setFMSAddress (const QHostAddress& address) {
    if (m_fmsAddress != address && !address.isNull()) {
        m_fmsAddress = address;
        bindSockets();
        qDebug() << "FMS Address set to" << GET_CONSOLE_IP (address);
    }
}

/**
 * Changes the radio address to the given \a address
 */
void Sockets::setRadioAddress (const QHostAddress& address) {
    if (m_radioAddress != address && !address.isNull()) {
        m_radioAddress = address;
        bindSockets();
        qDebug() << "Radio Address set to" << GET_CONSOLE_IP (address);
    }
}

/**
 * Changes the robot address to the given \a address
 */
void Sockets::setRobotAddress (const QHostAddress& address) {
    if (m_robotAddress != address && !address.isNull()) {
        m_robotAddress = address;
        bindSockets();
        qDebug() << "Robot Address set to" << GET_CONSOLE_IP (address);
    }
}

/**
 * Binds the input sockets to the respective (robot | radio | FMS) addresses and
 * incoming data ports
 */
void Sockets::bindSockets() {
    if (m_tcpFmsReceiver) {
        m_tcpFmsReceiver->abort();
        m_tcpFmsReceiver->bind (fmsAddress(),
                                m_fmsInputPort,
                                DS_BIND_MODE);
    }

    else if (m_udpFmsReceiver) {
        m_udpFmsReceiver->abort();
        m_udpFmsReceiver->bind (fmsAddress(),
                                m_fmsInputPort,
                                DS_BIND_MODE);
    }

    if (m_tcpRadioReceiver) {
        m_tcpRadioReceiver->abort();
        m_tcpRadioReceiver->bind (radioAddress(),
                                  m_radioInputPort,
                                  DS_BIND_MODE);
    }

    else if (m_udpRadioReceiver) {
        m_udpRadioReceiver->abort();
        m_udpRadioReceiver->bind (radioAddress(),
                                  m_radioInputPort,
                                  DS_BIND_MODE);
    }

    if (m_tcpRobotReceiver) {
        m_tcpRobotReceiver->abort();
        m_tcpRobotReceiver->bind (robotAddress(),
                                  m_robotInputPort,
                                  DS_BIND_MODE);
    }

    else if (m_udpRobotReceiver) {
        m_udpRobotReceiver->abort();
        m_udpRobotReceiver->bind (robotAddress(),
                                  m_robotInputPort,
                                  DS_BIND_MODE);
    }
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

/**
 * Assigns the found FMS IP
 */
void Sockets::onFMSLookupFinished (const QHostInfo& info) {
    if (fmsAddress().isNull() && !info.addresses().isEmpty())
        setFMSAddress (info.addresses().first());
}

/**
 * Assigns the found radio IP
 */
void Sockets::onRadioLookupFinished (const QHostInfo& info) {
    if (radioAddress().isNull() && !info.addresses().isEmpty())
        setRadioAddress (info.addresses().first());
}

/**
 * Assigns the found robot IP
 */
void Sockets::onRobotLookupFinished (const QHostInfo& info) {
    if (robotAddress().isNull() && !info.addresses().isEmpty())
        setRobotAddress (info.addresses().first());
}
