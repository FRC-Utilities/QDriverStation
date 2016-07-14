/*
 * Copyright (c) 2015-2016 Alex Spataru <alex_spataru@outlook.com>
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
import Qt.labs.settings 1.0

import "../Widgets"
import "../Globals.js" as Globals

Window {
    id: window

    //
    // Window size
    //
    width: minimumWidth
    height: minimumHeight
    minimumWidth: Globals.scale (460)
    maximumWidth: Globals.scale (460)
    minimumHeight: Globals.scale (320)
    maximumHeight: Globals.scale (320)

    //
    // Default window position
    //
    x: (Screen.width - width) / 4
    y: (Screen.height - height) / 4

    //
    // Window properties
    //
    visible: false
    color: Globals.Colors.WindowBackground
    title: qsTr ("Virtual Joystick Options")

    //
    // Set window flags
    //
    flags: Qt.Window |
           Qt.WindowTitleHint |
           Qt.WindowSystemMenuHint |
           Qt.WindowCloseButtonHint |
           Qt.WindowMinimizeButtonHint

    //
    // Save settings
    //
    Settings {
        category: "VirtualJoystick"
        property alias x: window.x
        property alias y: window.y
        property alias range: spin.value
        property alias enabled: checkbox.checked
    }

    //
    // Apply saved settings into QJoysticks
    //
    Component.onCompleted: {
        QJoysticks.setVirtualJoystickRange (spin.value / 100)
        QJoysticks.setVirtualJoystickEnabled (checkbox.checked)
    }

    //
    // Window controls
    //
    ColumnLayout {
        anchors.fill: parent
        spacing: Globals.spacing
        anchors.margins: Globals.spacing

        Label {
            text: qsTr ("Keyboard assignments") + ":"
        }

        //
        // Keyboard assignments tables
        //
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: Globals.spacing

            //
            // Function table
            //
            TextEditor {
                enabled: false
                Layout.fillWidth: true
                Layout.fillHeight: true

                Component.onCompleted: {
                    editor.append (qsTr ("Joystick Buttons"))
                    editor.append (qsTr ("Primary Thumb (axes 0 and 1)"))
                    editor.append (qsTr ("Left Trigger (axis 2)"))
                    editor.append (qsTr ("Right Trigger (axis 3)"))
                    editor.append (qsTr ("Secondary Thumb (axes 4 and 5)"))
                    editor.append (qsTr ("POV Hat"))
                }
            }

            //
            // Keyboard keys table
            //
            TextEditor {
                enabled: false
                Layout.fillWidth: true
                Layout.fillHeight: true

                Component.onCompleted: {
                    editor.append (qsTr ("0,1,2,3,4,5,6,7,8,9"))
                    editor.append (qsTr ("W,A,S,D"))
                    editor.append (qsTr ("Q & E"))
                    editor.append (qsTr ("U & O"))
                    editor.append (qsTr ("I,J,K,L"))
                    editor.append (qsTr ("Arrows"))
                }
            }
        }

        Label {
            text: qsTr ("Axis range") + ":"
        }

        //
        // Axis range control
        //
        Spinbox {
            id: spin
            value: 100
            minimumValue: 0
            maximumValue: 100
            Layout.fillWidth: true
            onValueChanged: QJoysticks.setVirtualJoystickRange (value / 100)
        }

        //
        // Close button & enable virtual joystick checkbox
        //
        RowLayout {
            Layout.fillWidth: true
            spacing: Globals.spacing

            //
            // Enables or disables the virtual joystick
            //
            Checkbox {
                id: checkbox
                checked: false
                Layout.fillWidth: true
                text: qsTr ("Use my keyboard as a joystick")
                backgroundColor: Globals.Colors.WidgetBackground
                onCheckedChanged: QJoysticks.setVirtualJoystickEnabled (checked)
            }

            //
            // Close button
            //
            Button {
                onClicked: close()
                text: qsTr ("Close")
            }
        }
    }
}
