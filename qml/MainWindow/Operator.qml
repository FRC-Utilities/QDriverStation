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

import DriverStation 1.0

import "../Widgets"
import "../Globals.js" as Globals

RowLayout {
    spacing: Globals.spacing
    signal windowModeChanged (var isDocked)

    //
    // Control modes & enable/disable buttons
    //
    ColumnLayout {
        Layout.fillHeight: true
        Layout.minimumWidth: Globals.scale (160)
        Layout.maximumWidth: Globals.scale (160)

        //
        // Control mode selector
        //
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: Globals.scale (-1)

            Button {
                Layout.fillWidth: true
                text: "  " + qsTr ("Teleoperated")
                caption.horizontalAlignment: Text.AlignLeft
                checked: DS.controlMode === LibDS.ControlTeleoperated
                onClicked: DS.controlMode = LibDS.ControlTeleoperated
            }

            Button {
                Layout.fillWidth: true
                text: "  " + qsTr ("Autonomous")
                caption.horizontalAlignment: Text.AlignLeft
                checked: DS.controlMode === LibDS.ControlAutonomous
                onClicked: DS.controlMode = LibDS.ControlAutonomous
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
                caption.horizontalAlignment: Text.AlignLeft
                checked: DS.controlMode === LibDS.ControlTest
                onClicked: DS.controlMode = LibDS.ControlTest
            }
        }

        //
        // Vertical spacer
        //
        Item {
            Layout.fillHeight: true
        }

        //
        // Enable/Disable buttons
        //
        RowLayout {
            Layout.fillWidth: true
            spacing: Globals.scale (-1)

            Button {
                checked: DS.enabled
                text: qsTr ("Enable")
                Layout.fillWidth: true
                Layout.fillHeight: true
                caption.font.bold: true
                onClicked: DS.enabled = DS.canBeEnabled
                caption.font.pixelSize: Globals.scale (16)
                caption.color: checked ? Globals.Colors.EnableButtonSelected :
                                         Globals.Colors.EnableButtonUnselected
            }

            Button {
                checked: !DS.enabled
                text: qsTr ("Disable")
                Layout.fillWidth: true
                Layout.fillHeight: true
                caption.font.bold: true
                onClicked: DS.enabled = false
                caption.font.pixelSize: Globals.scale (16)
                caption.color: checked ? Globals.Colors.DisableButtonSelected :
                                         Globals.Colors.DisableButtonUnselected
            }
        }
    }

    //
    // Small sepatator between core operator and information indicators
    //
    Rectangle {
        color: "#000"
        Layout.fillHeight: true
        width: Globals.scale (1)
    }

    //
    // Extra information indicators
    //
    GridLayout {
        columns: 2
        Layout.fillWidth: true
        Layout.fillHeight: true
        rowSpacing: Globals.scale (4)
        columnSpacing: Globals.spacing

        //
        // Elapsed time label
        //
        Label {
            Layout.fillWidth: true
            text: qsTr ("Elapsed Time")
            horizontalAlignment: Text.AlignRight
        }

        //
        // Elapsed time indicator
        //
        Label {
            font.bold: true
            Layout.fillWidth: true
            size: Globals.scale (18)
            text: DS.elapsedTime
            Layout.rightMargin: Globals.spacing
            horizontalAlignment: Text.AlignRight
        }

        //
        // Spacer (for both columns)
        //
        Item {
            Layout.fillHeight: true
        }
        Item {
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
            Layout.fillHeight: true
        }
        Item {
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
            Layout.fillHeight: true
        }
        Item {
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
            id: stations
            model: DS.stations
            Layout.fillWidth: true
            onCurrentIndexChanged: DS.station = currentIndex
        }
    }
}
