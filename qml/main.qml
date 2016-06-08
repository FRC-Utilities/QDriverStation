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
    // Holds the old values of x and y
    //
    property int oldX: 100
    property int oldY: 100

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
        if (!visible)
            return

        if (docked) {
            showMaximized()
            y = Screen.desktopAvailableHeight - height
        } else {
            showNormal()
            width = minimumWidth

            x = oldX
            y = oldY
        }
    }

    //
    // Save window position if the window is not docked
    //
    x: 100
    y: 100

    //
    // Update custom variables
    //
    onDockedChanged: updateWindowMode()
    onXChanged: oldX = !docked ? x : oldX
    onYChanged: oldY = !docked ? y : oldY

    //
    // Animations, we all like 'em!
    //
    Behavior on x { NumberAnimation { duration: Globals.slowAnimation } }
    Behavior on y { NumberAnimation { duration: Globals.slowAnimation } }

    //
    // Define the size of the window
    //
    maximumHeight: minimumHeight

    //
    // Misc. properties
    //
    color: Globals.Colors.WindowBackground
    title: appDspName + " - " + qsTr ("Version") + " " + appVersion

    //
    // Initialize the DS engine when the application starts
    //
    Component.onCompleted: DriverStation.init()

    Connections {
        target: DriverStation
        onInitialized: {
            Globals.niceBeep (440, Globals.defaultOffset, 100)
            Globals.niceBeep (220, Globals.defaultOffset, 100)

            if (!visible) {
                visible = true
                updateWindowMode()
            }
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
        property alias oldX: window.oldX
        property alias oldY: window.oldY
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
            Layout.fillWidth: false
            Layout.fillHeight: true
        }

        //
        // Messages, CAN Metrics & About tabs
        //
        RightTab {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumWidth: Globals.scale (520)
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
