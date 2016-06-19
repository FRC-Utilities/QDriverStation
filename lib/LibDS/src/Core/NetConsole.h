/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#ifndef _LIB_DS_NETCONSOLE_H
#define _LIB_DS_NETCONSOLE_H

#include <Core/DS_Base.h>

class NetConsole : public QObject {
    Q_OBJECT

  signals:
    void newMessage (const QString& message);

  public:
    explicit NetConsole();

  public slots:
    void setInputPort (int port);
    void setOutputPort (int port);
    void sendMessage (const QString& message);

  private:
    int m_outputPort;
    QUdpSocket m_inputSocket;
    QUdpSocket m_outputSocket;
};

#endif
