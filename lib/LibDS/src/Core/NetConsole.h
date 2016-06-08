/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#ifndef _LIB_DS_NETCONSOLE_H
#define _LIB_DS_NETCONSOLE_H

#include <Core/ConfigurableSocket.h>

class NetConsole : public QObject {
    Q_OBJECT

  public:
    NetConsole();
    Q_INVOKABLE int inputPort() const;
    Q_INVOKABLE int outputPort() const;

  public slots:
    void setInputPort (int port);
    void setOutputPort (int port);
    void sendMessage (const QString& message);

  private slots:
    void onReadyRead();

  signals:
    void newMessage (const QString& message);

  private:
    int m_inputPort;
    int m_outputPort;

    ConfigurableSocket* m_inputSocket;
    ConfigurableSocket* m_outputSocket;
};

#endif
