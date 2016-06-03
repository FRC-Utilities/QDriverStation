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
    id: dialog
    title: appDspName
    color: Globals.Colors.WindowBackground

    //
    // Set window geometry
    //
    minimumWidth: Globals.scale (320)
    maximumWidth: Globals.scale (302)
    minimumHeight: Globals.scale (102)
    maximumHeight: Globals.scale (102)

    //
    // Set window flags
    //
    flags: Qt.Window |
           Qt.WindowTitleHint |
           Qt.WindowStaysOnTopHint |
           Qt.WindowSystemMenuHint |
           Qt.WindowCloseButtonHint

    //
    // If this value is set to true, the window will show on application
    // start. This will happen only during the first launch
    //
    property bool firstLaunch: true

    Settings {
        property alias firstLaunch: dialog.firstLaunch
    }

    //
    // Show the dialog if it is the first launch
    //
    Component.onCompleted: {
        if (firstLaunch)
            show()
    }

    //
    // Allign the widgets vertically
    //
    ColumnLayout {
        anchors.fill: parent
        spacing: Globals.spacing
        anchors.margins: Globals.spacing

        Label {
            text: qsTr ("Please input your team number") + ":"
        }

        Spinbox {
            id: teamNumber
            minimumValue: 0
            maximumValue: 9999
            Layout.fillWidth: true
            Layout.fillHeight: false
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Item {
                Layout.fillWidth: true
            }

            Button {
                text: qsTr ("OK")
                anchors.verticalCenter: parent.verticalCenter
                onClicked: {
                    close()
                    firstLaunch = false
                    DriverStation.setTeam (teamNumber.value)
                }
            }
        }
    }
}
