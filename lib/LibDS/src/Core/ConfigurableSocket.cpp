/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#include "ConfigurableSocket.h"

/**
 * Implements a way to send data directly to the host, without the need of
 * using a UDP socket.
 *
 * This class is used in the DS in order to be able to use both TCP and UDP
 * sockets to send and receive data from the robot.
 */
ConfigurableSocket::ConfigurableSocket (const DS::SocketType& type)
{
    m_socketType = type;
    connect (socket(), SIGNAL (readyRead()), this, SIGNAL (readyRead()));
}

/**
 * Returns the received data on the socket using the implementations
 * for each socket type.
 */
QByteArray ConfigurableSocket::readAll()
{
    QByteArray data;

    if (socketType() == DS::kSocketTypeTCP)
        data = socket()->readAll();

    else {
        QHostAddress address;
        data.resize (udpSocket()->pendingDatagramSize());
        udpSocket()->readDatagram (data.data(), data.size(), &address);

        m_peerAddress = address.toString();
    }

    return data;
}

/**
 * Returns the peer address of the socket
 */
QString ConfigurableSocket::peerAddress()
{
    return m_peerAddress;
}

/**
 * Returns a pointer to the TCP socket
 */
QTcpSocket* ConfigurableSocket::tcpSocket()
{
    return &m_tcpSocket;
}

/**
 * Returns a pointer to the UDP socket
 */
QUdpSocket* ConfigurableSocket::udpSocket()
{
    return &m_udpSocket;
}

/**
 * Returns a pointer to the actual socket. Use this when you need to do
 * something more than just sending data, listening for incoming data or
 * establishing a TCP connection.
 *
 * Possible scenarios include reading the received data :)
 */
QAbstractSocket* ConfigurableSocket::socket()
{
    if (socketType() == DS::kSocketTypeTCP)
        return &m_tcpSocket;

    return &m_udpSocket;
}

/**
 * Returns the type of socket used. This function allows us to determine if
 * the socket is a TCP socket or an UDP socket.
 */
DS::SocketType ConfigurableSocket::socketType() const
{
    return m_socketType;
}

/**
 * Uses the socket's native methods to send the given \a data to the specified
 * \a ip and \a port.
 *
 * @param data the data to send
 * @param ip the network address in which to send the data
 * @param port the remote port in which to send the data
 *
 * @return the numbers of bytes written
 */
qint64 ConfigurableSocket::writeDatagram (const QByteArray& data,
                                          const QString& ip, quint16 port)
{
    return writeDatagram (data, QHostAddress (ip), port);
}

/**
 * Uses the socket's native methods to send the given \a data to the specified
 * \a ip and \a port.
 *
 * @param data the data to send
 * @param ip the network address in which to send the data
 * @param port the remote port in which to send the data
 *
 * @return the number of bytes written
 */
qint64 ConfigurableSocket::writeDatagram (const QByteArray& data,
                                          const QHostAddress& ip, quint16 port)
{
    if (socketType() == DS::kSocketTypeUDP)
        return m_udpSocket.writeDatagram (data, ip, port);

    if (socketType() == DS::kSocketTypeTCP)
        return m_tcpSocket.write (data);

    return -1;
}

/**
 * Instructs the socket to listed from incoming data from the given \a ip and
 * \a port.
 *
 * @param ip the address that sends the data
 * @param port the local port in which we receive the data
 * @param mode some bind flags (e.g. which allow us to obtain exclusive access
 *             to the IP and port or to share that access)
 */
void ConfigurableSocket::bind (const QString& ip, quint16 port,
                               QAbstractSocket::BindMode mode)
{
    bind (QHostAddress (ip), port, mode);
}

/**
 * Instructs the socket to listed from incoming data from the given \a ip and
 * \a port.
 *
 * @param ip the address that sends the data
 * @param port the local port in which we receive the data
 * @param mode some bind flags (e.g. which allow us to obtain exclusive access
 *             to the IP and port or to share that access)
 */
void ConfigurableSocket::bind (const QHostAddress& ip, quint16 port,
                               QAbstractSocket::BindMode mode)
{
    m_peerAddress = ip.toString();
    socket()->bind (ip, port, mode);
}

/**
 * Instructs the socket to establish a TCP connection with the given \a host in
 * the specified remote \a port
 *
 * @param host the host that we should connect to
 * @param port the remote port on the host
 * @param mode flags (e.g. write only, read only, read write, etc).
 */
void ConfigurableSocket::connectToHost (const QString& host,
                                        quint16 port,
                                        QIODevice::OpenMode mode)
{
    connectToHost (QHostAddress (host), port, mode);
}

/**
 * Instructs the socket to establish a TCP connection with the given \a host in
 * the specified remote \a port
 *
 * @param host the host that we should connect to
 * @param port the remote port on the host
 * @param mode flags (e.g. write only, read only, read write, etc).
 */
void ConfigurableSocket::connectToHost (const QHostAddress& host,
                                        quint16 port,
                                        QIODevice::OpenMode mode)
{
    if (socketType() == DS::kSocketTypeTCP) {
        if (m_tcpSocket.state() != QAbstractSocket::UnconnectedState)
            m_tcpSocket.disconnectFromHost();

        m_tcpSocket.connectToHost (host, port, mode);
    }
}
