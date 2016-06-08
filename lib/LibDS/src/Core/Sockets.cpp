/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#include "Sockets.h"
#include <QNetworkInterface>

// *INDENT-OFF*
#define WRITE QIODevice::WriteOnly
#define LBACK QAbstractSocket::MulticastLoopbackOption
#define FLAGS QAbstractSocket::ShareAddress | QAbstractSocket::ReuseAddressHint
// *INDENT-ON*

Sockets::Sockets() {
    m_iterator        = 0;
    m_scanRate     = 0;
    m_robotIp         = "";
    m_radioIp         = "";
    m_fmsSender       = Q_NULLPTR;
    m_fmsReceiver     = Q_NULLPTR;
    m_radioSender     = Q_NULLPTR;
    m_robotSender     = Q_NULLPTR;
    m_radioReceiver   = Q_NULLPTR;
    m_robotReceiver   = Q_NULLPTR;
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

    qDebug() << "Socket Manager initialized!";
}

/**
 * Returns the number of IPs probed per watchdog expiration time.
 * If the client/user did not assign a custom scan rate, then this function
 * will calculate an appropiate value based on the size of the robot IP list.
 */
int Sockets::scanRate() const {
    int rate = 0;

    if (customScanRate() > 0)
        rate = customScanRate();
    else
        rate = qMin (72, qMax (addressList().count() / 6, 1));

    return rate;
}

/**
 * Returns the port from which we receive data from the FMS
 */
int Sockets::fmsInputPort() const {
    return m_fmsInput;
}

/**
 * Returns the port in which we send data to the FMS
 */
int Sockets::fmsOutputPort() const {
    return m_fmsOutput;
}

/**
 * Returns the port in which we receive data from the robot radio
 */
int Sockets::radioInputPort() const {
    return m_radioInput;
}

/**
 * Returns the port in which we receive data from the robot
 */
int Sockets::robotInputPort() const {
    return m_robotInput;
}

/**
 * Returns the port in which we send data to the robot radio
 */
int Sockets::radioOutputPort() const {
    return m_radioOutput;
}

/**
 * Returns the port in which we send data to the robot
 */
int Sockets::robotOutputPort() const {
    return m_robotOutput;
}

/**
 * Returns the custom scan rate set by the client or user. If this value
 * is equal to zero, then this class will calculate an value appropiate to
 * the size of the robot IPs list.
 */
int Sockets::customScanRate() const {
    return m_scanRate;
}

/**
 * Returns the IP address of the robot radio
 */
QString Sockets::radioAddress() const {
    return m_radioIp;
}

/**
 * Returns the IP address of the robot (if set)
 */
QString Sockets::robotAddress() const {
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
QStringList Sockets::addressList() const {
    return m_robotIpList;
}

/**
 * Returns the socket type (UDP or TCP) used for client/FMS communications.
 */
DS::SocketType Sockets::fmsSocketType() const {
    return m_fmsSocketType;
}

/**
 * Returns the socket type used for client/radio communications.
 */
DS::SocketType Sockets::radioSocketType() const {
    return m_radioSocketType;
}

/**
 * Returns the socket type used for client/radio communications.
 */
DS::SocketType Sockets::robotSocketType() const {
    return m_robotSocketType;
}

/**
 * This function allows us to probe each IP on the robot IP list by "scrolling"
 * the list and probing several IPs at the same time. This function is called
 * when we finish sending a robot packet, thus, the scan speed is determined by
 * the following factors:
 *     - The frequency in which the DS sends a robot packet
 *     - The scan rate (which allow us to probe more IPs at the same time)
 */
void Sockets::refreshAddressList() {
    if (robotAddress().isEmpty() && !addressList().isEmpty()) {
        if (addressList().count() > m_iterator + scanRate())
            m_iterator += scanRate();

        else
            m_iterator = 0;
    }
}

/**
 * Sends the given \c data to the FMS (Field Management System).
 */
void Sockets::sendToFMS (const QByteArray& data) {
    if (m_fmsSender && fmsOutputPort() != DISABLED_PORT)
        m_fmsSender->writeDatagram (data, QHostAddress::Any, fmsOutputPort());
}

/**
 * Sends the given \a data to the robot. If the robot address is not
 * specified (e.g. when we don't have comms with robot), the data will be
 * sent to a dynamic set of IPs (depending on the scan rate).
 */
void Sockets::sendToRobot (const QByteArray& data) {
    if (robotOutputPort() == DISABLED_PORT)
        return;

    if (m_robotSender && !robotAddress().isEmpty())
        m_robotSender->writeDatagram (data, robotAddress(), robotOutputPort());

    else {
        for (int i = 0; i < scanRate(); ++i) {
            if (scanRate() > i && addressList().count() > (m_iterator + i)) {
                QString ip = addressList().at (m_iterator + i);
                m_robotSender->writeDatagram (data, ip, robotOutputPort());
            }
        }
    }
}

/**
 * Sends the given \a data to the robot radio.
 */
void Sockets::sendToRadio (const QByteArray& data) {
    if (m_radioSender && radioOutputPort() != DISABLED_PORT)
        m_radioSender->writeDatagram (data,
                                      QHostAddress (radioAddress()),
                                      radioOutputPort());
}

/**
 * Changes the radio IP, this should only done by the \c Protocol and not
 * the user.
 */
void Sockets::setRadioAddress (const QString& ip) {
    m_radioIp = ip;

    if (m_radioReceiver)
        m_radioReceiver->bind (ip, radioInputPort(), FLAGS);

    if (m_radioSender)
        m_radioSender->connectToHost (ip, radioInputPort(), WRITE);

    qDebug() << "Radio IP set to" << ip;
}

/**
 * Changes the robot IP, this can be done automatically when we detect the
 * robot or by the client.
 *
 * It is not recommended to use a custom address, since this class should
 * be able to detect any sign of the robot very fast.
 */
void Sockets::setRobotAddress (const QString& ip) {
    m_robotIp = ip;

    if (m_robotReceiver)
        m_robotReceiver->bind (ip, robotInputPort(), FLAGS);

    if (m_robotSender)
        m_robotSender->connectToHost (ip, robotOutputPort(), WRITE);

    qDebug() << "Robot IP set to" << ip;
}

/**
 * Sets the IP list that we use to detect the robot.
 * \note The \c Protocol generates a list with all the IP address of each
 *       LAN interface (e.g. ethernet & wifi) in order to make the robot
 *       detection process faster and less error-prone.
 */
void Sockets::setAddressList (const QStringList& list) {
    m_robotIpList.clear();
    m_robotIpList = list;
    generateLocalNetworkAddresses();
}

/**
 * Changes the \c port in which we receive data from the FMS.
 */
void Sockets::setFMSInputPort (int port) {
    m_fmsInput = port;

    if (m_fmsReceiver)
        m_fmsReceiver->bind (port, FLAGS);

    qDebug() << "FMS input port set to" << port;
}

/**
 * Changes the \c port in which we send data to the FMS.
 */
void Sockets::setFMSOutputPort (int port) {
    m_fmsOutput = port;

    if (m_fmsSender)
        m_fmsSender->connectToHost (QHostAddress::Any, port, WRITE);

    qDebug() << "FMS output port set to" << port;
}

/**
 * Changes the \c port in which we receive data from the radio.
 */
void Sockets::setRadioInputPort (int port) {
    m_radioInput = port;

    if (m_radioReceiver)
        m_radioReceiver->bind (port, FLAGS);

    qDebug() << "Radio input port set to" << port;
}

/**
 * Changes the \c port in which we receive data from the robot.
 */
void Sockets::setRobotInputPort (int port) {
    m_robotInput = port;

    if (m_robotReceiver)
        m_robotReceiver->bind (port, FLAGS);

    qDebug() << "Robot input port set to" << port;
}

/**
 * Changes the \c port in which we send data to the radio.
 */
void Sockets::setRadioOutputPort (int port) {
    m_radioOutput = port;

    if (m_radioSender)
        m_radioSender->connectToHost (radioAddress(), port, WRITE);

    qDebug() << "Radio output port set to" << port;
}

/**
 * Changes the \c port in which we send data to the robot.
 */
void Sockets::setRobotOutputPort (int port) {
    m_robotOutput = port;

    if (m_robotSender)
        m_robotSender->connectToHost (robotAddress(), port, WRITE);

    qDebug() << "Robot output port set to" << port;
}

/**
 * Changes the parallel socket \c count. This can be used to increase the
 * LAN scan speed at the cost of more memory usage.
 *
 * If the \c count is set to 0, then this function will calculate the best
 * socket count based on the size of the \c robotIpList()
 */
void Sockets::setScanRate (int count) {
    m_scanRate = count;
    qDebug() << "Scan rate set to" << count;
}

/**
 * Changes the socket type (UDP or TCP) that we use to communicate with
 * the FMS.
 */
void Sockets::setFMSSocketType (const DS::SocketType& type) {
    m_fmsSocketType = type;

    free (m_fmsSender);
    free (m_fmsReceiver);

    m_fmsSender = new ConfigurableSocket (type);
    m_fmsReceiver = new ConfigurableSocket (type);

    m_fmsSender->socket()->setSocketOption (LBACK, 0);
    m_fmsReceiver->socket()->setSocketOption (LBACK, 0);

    connect (m_fmsReceiver, SIGNAL (readyRead()), this, SLOT (readFMSSocket()));

    qDebug() << "FMS socket type set to" << type;
}

/**
 * Changes the socket type (UDP or TCP) that we use to communicate with
 * the robot radio.
 */
void Sockets::setRadioSocketType (const DS::SocketType& type) {
    m_radioSocketType = type;

    free (m_radioSender);
    free (m_radioReceiver);

    m_radioSender = new ConfigurableSocket (type);
    m_radioReceiver = new ConfigurableSocket (type);

    m_radioSender->socket()->setSocketOption (LBACK, 0);
    m_radioReceiver->socket()->setSocketOption (LBACK, 0);

    connect (m_radioReceiver, SIGNAL (readyRead()), this, SLOT (readRadioSocket()));

    qDebug() << "Radio socket type set to" << type;
}

/**
 * Changes the socket type (UDP or TCP) that we use to communicate with
 * the robot
 */
void Sockets::setRobotSocketType (const DS::SocketType& type) {
    m_robotSocketType = type;

    free (m_robotSender);
    free (m_robotReceiver);

    m_robotSender = new ConfigurableSocket (type);
    m_robotReceiver = new ConfigurableSocket (type);

    m_robotSender->socket()->setSocketOption (LBACK, 0);
    m_robotReceiver->socket()->setSocketOption (LBACK, 0);

    connect (m_robotReceiver, SIGNAL (readyRead()), this, SLOT (readRobotSocket()));

    qDebug() << "Robot socket type set to" << type;
}

/**
 * Called when we receive data from the FMS. To avoid crashing the application,
 * this function will check if the socket pointer is not NULL before trying to
 * read its data.
 */
void Sockets::readFMSSocket() {
    if (m_fmsReceiver)
        emit fmsPacketReceived (m_fmsReceiver->readAll());
}

/**
 * Called when we receive data from the radio. To avoid crashing the
 * application, this function will check if the socket pointer is not NULL
 * before trying to read its data.
 */
void Sockets::readRadioSocket() {
    if (m_radioReceiver)
        emit radioPacketReceived (m_radioReceiver->readAll());
}

/**
 * This function will self-assign the robot IP once we receive the
 * first robot packet, giving a break to the modem.
 */
void Sockets::readRobotSocket() {
    QByteArray data = m_robotReceiver->readAll();

    if (!data.isEmpty()) {
        if (robotAddress().isEmpty())
            setRobotAddress (m_robotReceiver->peerAddress());

        emit robotPacketReceived (data);
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
void Sockets::generateLocalNetworkAddresses() {
    foreach (QNetworkInterface interface, QNetworkInterface::allInterfaces()) {
        bool isUp      = (interface.flags() & QNetworkInterface::IsUp);
        bool isRunning = (interface.flags() & QNetworkInterface::IsRunning);

        if (isUp && isRunning) {
            foreach (QNetworkAddressEntry address, interface.addressEntries()) {
                if (address.ip().toString() == "127.0.0.1")
                    break;
                if (address.ip().isNull())
                    break;

                QStringList numbers = address.ip().toString().split (".");
                qDebug() << "Client IP detected:" << address.ip().toString();

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

    m_robotIpList.append ("127.0.0.1");
}
