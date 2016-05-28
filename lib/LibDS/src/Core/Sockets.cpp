/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#include "Sockets.h"
#include <QNetworkInterface>

#define WRITE_ONLY QIODevice::WriteOnly
#define MULTICAST_LOOPBACK QAbstractSocket::MulticastLoopbackOption
#define FLAGS QAbstractSocket::ShareAddress | QAbstractSocket::ReuseAddressHint

Sockets::Sockets()
{
    m_iterator        = 0;
    m_socketCount     = 0;
    m_robotIp         = "";
    m_radioIp         = "";
    m_fmsSender       = Q_NULLPTR;
    m_fmsReceiver     = Q_NULLPTR;
    m_radioSender     = Q_NULLPTR;
    m_robotSender     = Q_NULLPTR;
    m_radioReceiver   = Q_NULLPTR;
    m_fmsInput        = DISABLED_PORT;
    m_fmsOutput       = DISABLED_PORT;
    m_radioInput      = DISABLED_PORT;
    m_robotInput      = DISABLED_PORT;
    m_radioOutput     = DISABLED_PORT;
    m_robotOutput     = DISABLED_PORT;
    m_robotIpList     = QStringList ("");
    m_fmsSocketType   = DS::kSocketTypeUDP;
    m_radioSocketType = DS::kSocketTypeUDP;
    m_robotSocketType = DS::kSocketTypeUDP;
}

Sockets::~Sockets()
{
    clearSocketLists();
}

/**
 * Returns the number of parallel socket pairs.
 * If the client/user did not assign a custom number of parallel sockets, then
 * this function will calculate an appropiate value based on the size of the
 * robot IP list.
 */
int Sockets::socketCount() const
{
    if (customSocketCount() > 0)
        return customSocketCount();

    else
        return qMax (robotIpList().count() / 6, 4);
}

/**
 * Returns the port from which we receive data from the FMS
 */
int Sockets::fmsInputPort() const
{
    return m_fmsInput;
}

/**
 * Returns the port in which we send data to the FMS
 */
int Sockets::fmsOutputPort() const
{
    return m_fmsOutput;
}

/**
 * Returns the port in which we receive data from the robot radio
 */
int Sockets::radioInputPort() const
{
    return m_radioInput;
}

/**
 * Returns the port in which we receive data from the robot
 */
int Sockets::robotInputPort() const
{
    return m_robotInput;
}

/**
 * Returns the port in which we send data to the robot radio
 */
int Sockets::radioOutputPort() const
{
    return m_radioOutput;
}

/**
 * Returns the port in which we send data to the robot
 */
int Sockets::robotOutputPort() const
{
    return m_robotOutput;
}

/**
 * Returns the custom socket count set by the client or user. If this value
 * is equal to zero, then this class will calculate an value appropiate to
 * the size of the robot IPs list.
 */
int Sockets::customSocketCount() const
{
    return m_socketCount;
}

/**
 * Returns the IP address of the robot radio
 */
QString Sockets::radioIp() const
{
    return m_radioIp;
}

/**
 * Returns the IP address of the robot (if set)
 */
QString Sockets::robotIp() const
{
    return m_robotIp;
}

/**
 * Returns a list with the possible IP addresses of the robot.
 * Please note that the \c Protocol appends a generated list with all
 * the possible IPs based on the current IP(s) address(es) of the client.
 *
 * While this can be considered as brute force, it is more reliable than
 * hopping that the robot respects the default address, that mDNS works
 * and that the default gateway is available.
 *
 * Using this method is also faster, as we can control the ammount of
 * parallel socket pairs working on detecting the robot, effectively
 * limiting our scan speed to the ammount of RAM that we want the client to
 * use.
 */
QStringList Sockets::robotIpList() const
{
    return m_robotIpList;
}

/**
 * Returns the socket type (UDP or TCP) used for client/FMS communications.
 */
DS::SocketType Sockets::fmsSocketType() const
{
    return m_fmsSocketType;
}

/**
 * Returns the socket type used for client/radio communications.
 */
DS::SocketType Sockets::radioSocketType() const
{
    return m_radioSocketType;
}

/**
 * Returns the socket type used for client/radio communications.
 */
DS::SocketType Sockets::robotSocketType() const
{
    return m_robotSocketType;
}

/**
 * Sends the given \c data to the FMS (Field Management System).
 */
void Sockets::sendToFMS (const QByteArray& data)
{
    if (m_fmsSender && fmsOutputPort() != DISABLED_PORT)
        m_fmsSender->writeDatagram (data, QHostAddress::Any, fmsOutputPort());
}

/**
 * Sends the given \a data to the robot. If the robot address is not
 * specified (e.g. when we don't have comms with robot), the data will be
 * sent to a set of parallel sockets, which send the data to different
 * IPs obtained from the LAN until one of them responds.
 */
void Sockets::sendToRobot (const QByteArray& data)
{
    if (robotOutputPort() == DISABLED_PORT)
        return;

    if (m_robotSender && !robotIp().isEmpty())
        m_robotSender->writeDatagram (data, robotIp(), robotOutputPort());

    else if (robotSocketType() == DS::kSocketTypeUDP) {
        refreshRobotIPs();
        for (int i = 0; i < robotIpList().count(); ++i) {
            if (socketCount() > i && robotIpList().count() > m_iterator + i) {
                QString ip = robotIpList().at (m_iterator + i);
                m_robotSenderList.at (i)->writeDatagram (data,
                                                         QHostAddress (ip),
                                                         robotOutputPort());
            }
        }
    }
}

/**
 * Sends the given \a data to the robot radio.
 */
void Sockets::sendToRadio (const QByteArray& data)
{
    if (m_radioSender && radioOutputPort() != DISABLED_PORT)
        m_radioSender->writeDatagram (data,
                                      QHostAddress (radioIp()),
                                      radioOutputPort());
}

/**
 * Changes the radio IP, this should only done by the \c Protocol and not
 * the user.
 */
void Sockets::setRadioIp (const QString& ip)
{
    m_radioIp = ip;
    if (m_radioReceiver)
        m_radioReceiver->bind (ip, radioInputPort(), FLAGS);
    if (m_radioSender)
        m_radioSender->connectToHost (ip, radioInputPort(), WRITE_ONLY);
}

/**
 * Changes the robot IP, this can be done automatically when we detect the
 * robot or by the client.
 *
 * It is not recommended to use a custom address, since this class should
 * be able to detect any sign of the robot very fast.
 */
void Sockets::setRobotIp (const QString& ip)
{
    m_robotIp = ip;
    if (m_robotSender)
        m_robotSender->connectToHost (ip, robotOutputPort(), WRITE_ONLY);
}

/**
 * Sets the IP list that we use to detect the robot.
 * \note The \c Protocol generates a list with all the IP address of each
 *       LAN interface (e.g. ethernet & wifi) in order to make the robot
 *       detection process faster and less error-prone.
 */
void Sockets::setRobotIpList (const QStringList& list)
{
    m_robotIpList.clear();

    m_robotIpList = list;
    generateLocalNetworkAddresses();
}

/**
 * Changes the parallel socket \c count. This can be used to increase the
 * LAN scan speed at the cost of more memory usage.
 *
 * If the \c count is set to 0, then this function will calculate the best
 * socket count based on the size of the \c robotIpList()
 */
void Sockets::setSocketCount (const int& count)
{
    m_socketCount = count;
    generateSocketPairs();
}

/**
 * Changes the \c port in which we receive data from the FMS.
 */
void Sockets::setFMSInputPort (const int& port)
{
    m_fmsInput = port;
    if (m_fmsReceiver)
        m_fmsReceiver->bind (QHostAddress::Any, port, FLAGS);
}

/**
 * Changes the \c port in which we send data to the FMS.
 */
void Sockets::setFMSOutputPort (const int& port)
{
    m_fmsOutput = port;
    if (m_fmsSender)
        m_fmsSender->connectToHost (QHostAddress::Any, port, WRITE_ONLY);
}

/**
 * Changes the \c port in which we receive data from the radio.
 */
void Sockets::setRadioInputPort (const int& port)
{
    m_radioInput = port;
    if (m_radioReceiver)
        m_radioReceiver->bind (QHostAddress (radioIp()), port, FLAGS);
}

/**
 * Changes the \c port in which we receive data from the robot.
 */
void Sockets::setRobotInputPort (const int& port)
{
    m_robotInput = port;
}

/**
 * Changes the \c port in which we send data to the radio.
 */
void Sockets::setRadioOutputPort (const int& port)
{
    m_radioOutput = port;
    if (m_radioSender)
        m_radioSender->connectToHost (radioIp(), port, WRITE_ONLY);
}

/**
 * Changes the \c port in which we send data to the robot.
 */
void Sockets::setRobotOutputPort (const int& port)
{
    m_robotOutput = port;
    if (m_robotSender)
        m_robotSender->connectToHost (robotIp(), port, WRITE_ONLY);
}

/**
 * Changes the socket type (UDP or TCP) that we use to communicate with
 * the FMS.
 */
void Sockets::setFMSSocketType (const DS::SocketType& type)
{
    m_fmsSocketType = type;
    free (m_fmsSender);
    free (m_fmsReceiver);
    m_fmsSender = new ConfigurableSocket (type);
    m_fmsReceiver = new ConfigurableSocket (type);
    m_fmsReceiver->socket()->setSocketOption (MULTICAST_LOOPBACK, 0);
}

/**
 * Changes the socket type (UDP or TCP) that we use to communicate with
 * the robot radio.
 */
void Sockets::setRadioSocketType (const DS::SocketType& type)
{
    m_radioSocketType = type;
    free (m_radioSender);
    free (m_radioReceiver);
    m_radioSender = new ConfigurableSocket (type);
    m_radioReceiver = new ConfigurableSocket (type);
    m_radioReceiver->socket()->setSocketOption (MULTICAST_LOOPBACK, 0);
}

/**
 * Changes the socket type (UDP or TCP) that we use to communicate with
 * the robot
 */
void Sockets::setRobotSocketType (const DS::SocketType& type)
{
    m_robotSocketType = type;
    free (m_robotSender);
    m_robotSender = new ConfigurableSocket (type);
    if (type == DS::kSocketTypeTCP) {
        m_robotSender->socket()->connectToHost (robotIp(),
                                                robotOutputPort(),
                                                QIODevice::WriteOnly);
    }
}

/**
 * Called when we receive data from the FMS. To avoid crashing the application,
 * this function will check if the socket pointer is not NULL before trying to
 * read its data.
 */
void Sockets::readFMSData()
{
    if (m_fmsReceiver)
        emit fmsPacketReceived (m_fmsReceiver->readAll());
}

/**
 * Called when we receive data from the radio. To avoid crashing the
 * application, this function will check if the socket pointer is not NULL
 * before trying to read its data.
 */
void Sockets::readRadioData()
{
    if (m_radioReceiver)
        emit radioPacketReceived (m_radioReceiver->readAll());
}

/**
 * Called when we receive data from the robot. Since we are using the magical
 * parallel sockets, this function will self-assign the robot IP once we
 * receive the first robot packet.
 *
 * Doing so allows us to stop sending data through the parallel sockets, which
 * can cause the application to use more memory and the radio to "lag".
 */
void Sockets::readRobotData()
{
    ConfigurableSocket* socket = qobject_cast<ConfigurableSocket*> (sender());
    QByteArray data = socket->readAll();

    if (robotIp().isEmpty() && !data.isEmpty())
        setRobotIp (socket->peerAddress());

    if (!data.isEmpty())
        emit robotPacketReceived (data);

}

/**
 * This function allows us to probe each IP on the robot IP list by "scrolling"
 * the list and probing several IPs at the same time. This function is called
 * when we finish sending a robot packet, thus, the scan speed is determined by
 * the following factors:
 *     - The frequency in which the DS sends a robot packet
 *     - The number of parallel sockets (which allow us to probe more IPs
 *       at the same time)
 *
 * Of course, this function will do nothing if we use TCP for robot
 * communications or we already know the robot IP.
 */
void Sockets::refreshRobotIPs()
{
    if (!robotIp().isEmpty() || robotSocketType() == DS::kSocketTypeTCP)
        return;

    if (robotIpList().count() > m_iterator + socketCount())
        m_iterator += socketCount();

    else
        m_iterator = 0;

    for (int i = 0; i < robotIpList().count(); ++i) {
        if (socketCount() > i && robotIpList().count() > m_iterator + i) {
            m_robotInputSockets.at (i)->socket()->disconnectFromHost();
            m_robotInputSockets.at (i)->bind (
                QHostAddress (m_robotIpList.at (m_iterator + i)),
                robotInputPort(),
                FLAGS);
        }
    }
}

/**
 * Deletes the parallel sockets from the memory
 */
void Sockets::clearSocketLists()
{
    m_iterator = 0;
    m_robotSenderList.clear();
    m_robotInputSockets.clear();
}

/**
 * Generates the parallel sockets. The number of generated parallel sockets is
 * determined by the return-value of the \c socketCount() function.
 */
void Sockets::generateSocketPairs()
{
    clearSocketLists();

    for (int i = 0; i < socketCount(); ++i) {
        ConfigurableSocket* sender = new ConfigurableSocket (robotSocketType());
        ConfigurableSocket* receiver = new ConfigurableSocket (robotSocketType());

        m_robotSenderList.append (sender);
        m_robotInputSockets.append (receiver);

        connect (receiver, SIGNAL (readyRead()),
                 this,       SLOT (readRobotData()));

        receiver->tcpSocket()->setSocketOption (MULTICAST_LOOPBACK, 0);
        receiver->udpSocket()->setSocketOption (MULTICAST_LOOPBACK, 0);
    }
}

/**
 * Generates a very wide range of possible robot IPs. The generated list
 * contains all the possible LAN IPs for each hardware interface of the client
 * computer.
 *
 * For example, say that the computer is connected to both a WiFi network and
 * an ethernet network. In this case, this function will get both IPs of the
 * computer (the WiFi IP and the ethernet IP) and create a list of the possible
 * LAN addresses of each interface.
 *
 * Here is a more practical example:
 *     - WiFi IP of computer is 168.192.1.64, the function will generate the
 *       following IPs:
 *         - 168.192.1.1
 *         - 168.192.1.2
 *         - 168.192.1.3
 *         - ...
 *         - 168.192.1.254
 *     - The function will do this for each interface (ethernet, wifi, usb, etc)
 */
void Sockets::generateLocalNetworkAddresses()
{
    if (robotSocketType() == DS::kSocketTypeTCP)
        return;

    foreach (QNetworkInterface interface, QNetworkInterface::allInterfaces()) {
        bool isUp      = (interface.flags() & QNetworkInterface::IsUp);
        bool isRunning = (interface.flags() & QNetworkInterface::IsRunning);

        if (isUp && isRunning) {
            foreach (QNetworkAddressEntry address, interface.addressEntries()) {
                if (address.ip().toString() == "127.0.0.1")
                    return;
                if (address.ip().isNull())
                    return;

                QStringList numbers = address.ip().toString().split (".");

                if (numbers.count() == 4) {
                    QString base = QString ("%1.%2.%3.")
                                   .arg (numbers.at (0))
                                   .arg (numbers.at (1))
                                   .arg (numbers.at (2));

                    for (int i = 1; i < 255; ++i)
                        m_robotIpList.append (base + QString::number (i));
                }
            }
        }
    }

    generateSocketPairs();
}
