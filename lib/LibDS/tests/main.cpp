/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#include "Test_CRC32.h"
#include "Test_Sockets.h"
#include "Test_Watchdog.h"
#include "Test_DS_Config.h"
#include "Test_NetConsole.h"
#include "Test_DriverStation.h"

int main (int argc, char* argv[]) {
    QApplication app (argc, argv);

    QTest::qExec (new Test_CRC32, argc, argv);
    QTest::qExec (new Test_Watchdog, argc, argv);
    QTest::qExec (new Test_DS_Config, argc, argv);
    QTest::qExec (new Test_DriverStation, argc, argv);
    QTest::qExec (new Test_SocketsSenderUDP, argc, argv);
    QTest::qExec (new Test_SocketsSenderTCP, argc, argv);
    QTest::qExec (new Test_NetConsoleSender, argc, argv);
    QTest::qExec (new Test_NetConsoleReceiver, argc, argv);

    QTimer::singleShot (2000, Qt::PreciseTimer, qApp, SLOT (quit()));

    return app.exec();
}
