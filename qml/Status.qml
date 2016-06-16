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

import "widgets"
import "globals.js" as Globals

ColumnLayout {
    id: status

    //
    // Holds the number of flashes to perform during the error animation
    //
    property int flashes: 0

    //
    // Defines the separation between the different components of the widget
    //
    property double spacerHeight: Globals.spacing / 2

    //
    // These properties are updated when the DS emits the appropiate signals
    //
    property bool simulated: false
    property string teamNumber: DriverStation.team()
    property string robotVoltage: Globals.invalidStr
    property string robotStatus: qsTr ("Loading") + "..."

    //
    // Used to perfom the error animation when the user tries to enable
    // the robot when the DS cannot enable the robot
    //
    property bool red: false
    function toggleColors() {
        var color
        red = !red

        if (!red)
            color = Globals.Colors.IndicatorError
        else
            color = Globals.Colors.Foreground

        rstatus.color = color

        if (!codeLed.checked)
            codeLed.textColor = color

        if (!communicationsLed.checked)
            communicationsLed.textColor = color
    }

    //
    // Used for the error animation described above
    //
    Timer {
        id: timer
        interval: 100

        onTriggered: {
            flashes -= 1

            if (flashes > 0)
                toggleColors()

            if (flashes <= 0) {
                repeat = false
                rstatus.color = Globals.Colors.Foreground
                codeLed.textColor = Globals.Colors.Foreground
                communicationsLed.textColor = Globals.Colors.Foreground
            }
        }
    }

    //
    // Uses the timer and the color toggle functions to generate the error animation
    //
    function flashStatusIndicators() {
        flashes = 8
        timer.repeat = true
        timer.start()

        Globals.morse ("..-", 440, true)
        Globals.beep (220, 100)
    }

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
    // Check if there is already a joystick connected to the computer
    //
    Component.onCompleted: joysticksLed.checked = QJoysticks.count() > 0

    //
    // Update the joystick LED when a joystick is attached or removed
    //
    Connections {
        target: QJoysticks
        onCountChanged: joysticksLed.checked = QJoysticks.nonBlacklistedCount() > 0
    }

    //
    // Update the robot status, communications LED, code LED and voltage indicator
    // when the DriverStation emits the corresponding signal
    //
    Connections {
        target: DriverStation
        onTeamChanged: teamNumber = team
        onStatusChanged: robotStatus = status
        onSimulatedChanged: simulated = isSimulation
        onCodeStatusChanged: codeLed.checked = DriverStation.isRobotCodeRunning()
        onRobotCommStatusChanged: communicationsLed.checked = DriverStation.isConnectedToRobot()

        onVoltageChanged: {
            if (DriverStation.isConnectedToRobot())
                robotVoltage = voltageString
            else
                robotVoltage = Globals.invalidStr
        }

        onProtocolChanged: robotVoltage = Globals.invalidStr
    }

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
            text: status.teamNumber
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
    // Simulated Robot label
    //
    Label {
        size: large
        font.bold: true
        visible: simulated
        Layout.fillWidth: true
        Layout.fillHeight: true
        text: qsTr ("Simulated Robot")
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        Layout.minimumHeight: Globals.scale (34)
        color: Globals.Colors.AlternativeHighlight
    }

    //
    // Voltage indicator & plot
    //
    RowLayout {
        visible: !simulated
        spacing: Globals.spacing
        Layout.minimumHeight: Globals.scale (42)

        //
        // Voltage widget
        //
        Item {
            width: Globals.scale (53)
            height: Globals.scale (32)

            property var diodeHeight: height * 0.20
            property var backgroundColor: Globals.Colors.IconColor

            Rectangle {
                id: cover
                height: Globals.scale (4)
                color: parent.backgroundColor

                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.topMargin: parent.diodeHeight
            }

            Rectangle {
                id: leftDiode
                width: parent.width * 0.2
                color: parent.backgroundColor
                height: parent.diodeHeight - Globals.scale (2)

                anchors.top: parent.top
                anchors.left: parent.left
                anchors.leftMargin: cover.height
            }

            Rectangle {
                id: rightDiode
                width: parent.width * 0.2
                color: parent.backgroundColor
                height: parent.diodeHeight - Globals.scale (2)

                anchors.top: parent.top
                anchors.right: parent.right
                anchors.rightMargin: cover.height
            }

            Rectangle {
                id: base
                anchors.fill: parent
                color: parent.backgroundColor
                anchors.leftMargin: Globals.scale (2)
                anchors.rightMargin: Globals.scale (2)
                anchors.topMargin: parent.diodeHeight + (cover.height / 2)

                VoltageGraph {
                    color: parent.color
                    anchors.fill: parent
                    border.color: parent.color
                    anchors.margins: Globals.scale (2)
                    anchors.topMargin: Globals.scale (0)
                    noCommsColor: Globals.Colors.WindowBackground

                    Component.onCompleted: setSpeed (24)
                }
            }
        }

        Item {
            Layout.fillWidth: true
        }

        Label {
            size: large
            font.bold: true
            id: voltageIndicator
            text: status.robotVoltage
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
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
            leftToRight: true
            id: communicationsLed
            anchors.right: parent.right
            text: qsTr ("Communications")
        }

        LED {
            id: codeLed
            leftToRight: true
            text: qsTr ("Robot Code")
            anchors.right: parent.right
        }

        LED {
            id: joysticksLed
            leftToRight: true
            text: qsTr ("Joysticks")
            anchors.right: parent.right
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
        id: rstatus
        size: large
        font.bold: true
        Layout.fillWidth: true
        Layout.fillHeight: true
        text: status.robotStatus
        verticalAlignment: Qt.AlignVCenter
        horizontalAlignment: Qt.AlignHCenter
        Layout.minimumHeight: Globals.scale (42)
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
    }
}
