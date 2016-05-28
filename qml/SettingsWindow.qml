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
import QtQuick.Window 2.0
import QtQuick.Layouts 1.0
import Qt.labs.settings 1.0

import "widgets"
import "globals.js" as Globals

Window {
    id: window
    visible: false
    title: qsTr ("Settings")
    minimumWidth: Globals.scale (420)
    minimumHeight: Globals.scale (460)
    color: Globals.Colors.WindowBackground

    //
    // Applies the UI settings...
    //
    function apply() {
        /* Set the placeholder */
        customAddress.placeholder = DriverStation.defaultRobotAddress()

        /* Set the PSC limit */
        if (pcsCheckbox.checked && pcs.value > 0)
            DriverStation.setParallelSocketCount (pcs.value)
        else {
            pcs.value = 0
            DriverStation.setParallelSocketCount (0)
        }

        /* Clear the custom robot address if needed */
        if (!customAddressCheck.checked)
            customAddress.text = ""

        /* Apply the robot address */
        DriverStation.setCustomRobotAddress (customAddress.text)
    }

    //
    // Check for updates (if we are allowed) & allow/disallow sounds
    //
    Component.onCompleted: {
        cBeeper.setEnabled (enableSoundEffects.checked)

        if (checkForUpdates.checked)
            cUpdater.checkForUpdates (cUpdaterUrl)
    }

    //
    // Configure DS on init
    //
    Connections {
        target: DriverStation
        onInitialized: apply()
        onProtocolChanged: apply()
        onTeamChanged: customAddress.placeholder = DriverStation.defaultRobotAddress()
    }

    //
    // Save and load the settings
    //
    Settings {
        category: "SettingsWindow"
        property alias x: window.x
        property alias y: window.y
        property alias pnss: pcs.value
        property alias address: customAddress.text
        property alias promptOnQuit: promptOnQuit.checked
        property alias checkForUpdates: checkForUpdates.checked
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
                    text: qsTr ("Network Settings") + ":"
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
                    ColumnLayout {
                        spacing: Globals.spacing

                        Checkbox {
                            id: customAddressCheck
                            checked: customAddress.text.length > 0
                            text: qsTr ("Use a custom robot address") + ":"
                        }

                        LineEdit {
                            id: customAddress
                            Layout.fillWidth: true
                            enabled: customAddressCheck.checked
                            placeholder: DriverStation.defaultRobotAddress()
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
                            id: checkForUpdates
                            text: qsTr ("Check for updates automatically")
                        }

                        Checkbox {
                            checked: true
                            id: enableSoundEffects
                            text: qsTr ("Enable UI sound effects")
                            onCheckedChanged: cBeeper.setEnabled (checked)
                        }

                        Checkbox {
                            checked: true
                            id: promptOnQuit
                            text: qsTr ("Prompt on quit")
                        }
                    }
                }

                Item {
                    height: Globals.spacing * 2
                }

                //
                // Advanced DS Options label
                //
                Label {
                    font.bold: true
                    text: qsTr ("Advanced DS Options") + ":"
                }

                //
                // Advanced DS Options controls
                //
                RowLayout {
                    Layout.fillWidth: true
                    spacing: Globals.spacing

                    //
                    // Terminal icon
                    //
                    Icon {
                        name: icons.fa_cogs
                        size: Globals.scale (48)
                        Layout.minimumWidth: 2 * size
                    }

                    //
                    // PSC Settings
                    //
                    ColumnLayout {
                        spacing: Globals.spacing

                        Checkbox {
                            id: pcsCheckbox
                            checked: pcs.value > 0
                            text: qsTr ("Set the PSC* to:")
                        }

                        Spinbox {
                            id: pcs
                            value: 0
                            minimumValue: 0
                            maximumValue: 1000
                            Layout.fillWidth: true
                            enabled: pcsCheckbox.checked
                        }

                        Label {
                            size: small
                            text: qsTr ("*PSC: Parallel Socket Count (0 = auto)")
                        }
                    }
                }

                Item {
                    Layout.fillHeight: true
                }
            }
        }

        //
        // The close button
        //
        RowLayout {
            Layout.fillWidth: true

            Item {
                Layout.fillWidth: true
            }

            Button {
                text: qsTr ("Close")
                onClicked: {
                    apply()
                    close()
                }
            }
        }
    }
}
