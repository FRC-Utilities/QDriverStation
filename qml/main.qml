/*
 * Copyright (c) 2015-2020 Alex Spataru <alex_spataru@outlook.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

import QtQuick 2.0

import "Dialogs"
import "MainWindow"
import "Globals.js" as Globals

Item {
    id: app
    visible: false

    //
    // Display the virtual joystick window (from anywhere in the app)
    //
    function showVirtualJoystickWindow() {
        virtualJoystickWindow.show()
    }

    //
    // Display the settings window (from anywhere in the app)
    //
    function showSettingsWindow() {
        settingsWindow.show()
    }

    //
    // Load the fonts used by the application
    //
    FontLoader { source: Qt.resolvedUrl ("qrc:/fonts/UbuntuMono.ttf")     }
    FontLoader { source: Qt.resolvedUrl ("qrc:/fonts/FontAwesome.ttf")    }
    FontLoader { source: Qt.resolvedUrl ("qrc:/fonts/Ubuntu-Bold.ttf")    }
    FontLoader { source: Qt.resolvedUrl ("qrc:/fonts/Ubuntu-Regular.ttf") }

    //
    // Initialize the DS engine when the application starts
    //
    Component.onCompleted: {
        if (!mainwindow.visible) {
            mainwindow.visible = true
            mainwindow.updateWindowMode()
        }

        Globals.beep (440, 100)
        Globals.beep (220, 100)
    }


    //
    // The Main window
    //
    MainWindow {
        id: mainwindow
        onVisibleChanged: {
            if (!visible)
                Qt.quit()
        }
    }

    //
    // The settings window
    //
    SettingsWindow {
        id: settingsWindow
    }

    //
    // The virtual joystick window
    //
    VirtualJoystickWindow {
        id: virtualJoystickWindow
    }
}
