/*
 * Copyright (c) 2015-2020 Alex Spataru <alex_spataru@outlook.com>
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

import "../Widgets"
import "../Globals.js" as Globals

Window {
    id: window
    visible: false
    width: minimumWidth
    height: minimumHeight
    title: qsTr ("Settings")
    minimumWidth: Globals.scale (420)
    maximumWidth: Globals.scale (420)
    minimumHeight: Globals.scale (340)
    maximumHeight: Globals.scale (340)
    color: Globals.Colors.WindowBackground

    //
    // Used to obtain the placeholder of each custom IP box
    //
    function getPlaceholder (ip) {
        return ip === "" ? qsTr ("Auto") : ip
    }

    //
    // Changes the placeholder text of each custom IP box
    //
    function updatePlaceholders() {
        fmsAddress.placeholder = getPlaceholder (CppDS.defaultFMSAddress)
        radioAddress.placeholder = getPlaceholder (CppDS.defaultRadioAddress)
        robotAddress.placeholder = getPlaceholder (CppDS.defaultRobotAddress)
    }

    //
    // Default window position
    //
    x: (Screen.width - width) / 4
    y: (Screen.height - height) / 4

    //
    // Set window flags
    //
    flags: Qt.Window |
           Qt.WindowTitleHint |
           Qt.WindowSystemMenuHint |
           Qt.WindowCloseButtonHint |
           Qt.WindowMinimizeButtonHint

    //
    // Applies the UI settings...
    //
    function apply() {
        updatePlaceholders()
        CppBeeper.setEnabled (enableSoundEffects.checked)
        CppUtilities.setAutoScaleEnabled (autoScale.checked)
		
        CppDS.customFMSAddress = fmsAddress.text
        CppDS.customRadioAddress = radioAddress.text
        CppDS.customRobotAddress = robotAddress.text
    }

    //
    // Apply settings on launch
    //
    Component.onCompleted: {
        apply()
        CppBeeper.setEnabled (enableSoundEffects.checked)
    }

    //
    // Configure DS on init
    //
    Connections {
        target: CppDS
        function onProtocolChanged() {
            apply()
        }

        function onTeamNumberChanged() {
            updatePlaceholders()
        }
    }

    //
    // Save and load the settings
    //
    Settings {
        category: "SettingsWindow"
        property alias x: window.x
        property alias y: window.y
        property alias address: robotAddress.text
        property alias autoScale: autoScale.checked
        property alias enableSoundEffects: enableSoundEffects.checked
    }

    //
    // All the widgets of this window are placed in a column
    //
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Globals.spacing

        //
        // All the controls that indicate or change preferences
        //
        Panel {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ColumnLayout {
                anchors.fill: parent
                spacing: Globals.spacing
                anchors.margins: Globals.spacing * 1.5

                //
                // Network settings label
                //
                Label {
                    font.bold: true
                    text: qsTr ("Custom network addresses") + ":"
                }

                //
                // Network settings controls
                //
                RowLayout {
                    Layout.fillWidth: true
                    spacing: Globals.spacing

                    //
                    // Networking icon
                    //
                    Icon {
                        name: icons.fa_laptop
                        size: Globals.scale (48)
                        Layout.minimumWidth: 2 * size
                    }

                    //
                    // Network settings checkbox & line edit
                    //
                    GridLayout {
                        columns: 2
                        rowSpacing: Globals.spacing
                        columnSpacing: Globals.spacing

                        Label {
                            text: qsTr ("FMS") + ":"
                        }

                        LineEdit {
                            id: fmsAddress
                            Layout.fillWidth: true
                        }

                        Label {
                            text: qsTr ("Radio") + ":"
                        }

                        LineEdit {
                            id: radioAddress
                            Layout.fillWidth: true
                        }

                        Label {
                            text: qsTr ("Robot") + ":"
                        }

                        LineEdit {
                            id: robotAddress
                            Layout.fillWidth: true
                        }
                    }
                }

                Item {
                    height: Globals.spacing * 2
                }

                //
                // "Other Settings" label
                //
                Label {
                    font.bold: true
                    text: qsTr ("Other Settings") + ":"
                }

                //
                // "Other settings" controls
                //
                RowLayout {
                    Layout.fillWidth: true
                    spacing: Globals.spacing

                    //
                    // Gears icon
                    //
                    Icon {
                        name: icons.fa_sliders
                        size: Globals.scale (48)
                        Layout.minimumWidth: 2 * size
                    }

                    //
                    // Misc. settings
                    //
                    ColumnLayout {
                        spacing: Globals.spacing

                        Checkbox {
                            checked: true
                            id: enableSoundEffects
                            text: qsTr ("Enable UI sound effects")
                        }

                        Checkbox {
                            checked: true
                            id: autoScale
                            text: qsTr ("Auto-scale text and UI items")
                        }
                    }
                }  

                Item {
                    Layout.fillHeight: true
                }
            }
        }

        //
        // Dialog buttons
        //
        RowLayout {
            Layout.fillWidth: true

            Item {
                Layout.fillWidth: true
            }

            Button {
                text: qsTr ("OK")
                onClicked: {
                    apply()
                    close()
                }
            }

            Button {
                text: qsTr ("Apply")
                onClicked: apply()
            }
        }
    }
}
