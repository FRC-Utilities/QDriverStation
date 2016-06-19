/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#ifndef TEST_SOCKETS
#define TEST_SOCKETS

#include <QtTest>
#include <Core/Sockets.h>

//==============================================================================
// GLOBAL VARIABLES
//==============================================================================

#define SOCKET_BIND_MODE QAbstractSocket::ShareAddress | \
    QAbstractSocket::ReuseAddressHint

//==============================================================================
// SOCKET SENDER TESTS (UDP)
//==============================================================================

class Test_SocketsSenderUDP : public QObject {
    Q_OBJECT

  private slots:
    void initTestCase() {
        int fmsPort = 1160;
        int radPort = 1140;
        int robPort = 1110;

        testData = QByteArray ("@Ahead)Together!FRC^2016");

        sockets.setFMSSocketType (DS::kSocketTypeUDP);
        sockets.setRadioSocketType (DS::kSocketTypeUDP);
        sockets.setRobotSocketType (DS::kSocketTypeUDP);

        sockets.setFMSInputPort (0);
        sockets.setRadioInputPort (0);
        sockets.setRobotInputPort (0);
        sockets.setFMSOutputPort (fmsPort);
        sockets.setRadioOutputPort (radPort);
        sockets.setRobotOutputPort (robPort);

        fmsReceiver.bind (DS_LISTENER, fmsPort, SOCKET_BIND_MODE);
        radReceiver.bind (DS_LISTENER, radPort, SOCKET_BIND_MODE);
        robReceiver.bind (DS_LISTENER, robPort, SOCKET_BIND_MODE);

        connect (&fmsReceiver, &QUdpSocket::readyRead, [=]() {
            fmsData = DS::readSocket (&fmsReceiver);
        });

        connect (&radReceiver, &QUdpSocket::readyRead, [=]() {
            radData = DS::readSocket (&radReceiver);
        });

        connect (&robReceiver, &QUdpSocket::readyRead, [=]() {
            robData = DS::readSocket (&robReceiver);
        });

        sockets.sendToFMS (testData);
        sockets.sendToRadio (testData);
        sockets.sendToRobot (testData);

        QTest::qWait (100);
    }

    void checkFMS() {
        QCOMPARE (fmsData, testData);
    }

    void checkRadio() {
        QCOMPARE (radData, testData);
    }

    void checkRobot() {
        QCOMPARE (robData, testData);
    }

  private:
    Sockets sockets;
    QUdpSocket fmsReceiver;
    QUdpSocket radReceiver;
    QUdpSocket robReceiver;

    QByteArray fmsData;
    QByteArray radData;
    QByteArray robData;
    QByteArray testData;
};

//==============================================================================
// SOCKET SENDER TESTS (TCP)
//==============================================================================

class Test_SocketsSenderTCP : public QObject {
    Q_OBJECT

  private slots:
    void initTestCase() {
        int fmsPort = 1120;
        int radPort = 1130;
        int robPort = 1150;

        address = DS_LISTENER;
        testData = QByteArray ("Hello World");

        sockets.setFMSSocketType (DS::kSocketTypeTCP);
        sockets.setRadioSocketType (DS::kSocketTypeTCP);
        sockets.setRobotSocketType (DS::kSocketTypeTCP);

        sockets.setFMSAddress (address);
        sockets.setRadioAddress (address);
        sockets.setRobotAddress (address);

        sockets.setFMSOutputPort (fmsPort);
        sockets.setRadioOutputPort (radPort);
        sockets.setRobotOutputPort (robPort);

        fmsReceiver.connectToHost (address, fmsPort, QIODevice::ReadOnly);
        radReceiver.connectToHost (address, radPort, QIODevice::ReadOnly);
        robReceiver.connectToHost (address, robPort, QIODevice::ReadOnly);

        connect (&fmsReceiver, &QUdpSocket::readyRead, [=]() {
            fmsData = DS::readSocket (&fmsReceiver);
        });

        connect (&radReceiver, &QUdpSocket::readyRead, [=]() {
            radData = DS::readSocket (&radReceiver);
        });

        connect (&robReceiver, &QUdpSocket::readyRead, [=]() {
            robData = DS::readSocket (&robReceiver);
        });

        sockets.sendToFMS (testData);
        sockets.sendToRadio (testData);
        sockets.sendToRobot (testData);

        QTest::qWait (100);
    }

    void checkFMS() {
        QCOMPARE (fmsData, testData);
    }

    void checkRadio() {
        QCOMPARE (radData, testData);
    }

    void checkRobot() {
        QCOMPARE (robData, testData);
    }

  private:
    Sockets sockets;
    QHostAddress address;
    QTcpSocket fmsReceiver;
    QTcpSocket radReceiver;
    QTcpSocket robReceiver;

    QByteArray fmsData;
    QByteArray radData;
    QByteArray robData;
    QByteArray testData;
};

#endif
