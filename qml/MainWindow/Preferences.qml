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

    Settings {
        property alias delay: delay.value
        property alias teleop: teleop.value
        property alias team: teamNumber.value
        property alias endGame: endGame.value
        property alias countdown: countdown.value
        property alias autonomous: autonomous.value
        property alias protocol: protocol.currentIndex
        property alias dashbaord: dashboard.currentIndex
    }

    //
    // Open the dashboard on application launch
    //
    Component.onCompleted: cDashboard.openDashboard (dashboard.currentIndex)

    //
    // Team number, dashboard & protocol selector
    //
    ColumnLayout {
        spacing: Globals.scale (5)
        Layout.maximumWidth: parent.width * 0.5

        //
        // Team number label
        //
        Label {
            font.bold: true
            text: qsTr ("Team Number") + ":"
        }

        //
        // Team number control
        //
        Spinbox {
            id: teamNumber
            minimumValue: 0
            maximumValue: 9999
            Layout.fillWidth: true
            value: DS.teamNumber
            onValueChanged: DS.teamNumber = value
        }

        //
        // Dashboard label
        //
        Label {
            font.bold: true
            text: qsTr ("Dashboard Type") + ":"
        }

        //
        // Dashboard selector
        //
        Combobox {
            id: dashboard
            Layout.fillWidth: true
            model: cDashboard.dashboardList()
            onCurrentIndexChanged: cDashboard.openDashboard (dashboard.currentIndex)
        }

        //
        // Vertical Spacer
        //
        Item {
            Layout.fillHeight: true
        }

        //
        // Protocol label
        //
        Label {
            font.bold: true
            text: qsTr ("Protocol") + ":"
        }

        //
        // Protocol selector
        //
        Combobox {
            id: protocol
            Layout.fillWidth: true
            model: DS.protocols
            onCurrentIndexChanged: DS.setProtocol (currentIndex)
        }
    }

    //
    // Horizontal spacer
    //
    Item {
        Layout.fillWidth: true
    }

    //
    // Practice timings
    //
    ColumnLayout {
        Layout.fillWidth: true
        spacing: Globals.spacing
        Layout.maximumWidth: Globals.scale (150)

        //
        // Title label
        //
        Label {
            font.bold: true
            text: qsTr ("Practice Timings") + ":"
        }

        //
        // Practice timings controls & labels
        //
        GridLayout {
            columns: 2
            rowSpacing: Globals.scale (1)
            columnSpacing: Globals.spacing

            Label {
                Layout.fillWidth: true
                text: qsTr ("Countdown")
            }

            //
            // Countdown control
            //
            Spinbox {
                value: 5
                id: countdown
                minimumValue: 0
                maximumValue: 150
                Layout.minimumWidth: Globals.scale (36)
            }

            Label {
                Layout.fillWidth: true
                text: qsTr ("Autonomous")
            }

            //
            // Autonomous period
            //
            Spinbox {
                value: 15
                id: autonomous
                minimumValue: 0
                maximumValue: 150
                Layout.minimumWidth: Globals.scale (36)
            }

            Label {
                Layout.fillWidth: true
                text: qsTr ("Delay")
            }

            //
            // Delay timings
            //
            Spinbox {
                value: 1
                id: delay
                minimumValue: 0
                maximumValue: 150
                Layout.minimumWidth: Globals.scale (36)
            }

            Label {
                Layout.fillWidth: true
                text: qsTr ("Teleoperated")
            }

            //
            // Teleop period
            //
            Spinbox {
                value: 100
                id: teleop
                minimumValue: 0
                maximumValue: 150
                Layout.minimumWidth: Globals.scale (36)
            }

            Label {
                Layout.fillWidth: true
                text: qsTr ("End Game")
            }

            //
            // End game warning
            //
            Spinbox {
                value: 20
                id: endGame
                minimumValue: 0
                maximumValue: 150
                Layout.minimumWidth: Globals.scale (36)
            }
        }

        //
        // Put a vertical spacer to compact the controls
        //
        Item {
            Layout.fillHeight: true
        }
    }

    //
    // Yet another horizontal spacer to keep things tidy
    //
    Item {
        Layout.fillWidth: true
    }
}
