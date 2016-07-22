/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#include <QObject>
#include <QHostInfo>
#include <QUdpSocket>

class MulticastDNS : public QObject {
    Q_OBJECT

  signals:
    void finished (const QHostInfo& info);

  public:
    MulticastDNS();

  public slots:
    void lookup (const QString& name);

  private slots:
    void onReadyRead();

  private:
    QString getHost (const QByteArray& data);
    QString getIPv4 (const QByteArray& data,
                     const QString& host);
    QString getIPv6 (const QByteArray& data,
                     const QString& host,
                     const QString& ipv4);
    QList<QHostAddress> getAddresses (const QByteArray& data,
                                      const QString& host);

  private:
    bool m_IPv4Binded;
    bool m_IPv6Binded;
    QUdpSocket m_IPv4Socket;
    QUdpSocket m_IPv6Socket;
};
