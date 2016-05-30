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

Item {
    id: status

    //
    // Holds the number of flashes to perform during the error animation
    //
    property int flashes: 0

    //
    // These properties are updated when the DS emits the appropiate signals
    //
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

        Globals.morse ("..-", 440)
        Globals.beep  (220, 150)
    }

    //
    // Ensure that the width of the controls is not to small but not too large
    //
    implicitWidth: Math.max (childrenRect.width, Globals.scale (166))

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
        onProtocolChanged: robotVoltage = Globals.invalidStr
        onCodeStatusChanged: codeLed.checked = DriverStation.isRobotCodeRunning()
        onRobotCommStatusChanged: communicationsLed.checked = DriverStation.isConnectedToRobot()
        onVoltageChanged: robotVoltage = DriverStation.isConnectedToRobot() ?
                              voltageString : Globals.invalidStr
    }

    //
    // The actual controls
    //
    ColumnLayout {
        spacing: 0
        anchors.fill: parent
        anchors.margins: Globals.spacing

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
                Layout.fillWidth: true
                text: status.teamNumber
                verticalAlignment: Qt.AlignVCenter
                horizontalAlignment: Qt.AlignHCenter
            }
        }

        //
        // Spacer
        //
        Item {
            width: height
            height: Globals.scale (5)
        }

        //
        // Voltage indicator
        //
        RowLayout {
            spacing: Globals.spacing

            Icon {
                font.bold: true
                Layout.fillWidth: true
                name: icons.fa_battery
                size: Globals.scale (42)
                horizontalAlignment: Text.AlignLeft

                color: mouse.containsMouse ?
                           Globals.Colors.AlternativeHighlight :
                           Globals.Colors.Foreground

                Behavior on color {
                    ColorAnimation {
                        duration: Globals.slowAnimation
                    }
                }
            }

            Label {
                size: large
                font.bold: true
                Layout.fillWidth: true
                text: status.robotVoltage
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter

                color: mouse.containsMouse ?
                           Globals.Colors.AlternativeHighlight :
                           Globals.Colors.Foreground

                Behavior on color {
                    ColorAnimation {
                        duration: Globals.slowAnimation
                    }
                }
            }

            MouseArea {
                id: mouse
                hoverEnabled: true
                anchors.fill: parent
                onClicked: powerWindow.show()
            }
        }

        //
        // Another spacer
        //
        Item {
            width: height
            Layout.fillHeight: true
            height: Globals.scale (10)
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
        // Yet another spacer
        //
        Item {
            width: height
            height: Globals.scale (10)
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
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }
    }

    PowerWindow {
        id: powerWindow
    }
}
