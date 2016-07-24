/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of qMDNS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#include <QDebug>
#include <QCoreApplication>

#include "Console.h"

int main (int argc, char** argv) {
    QCoreApplication application (argc, argv);

#if defined Q_OS_WIN
    system ("cls");
#else
    Q_UNUSED (system ("clear"));
#endif

    qDebug() << "qMDNS Example Project";
    qDebug() << "Welcome, type \"help\" to get started.\n";

    Console console;
    console.run();

    return application.exec();
}
