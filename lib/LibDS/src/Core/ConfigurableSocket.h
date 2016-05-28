/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#ifndef _LIB_DS_CONFIGURABLE_SOCKET_H
#define _LIB_DS_CONFIGURABLE_SOCKET_H

#include <QTcpSocket>
#include <QUdpSocket>
#include <Core/DS_Common.h>

class ConfigurableSocket : public QObject
{
    Q_OBJECT

public:
    explicit ConfigurableSocket (const DS::SocketType& type);

    QByteArray readAll();
    QString peerAddress();
    QTcpSocket* tcpSocket();
    QUdpSocket* udpSocket();
    QAbstractSocket* socket();
    DS::SocketType socketType() const;

    qint64 writeDatagram (const QByteArray& data, const QString& ip, quint16 port);
    qint64 writeDatagram (const QByteArray& data, const QHostAddress& ip,
                          quint16 port);

public slots:
    void bind (const QString& ip, quint16 port, QAbstractSocket::BindMode mode);
    void bind (const QHostAddress& ip, quint16 port,
               QAbstractSocket::BindMode mode);
    void connectToHost (const QString& host, quint16 port,
                        QIODevice::OpenMode mode);
    void connectToHost (const QHostAddress& host, quint16 port,
                        QIODevice::OpenMode mode);

signals:
    void readyRead();

private:
    QString m_peerAddress;
    QUdpSocket m_udpSocket;
    QTcpSocket m_tcpSocket;
    DS::SocketType m_socketType;
};

#endif

