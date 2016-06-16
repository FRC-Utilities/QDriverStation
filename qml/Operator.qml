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

    //
    // If emitted, the status controls will blink to indicate an error
    //
    signal flashStatusIndicators

    //
    // Emitted when the window mode is changed
    //
    signal windowModeChanged (var isDocked)

    //
    // Save the dock state and the selected alliance
    //
    Settings {
        category: "Operator"
        property alias dockSelected: docked.checked
        property alias selectedAlliance: alliances.currentIndex
    }

    //
    // Update the UI when the DS emits a corresponding signal
    //
    Connections {
        target: DriverStation
        onEnabledChanged: enable.checked = DriverStation.isEnabled()
        onControlModeChanged: enable.checked = false
        onElapsedTimeChanged: elapsedTime.text = string
    }

    //
    // Query for CPU usage and battery level every second
    //
    Timer {
        repeat: true
        interval: 1000
        Component.onCompleted: start()

        onTriggered: {
            cpuProgressBar.value = cUtilities.getCpuUsage()
            plug.visible = cUtilities.isConnectedToPowerSource()
            batteryProgressBar.value = cUtilities.getBatteryLevel()
        }
    }

    //
    // Robot modes & enable/disable buttons
    //
    ColumnLayout {
        Layout.fillWidth: false
        Layout.fillHeight: true
        spacing: Globals.spacing
        Layout.minimumWidth: Globals.scale (155)
        Layout.maximumWidth: Globals.scale (155)

        //
        // Robot modes selector
        //
        Column {
            Layout.fillHeight: true
            spacing: Globals.scale (-1)

            anchors {
                left: parent.left
                right: parent.right
            }

            function uncheckEverything() {
                test.checked = false
                teleop.checked = false
                practice.checked = false
                autonomous.checked = false
            }

            //
            // Teleop selector
            //
            Button {
                id: teleop
                checked: true
                text: "  " + qsTr ("Teleoperated")
                anchors.left: parent.left
                anchors.right: parent.right
                caption.horizontalAlignment: Text.AlignLeft

                onClicked: {
                    parent.uncheckEverything()
                    checked = true

                    disable.checked = true
                    DriverStation.switchToTeleoperated()
                }
            }

            //
            // Autonomous selector
            //
            Button {
                id: autonomous
                text: "  " + qsTr ("Autonomous")
                anchors.left: parent.left
                anchors.right: parent.right
                caption.horizontalAlignment: Text.AlignLeft

                onClicked: {
                    parent.uncheckEverything()
                    checked = true

                    disable.checked = true
                    DriverStation.switchToAutonomous()
                }
            }

            //
            // Practice selector
            //
            Button {
                id: practice
                enabled: false
                anchors.left: parent.left
                anchors.right: parent.right
                text: "  " + qsTr ("Practice")
                caption.horizontalAlignment: Text.AlignLeft

                onClicked: {
                    parent.uncheckEverything()
                    checked = true
                }
            }

            //
            // Test mode selector
            //
            Button {
                id: test
                text: "  " + qsTr ("Test")
                anchors.left: parent.left
                anchors.right: parent.right
                caption.horizontalAlignment: Text.AlignLeft

                onClicked: {
                    parent.uncheckEverything()
                    checked = true

                    disable.checked = true
                    DriverStation.switchToTestMode()
                }
            }
        }

        Item {
            Layout.fillHeight: true
        }

        //
        // Enable/Disable buttons
        //
        RowLayout {
            spacing: Globals.scale (-1)

            anchors {
                left: parent.left
                right: parent.right
            }

            //
            // Enable button
            //
            Button {
                id: enable
                text: qsTr ("Enable")
                caption.font.bold: true
                implicitWidth: parent.width / 2
                caption.size: Globals.scale (14)
                implicitHeight: Globals.scale (48)
                caption.color: checked ? Globals.Colors.EnableButtonSelected :
                                         Globals.Colors.EnableButtonUnselected

                onClicked: {
                    if (DriverStation.canBeEnabled())
                        checked = true
                    else
                        flashStatusIndicators()
                }

                onCheckedChanged: {
                    disable.checked = !checked
                    DriverStation.setEnabled (checked)
                }
            }

            //
            // Disable button
            //
            Button {
                id: disable
                checked: true
                text: qsTr ("Disable")
                caption.font.bold: true
                implicitWidth: parent.width / 2
                caption.size: Globals.scale (14)
                implicitHeight: Globals.scale (48)
                caption.color: checked ? Globals.Colors.DisableButtonSelected :
                                         Globals.Colors.DisableButtonUnselected

                onClicked: checked = true

                onCheckedChanged: {
                    enable.checked = !checked
                    DriverStation.setEnabled (!checked)
                }
            }
        }
    }

    //
    // Separator line
    //
    Rectangle {
        Layout.fillHeight: true
        width: Globals.scale (1)
        color: Globals.Colors.WidgetBorder
    }

    //
    // The right controls (such as CPU/battery progress bars, window mode
    // and team station selectors). This is a mess, but this is nothing
    // compared to the Operator widget done with C++
    //
    GridLayout {
        id: grid
        columns: 2
        Layout.fillWidth: false
        Layout.fillHeight: true
        rowSpacing: Globals.spacing
        columnSpacing: Globals.spacing
        Layout.minimumWidth: minWidth * 2
        property int minWidth: Globals.scale (102)

        //
        // Elapsed time caption
        //
        Label {
            Layout.fillWidth: true
            text: qsTr ("Elapsed Time")
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignRight
        }

        //
        // Elapsed time indicator
        //
        Label {
            text: "00:00.0"
            id: elapsedTime
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
        } Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        //
        // PC Battery label & plug icon
        //
        RowLayout {
            Layout.fillWidth: true
            spacing: Globals.scale (5)
            
            Icon {
                id: plug
                name: icons.fa_plug
                Layout.fillWidth: true
                size: Globals.scale (10)
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignRight
            }

            Label {
                text: qsTr ("PC Battery")
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignRight
            }
        }

        //
        // Battery level indicator
        //
        Progressbar {
            id: batteryProgressBar

            text: ""
            value: 0
            Layout.fillWidth: true
            Layout.minimumWidth: grid.minWidth
            Layout.maximumWidth: grid.minWidth
            barColor: {
                if (value > 60)
                    return Globals.Colors.HighlightColor

                else if (value > 25)
                    return Globals.Colors.CPUProgress

                return Globals.Colors.LowBattery
            }
        }

        //
        // CPU caption
        //
        Label {
            Layout.fillWidth: true
            text: qsTr ("PC CPU") + " %"
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignRight
        }

        //
        // CPU Usage Indicator
        //
        Progressbar {
            id: cpuProgressBar

            text: ""
            value: 0
            Layout.fillWidth: true
            barColor: Globals.Colors.CPUProgress
            Layout.minimumWidth: grid.minWidth
            Layout.maximumWidth: grid.minWidth
        }

        //
        // Spacer (for both columns)
        //
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
        } Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        //
        // Window mode label
        //
        Label {
            text: qsTr ("Window")
            Layout.fillWidth: true
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignRight
        }

        //
        // Window mode buttons
        //
        RowLayout {
            Layout.fillWidth: true
            spacing: Globals.scale (-1)

            //
            // Normal window button
            //
            Button {
                id: normal
                checked: true
                Layout.fillWidth: true
                icon: icons.fa_mail_reply
                height: Globals.scale (24)
                iconSize: Globals.scale (12)
                Layout.minimumWidth: grid.minWidth / 2
                Layout.maximumWidth: grid.minWidth / 2

                onClicked: {
                    normal.checked = true
                    docked.checked = false
                    windowModeChanged (docked.checked)
                }

                onCheckedChanged: docked.checked = !checked
            }

            //
            // Docked window button
            //
            Button {
                id: docked
                icon: icons.fa_expand
                Layout.fillWidth: true
                height: Globals.scale (24)
                iconSize: Globals.scale (12)
                Layout.minimumWidth: grid.minWidth / 2
                Layout.maximumWidth: grid.minWidth / 2

                onClicked: {
                    docked.checked = true
                    normal.checked = false
                    windowModeChanged (docked.checked)
                }

                onCheckedChanged: normal.checked = !checked
            }
        }

        //
        // Team station caption
        //
        Label {
            Layout.fillWidth: true
            text: qsTr ("Team Station")
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignRight
        }

        //
        // Team station selector
        //
        Combobox {
            id: alliances
            Layout.fillWidth: true
            height: Globals.scale (24)
            Layout.minimumWidth: grid.minWidth
            Layout.maximumWidth: grid.minWidth
            model: DriverStation.teamStations()
            onCurrentIndexChanged: DriverStation.setTeamStation (currentIndex)
        }
    }
}
