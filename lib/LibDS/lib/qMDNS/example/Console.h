/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of qMDNS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#include <QObject>
#include <QHostInfo>

class Console : public QObject {
    Q_OBJECT

  public:
    Console();

  public slots:
    void run();
    void showHelp();
    void showHostName();
    void promptLookup();
    void promptSetHost();
    void lookup (QString name);
    void setHostName (QString name);
    void onDeviceDiscovered (const QHostInfo& info);

  private slots:
    void enableUserInput();
    void disableUserInput();

  private:
    bool m_enabled;
};
