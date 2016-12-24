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

import "../Widgets"
import "../Globals.js" as Globals

RowLayout {
    spacing: Globals.spacing

    //
    // Network diagnostics items
    //
    ColumnLayout {
        Layout.fillHeight: true
        spacing: Globals.spacing
        Layout.maximumWidth: Globals.scale (240)

        Label {
            font.bold: true
            text: qsTr ("Network Diagnostics") + ":"
        }

        ColumnLayout {
            spacing: Globals.scale (1)

            Checkbox {
                enabled: false
                text: qsTr ("FMS")
                checked: DS.connectedToFMS
            }

            Checkbox {
                enabled: false
                text: qsTr ("Robot")
                checked: DS.connectedToRobot
            }

            Checkbox {
                enabled: false
                text: qsTr ("Bridge/Radio")
                checked: DS.connectedToRadio
            }
        }

        Item {
            Layout.fillHeight: true
        }

        ColumnLayout {
            spacing: Globals.scale (-1)

            Button {
                Layout.fillWidth: true
                text: qsTr ("Reboot RIO")
                onClicked: DS.rebootRobot()
            }

            Button {
                Layout.fillWidth: true
                text: qsTr ("Restart Code")
                onClicked: DS.restartRobotCode()
            }
        }
    }

    //
    // Horizontal spacer
    //
    Item {
        Layout.fillWidth: true
    }

    //
    // Robot information items (labels & indicators)
    //
    ColumnLayout {
        Layout.fillHeight: true
        spacing: Globals.spacing

        Label {
            font.bold: true
            text: qsTr ("Robot Information") + ":"
        }

        Grid {
            columns: 2
            Layout.fillHeight: true
            rowSpacing: Globals.scale (1)
            columnSpacing: Globals.spacing

            Label {
                text: qsTr ("CPU Usage")
            }

            Label {
                text: DS.connectedToRobot ? DS.cpuUsage + " %" : Globals.invalidStr
            }

            Label {
                text: qsTr ("RAM Usage")
            }

            Label {
                text: DS.connectedToRobot ? DS.ramUsage + " %" : Globals.invalidStr
            }

            Label {
                text: qsTr ("Disk Usage")
            }

            Label {
                text: DS.connectedToRobot ? DS.diskUsage+ " %" : Globals.invalidStr
            }

            Label {
                text: qsTr ("CAN Utilization")
            }

            Label {
                text: DS.connectedToRobot ? DS.canUsage+ " %" : Globals.invalidStr
            }
        }
    }

    //
    // Another spacer
    //
    Item {
        Layout.fillWidth: true
    }
}
