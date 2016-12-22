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

ColumnLayout {
    id: status

    //
    // Defines the separation between the different components of the widget
    //
    property double spacerHeight: Globals.spacing / 2

    //
    // Layout/geometry options
    //
    spacing: 0
    Layout.margins: 0
    Layout.leftMargin: Globals.spacing
    Layout.rightMargin: Globals.spacing
    Layout.minimumWidth: Globals.scale (144)
    Layout.preferredWidth: Globals.scale (144)

    //
    // Team number indicator
    //
    RowLayout {
        spacing: Globals.spacing

        Label {
            size: large
            font.bold: true
            Layout.fillWidth: true
            text: qsTr ("Team") + " #"
            verticalAlignment: Qt.AlignCenter
        }

        Label {
            size: large
            font.bold: true
            text: DriverStation.teamNumber
            verticalAlignment: Qt.AlignVCenter
            horizontalAlignment: Qt.AlignHCenter
        }
    }

    //
    // Spacer
    //
    Item {
        Layout.fillHeight: true
        Layout.minimumHeight: spacerHeight
    }

    //
    // Voltage indicator & plot
    //
    RowLayout {
        spacing: Globals.spacing
        Layout.minimumHeight: Globals.scale (42)

        //
        // Robot battery graph
        //
        Item {
            width: Globals.scale (53)
            height: Globals.scale (32)

            property var diodeHeight: height * 0.20
            property var backgroundColor: Globals.Colors.IconColor

            //
            // Battery cover
            //
            Rectangle {
                id: cover
                height: Globals.scale (4)
                color: parent.backgroundColor

                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                    topMargin: parent.diodeHeight
                }
            }

            //
            // Left diode
            //
            Rectangle {
                width: parent.width * 0.2
                color: parent.backgroundColor
                height: parent.diodeHeight - Globals.scale (2)

                anchors {
                    top: parent.top
                    left: parent.left
                    leftMargin: cover.height
                }
            }

            //
            // Right diode
            //
            Rectangle {
                width: parent.width * 0.2
                color: parent.backgroundColor
                height: parent.diodeHeight - Globals.scale (2)

                anchors {
                    top: parent.top
                    right: parent.right
                    rightMargin: cover.height
                }
            }

            //
            // Base
            //
            Rectangle {
                color: parent.backgroundColor

                anchors {
                    fill: parent
                    leftMargin: Globals.scale (2)
                    rightMargin: Globals.scale (2)
                    topMargin: parent.diodeHeight + (cover.height / 2)
                }

                VoltageGraph {
                    border.color: parent.color
                    Component.onCompleted: setSpeed (24)
                    color: Globals.Colors.WindowBackground
                    noCommsColor: Globals.Colors.WindowBackground

                    anchors {
                        fill: parent
                        topMargin: 0
                        margins: Globals.scale (2)
                    }
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
        // Voltage text (finally)
        //
        Label {
            size: large
            font.bold: true
            id: voltageIndicator
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            text: DriverStation.connectedToRobot ? DriverStation.voltageString :
                                                   Globals.invalidStr
        }
    }

    //
    // Another spacer
    //
    Item {
        Layout.fillHeight: true
        Layout.minimumHeight: spacerHeight
    }

    //
    // Status LEDs
    //
    Column {
        Layout.fillWidth: true
        spacing: Globals.scale (3)

        LED {
            id: commsLed
            leftToRight: true
            anchors.right: parent.right
            text: qsTr ("Communications")
            checked: DriverStation.connectedToRobot
        }

        LED {
            id: codeLed
            leftToRight: true
            text: qsTr ("Robot Code")
            anchors.right: parent.right
            checked: DriverStation.robotCode
        }

        LED {
            leftToRight: true
            text: qsTr ("Joysticks")
            anchors.right: parent.right
            checked: QJoysticks.count() > 0
        }
    }

    //
    // (Yet) Another spacer
    //
    Item {
        Layout.fillHeight: true
        Layout.minimumHeight: spacerHeight
    }

    //
    // Robot status
    //
    Label {
        size: large
        font.bold: true
        Layout.fillWidth: true
        Layout.fillHeight: true
        text: DriverStation.status
        verticalAlignment: Qt.AlignVCenter
        horizontalAlignment: Qt.AlignHCenter
        Layout.minimumHeight: Globals.scale (42)
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
    }
}
