/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#ifndef TEST_NETCONSOLE
#define TEST_NETCONSOLE

#include <QtTest>
#include <Core/NetConsole.h>

//==============================================================================
// NETCONSOLE SENDER TEST
//==============================================================================

class Test_NetConsoleSender : public QObject {
    Q_OBJECT

  private slots:
    void initTestCase() {
        int port = 6668;

        received = "";
        message = "@Ahead)Together!FRC^2016";

        netconsole.setOutputPort (port);
        receiver.bind (DS_LISTENER, port, DS_BIND_FLAGS);

        connect (&receiver, &QUdpSocket::readyRead, [=]() {
            received = QString::fromUtf8 (DS::readSocket (&receiver));
        });

        netconsole.sendMessage (message);
        QTest::qWait (100);
    }

    void verifyMessage() {
        QCOMPARE (received, message);
    }

  private:
    QString message;
    QString received;
    QUdpSocket receiver;
    NetConsole netconsole;
};

//==============================================================================
// NETCONSOLE RECEIVER TEST
//==============================================================================

class Test_NetConsoleReceiver : public QObject {
    Q_OBJECT

  private slots:
    void initTestCase() {
        int port = 6666;

        received = "";
        message = "@Ahead)Together!FRC^2016";

        netconsole.setInputPort (port);
        connect (&netconsole, &NetConsole::newMessage, [=](const QString& msg) {
            received = msg;
        });

        sender.writeDatagram (message.toUtf8(), QHostAddress::Broadcast, port);
        QTest::qWait (100);
    }

    void verifyMessage() {
        QCOMPARE (received, message);
    }

  private:
    QString message;
    QString received;
    QUdpSocket sender;
    NetConsole netconsole;
};

#endif
