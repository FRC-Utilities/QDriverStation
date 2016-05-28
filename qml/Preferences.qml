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
import QtQuick.Layouts 1.0
import Qt.labs.settings 1.0

import "widgets"
import "globals.js" as Globals

RowLayout {
    spacing: Globals.spacing

    Settings {
        category: "DriverStationOptions"
        property alias delay: delay.value
        property alias teleop: teleop.value
        property alias team: teamNumber.value
        property alias endGame: endGame.value
        property alias countdown: countdown.value
        property alias autonomous: autonomous.value
        property alias protocol: protocol.currentIndex
        property alias dashbaord: dashboard.currentIndex
    }

    Component.onCompleted: {
        cDashboard.openDashboard (dashboard.currentIndex)
    }

    Connections {
        target: DriverStation
        onTeamChanged: if (teamNumber.value !== team) teamNumber.value = team
    }

    ColumnLayout {
        spacing: Globals.scale (5)
        Layout.maximumWidth: parent.width * 0.5

        Label {
            font.bold: true
            text: qsTr ("Team Number") + ":"
        }

        Spinbox {
            id: teamNumber
            minimumValue: 0
            maximumValue: 9999
            Layout.fillWidth: true
            value: DriverStation.team()
            onValueChanged: DriverStation.setTeam (value)
        }

        Label {
            font.bold: true
            text: qsTr ("Dashboard Type") + ":"
        }

        Combobox {
            id: dashboard
            Layout.fillWidth: true
            model: cDashboard.dashboardList()
            onCurrentIndexChanged: cDashboard.openDashboard (dashboard.currentIndex)
        }

        Item {
            Layout.fillHeight: true
        }

        Label {
            font.bold: true
            text: qsTr ("Protocol") + ":"
        }

        Combobox {
            id: protocol
            Layout.fillWidth: true
            model: DriverStation.protocols()
            onCurrentIndexChanged: DriverStation.setProtocolType (currentIndex)
        }
    }

    Item {
        Layout.fillWidth: true
    }

    ColumnLayout {
        Layout.fillWidth: true
        spacing: Globals.spacing
        Layout.maximumWidth: Globals.scale (150)

        Label {
            font.bold: true
            text: qsTr ("Practice Timings") + ":"
        }

        GridLayout {
            columns: 2
            rowSpacing: Globals.scale (1)
            columnSpacing: Globals.spacing

            Label {
                Layout.fillWidth: true
                text: qsTr ("Countdown")
            }

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

            Spinbox {
                value: 20
                id: endGame
                minimumValue: 0
                maximumValue: 150
                Layout.minimumWidth: Globals.scale (36)
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }

    Item {
        Layout.fillWidth: true
    }
}
