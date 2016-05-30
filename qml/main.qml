/*
 * Copyright (c) 2015-2016 WinT 3794 <http://wint3794.org>
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
import QtQuick.Window 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0
import Qt.labs.settings 1.0

import "widgets"
import "globals.js" as Globals

ApplicationWindow {
    id: window

    //
    // If set to true, the window will dock to the bottom of the screen
    //
    property bool docked: false

    //
    // Set window flags
    //
    flags: Qt.Window |
           Qt.WindowTitleHint |
           Qt.WindowSystemMenuHint |
           Qt.WindowCloseButtonHint |
           Qt.WindowMinimizeButtonHint

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
    // The actual docking procedures
    //
    function updateWindowMode() {
        if (docked) {
            showMaximized()
            y = Screen.desktopAvailableHeight - height
        } else {
            showNormal()
            width = minimumWidth
        }
    }

    //
    // Save window position if the window is not docked
    //
    x: 100
    y: 100

    //
    // Define the size of the window
    //
    minimumWidth: Globals.scale (1185)
    minimumHeight: Globals.scale (210)
    maximumHeight: Globals.scale (210)
    onDockedChanged: updateWindowMode()

    //
    // Misc. properties
    //
    color: Globals.Colors.WindowBackground
    title: appDspName + " - " + qsTr ("Version") + " " + appVersion

    //
    // Show the window and initialize the DS engine when the application starts
    //
    Component.onCompleted: {
        show()
        updateWindowMode()

        DriverStation.init()
    }

    Connections {
        target: DriverStation
        onInitialized: {
            Globals.beep (440, 100)
            Globals.beep (220, 100)
        }
    }

    //
    // Load the fonts used by the application
    //
    FontLoader { source: Qt.resolvedUrl ("qrc:/fonts/UbuntuMono.ttf")     }
    FontLoader { source: Qt.resolvedUrl ("qrc:/fonts/FontAwesome.ttf")    }
    FontLoader { source: Qt.resolvedUrl ("qrc:/fonts/Ubuntu-Bold.ttf")    }
    FontLoader { source: Qt.resolvedUrl ("qrc:/fonts/Ubuntu-Regular.ttf") }

    //
    // Save window geometry
    //
    Settings {
        category: "MainWindow"
        property alias x: window.x
        property alias y: window.y
        property alias docked: window.docked
    }

    //
    // Display the left tab, status controls and right tab horizontally
    //
    RowLayout {
        anchors.fill: parent
        anchors.margins: Globals.spacing

        //
        // Operator, Diagnostics, Preferences & Joysticks
        //
        LeftTab {
            Layout.fillWidth: false
            Layout.fillHeight: true
            Layout.minimumWidth: Globals.scale (460)
            onWindowModeChanged: window.docked = isDocked
            onFlashStatusIndicators: status.flashStatusIndicators()
        }

        //
        // Robot status & LEDs
        //
        Status {
            id: status
            Layout.fillHeight: true
            Layout.maximumWidth: implicitWidth
            Layout.minimumHeight: implicitHeight
        }

        //
        // Messages, CAN Metrics & About tabs
        //
        RightTab {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumWidth: Globals.scale (420)
        }
    }

    //
    // The settings window,
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

    //
    // Asks for team number on first launch
    //
    GetTeamNumber {
        id: getTeamNumber
    }
}
