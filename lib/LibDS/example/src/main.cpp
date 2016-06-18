/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#include "Window.h"
#include "VirtualJoystick.h"

int main (int argc, char* argv[]) {
    QApplication app (argc, argv);

    Window window;
    window.show();

    VirtualJoystick joystick;
    Q_UNUSED (joystick);

    return app.exec();
}
