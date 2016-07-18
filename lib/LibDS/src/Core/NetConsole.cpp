/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#include "DS_Common.h"
#include "NetConsole.h"

NetConsole::NetConsole() {
    m_outputPort = 0;
    connect (&m_inputSocket, &QUdpSocket::readyRead, [ = ]() {
        emit newMessage (QString::fromUtf8 (DS::readSocket (&m_inputSocket)));
    });
}

/**
 * Changes the port in which we receive broadcasted robot messages.
 * If the \a port is set to \c 0, then the \c NetConsole will disable the
 * input socket.
 */
void NetConsole::setInputPort (int port) {
    if (port != DS_DISABLED_PORT)
        m_inputSocket.bind (QHostAddress::Broadcast, port, DS_BIND_MODE);
}

/**
 * Changes the port in which we broadcast commands to the robot.
 * If the \a port is set to \c 0, then the \c NetConsole will disable the
 * output socket.
 */
void NetConsole::setOutputPort (int port) {
    m_outputPort = port;
}

/**
 * Broadcasts the given \a message to the robot.
 * \note the output port must not be \c 0 in order for this to work
 */
void NetConsole::sendMessage (const QString& message) {
    if (!message.isEmpty() && m_outputPort != DS_DISABLED_PORT) {
        m_outputSocket.writeDatagram (message.toUtf8(),
                                      QHostAddress::Broadcast,
                                      m_outputPort);
    }
}
