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

import "widgets"
import "globals.js" as Globals

Window {
    id: window

    //
    // Window size
    //
    width: minimumWidth
    height: minimumHeight
    minimumWidth: Globals.scale (640)
    minimumHeight: Globals.scale (420)

    //
    // Returns a list with the robot log names
    //
    function logs() {
        return DriverStation.availableLogs().reverse()
    }

    //
    // Returns a formatted name for the given log
    //
    function niceName (log) {
        log = log.replace (/_/gi, ':')
        log = log.replace(/.qdslog/gi, '')

        return log
    }

    //
    // Default window position
    //
    x: (Screen.width - width) / 4
    y: (Screen.height - height) / 4

    //
    // Window properties
    //
    visible: false
    title: qsTr ("Driver Station Logs")
    color: Globals.Colors.WindowBackground

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
        category: "Logs Window"
        property alias x: window.x
        property alias y: window.y
        property alias width: window.width
        property alias height: window.height
    }

    //
    // Main layout
    //
    RowLayout {
        anchors.fill: parent
        spacing: Globals.spacing
        anchors.margins: Globals.spacing

        //
        // Navigator bar
        //
        ColumnLayout {
            Layout.fillWidth: false
            Layout.fillHeight: true
            spacing: Globals.spacing

            //
            // Choose log file caption & button
            //
            RowLayout {
                Label {
                    Layout.fillWidth: true
                    text: qsTr ("Choose log file") + ": "
                    anchors.verticalCenter: parent.verticalCenter
                }

                Button {
                    icon: icons.fa_folder_open_o
                    onClicked: DriverStation.openLogsPath()
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            //
            // Log selector
            //
            TextEditor {
                Layout.fillWidth: true
                Layout.fillHeight: true
                backgroundColor: Globals.Colors.PanelBackground
                foregroundColor: Globals.Colors.WidgetForeground

                Component.onCompleted: {
                    for (var i = 0; i < logs().length; ++i)
                        editor.append (niceName (logs()[i]))
                }
            }
        }

        //
        // Log charts
        //
        Panel {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ColumnLayout {
                anchors.fill: parent
                spacing: Globals.spacing
                anchors.margins: Globals.spacing

                //
                // Log type selector
                //
                RowLayout {
                    Layout.fillWidth: true

                    Label {
                        Layout.fillWidth: true
                        text: qsTr ("Select Category") + ":"
                    }

                    Button {
                        id: eventsBt
                        checked: true
                        text: qsTr ("Event Charts")

                        onClicked: {
                            eventsBt.checked = true
                            consoleBt.checked = false

                            eventCharts.visible = true
                            consoleLogs.visible = false
                        }
                    }

                    Button {
                        id: consoleBt
                        text: qsTr ("Console Logs")

                        onClicked: {
                            eventsBt.checked = false
                            consoleBt.checked = true

                            consoleLogs.visible = true
                            eventCharts.visible = false
                        }
                    }
                }

                //
                // Event charts
                //
                RowLayout {
                    id: eventCharts
                    visible: true
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    spacing: Globals.spacing

                    //
                    // Colors
                    //
                    property string pktColor: "blue"
                    property string cpuColor: "red"
                    property string ramColor: "orange"
                    property string volColor: "yellow"

                    //
                    // Plot
                    //
                    Plot {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }

                    //
                    // Legends
                    //
                    ColumnLayout {
                        Layout.fillWidth: false
                        Layout.fillHeight: true
                        spacing: Globals.spacing

                        LED {
                            text: qsTr ("Lost Packets")
                            unpoweredColor: eventCharts.pktColor
                        }

                        LED {
                            text: qsTr ("CPU") + " %"
                            unpoweredColor: eventCharts.cpuColor
                        }

                        LED {
                            text: qsTr ("RAM") + " %"
                            unpoweredColor: eventCharts.ramColor
                        }

                        LED {
                            text: qsTr ("Voltage") + " %"
                            unpoweredColor: eventCharts.volColor
                        }
                    }
                }

                //
                // Console logs
                //
                TextEditor {
                    id: consoleLogs
                    visible: false
                    editor.readOnly: true
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    editor.textFormat: Text.PlainText
                    editor.font.family: Globals.monoFont
                    editor.font.pixelSize: Globals.scale (11)
                    foregroundColor: Globals.Colors.WidgetForeground
                    backgroundColor: Globals.Colors.WindowBackground
                }
            }
        }
    }
}
