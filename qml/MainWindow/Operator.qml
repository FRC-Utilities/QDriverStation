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
import QtQuick.Layouts 1.0
import Qt.labs.settings 1.0

import "../Widgets"
import "../Globals.js" as Globals

RowLayout {
    spacing: Globals.spacing
    signal windowModeChanged (var isDocked)

    ColumnLayout {
        Layout.fillHeight: true
        Layout.maximumWidth: Globals.scale (160)

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: Globals.scale (-1)

            Button {
                Layout.fillWidth: true
                text: "  " + qsTr ("Teleoperated")
                checked: DriverStation.controlMode === 2
                onClicked: DriverStation.controlMode = 2
                caption.horizontalAlignment: Text.AlignLeft
            }

            Button {
                Layout.fillWidth: true
                text: "  " + qsTr ("Autonomous")
                checked: DriverStation.controlMode === 1
                onClicked: DriverStation.controlMode = 1
                caption.horizontalAlignment: Text.AlignLeft
            }

            Button {
                enabled: false
                Layout.fillWidth: true
                text: "  " + qsTr ("Practice")
                caption.horizontalAlignment: Text.AlignLeft
            }

            Button {
                Layout.fillWidth: true
                text: "  " + qsTr ("Test")
                checked: DriverStation.controlMode === 0
                onClicked: DriverStation.controlMode = 0
                caption.horizontalAlignment: Text.AlignLeft
            }
        }

        Item {
            Layout.fillHeight: true
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: Globals.scale (-1)

            Button {
                text: qsTr ("Enable")
                Layout.fillWidth: true
                Layout.fillHeight: true
                caption.font.bold: true
                checked: DriverStation.enabled
                caption.font.pixelSize: Globals.scale (16)
                onClicked: DriverStation.enabled = DriverStation.canBeEnabled
                caption.color: checked ? Globals.Colors.EnableButtonSelected :
                                         Globals.Colors.EnableButtonUnselected
            }

            Button {
                text: qsTr ("Disable")
                Layout.fillWidth: true
                Layout.fillHeight: true
                caption.font.bold: true
                checked: !DriverStation.enabled
                onClicked: DriverStation.enabled = false
                caption.font.pixelSize: Globals.scale (16)
                caption.color: checked ? Globals.Colors.DisableButtonSelected :
                                         Globals.Colors.DisableButtonUnselected
            }
        }
    }

    Rectangle {
        color: "#000"
        Layout.fillHeight: true
        width: Globals.scale (1)
    }

    GridLayout {
        columns: 2
        Layout.fillWidth: true
        Layout.fillHeight: true
        rowSpacing: Globals.spacing
        columnSpacing: Globals.spacing

        Label {
            Layout.fillWidth: true
            text: qsTr ("Elapsed Time")
            horizontalAlignment: Text.AlignRight
        }

        Label {
            text: "00:00.0"
            font.bold: true
            Layout.fillWidth: true
            size: Globals.scale (18)
            Layout.rightMargin: Globals.spacing
            horizontalAlignment: Text.AlignRight
        }

        //
        // Spacer (for both columns)
        //
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        //
        // PC Battery label
        //
        RowLayout {
            Layout.fillWidth: true

            Icon {
                name: icons.fa_plug
                Layout.fillWidth: true
                size: Globals.scale (10)
                horizontalAlignment: Text.AlignRight
                opacity: Utilities.connectedToAC ? 1 : 0
                Behavior on opacity { NumberAnimation{} }
            }

            Label {
                text: qsTr ("PC Battery")
                horizontalAlignment: Text.AlignRight
            }
        }

        //
        // Battery progressbar
        //
        Progressbar {
            text: ""
            Layout.fillWidth: true
            value: Utilities.batteryLevel
            barColor: {
                if (value > 60)
                    return Globals.Colors.HighlightColor

                else if (value > 25)
                    return Globals.Colors.CPUProgress

                return Globals.Colors.LowBattery
            }
        }

        //
        // PC CPU label
        //
        Label {
            Layout.fillWidth: true
            text: qsTr ("PC CPU") + " %"
            horizontalAlignment: Text.AlignRight
        }

        //
        // CPU prgoressbar
        //
        Progressbar {
            text: ""
            Layout.fillWidth: true
            value: Utilities.cpuUsage
            barColor: Globals.Colors.CPUProgress
        }

        //
        // Spacer (for both columns)
        //
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        //
        // Window type label
        //
        Label {
            Layout.fillWidth: true
            text: qsTr ("Window")
            horizontalAlignment: Text.AlignRight
        }

        //
        // Dock/Normal buttons
        //
        RowLayout {
            Layout.fillWidth: true
            spacing: Globals.scale (-1)

            Button {
                id: normal
                checked: !mw.docked
                Layout.fillWidth: true
                icon: icons.fa_mail_reply
                width: Globals.scale (48)
                iconSize: Globals.scale (12)
                onClicked: {
                    normal.checked = true
                    docked.checked = false
                    windowModeChanged (false)
                }
            }

            Button {
                id: docked
                checked: mw.docked
                icon: icons.fa_expand
                Layout.fillWidth: true
                width: Globals.scale (48)
                iconSize: Globals.scale (12)
                onClicked: {
                    docked.checked = true
                    normal.checked = false
                    windowModeChanged (true)
                }
            }
        }

        //
        // Spacer (for both columns)
        //
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        //
        // Team station label
        //
        Label {
            Layout.fillWidth: true
            text: qsTr ("Team Station")
            horizontalAlignment: Text.AlignRight
        }

        //
        // Team station selector
        //
        Combobox {
            Layout.fillWidth: true
            model: DriverStation.stations
            onCurrentIndexChanged: DriverStation.station = currentIndex
        }
    }
}

