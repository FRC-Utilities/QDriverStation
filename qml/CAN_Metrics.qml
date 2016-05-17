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
import QtQuick 2.0
import QtQuick.Layouts 1.0

import "widgets"
import "globals.js" as Globals

Item {
    Connections {
        target: DriverStation
    }

    Column {
        anchors.centerIn: parent
        spacing: Globals.spacing

        Label {
            size: large
            font.bold: true
            Layout.fillWidth: true
            text: qsTr ("CAN Metrics")
            anchors.horizontalCenter: parent.horizontalCenter
        }

        GridLayout {
            columns: 2
            rowSpacing: Globals.spacing
            columnSpacing: Globals.spacing
            anchors.horizontalCenter: parent.horizontalCenter

            Label {
                Layout.fillWidth: true
                text: qsTr ("Bus Utilization") + ":"
                horizontalAlignment: Text.AlignRight
            }

            Label {
                id: utilization
                Layout.fillWidth: true
                text: Globals.invalidStr
            }

            Label {
                Layout.fillWidth: true
                text: qsTr ("Bus Off") + ":"
                horizontalAlignment: Text.AlignRight
            }

            Label {
                id: busOff
                Layout.fillWidth: true
                text: Globals.invalidStr
            }

            Label {
                Layout.fillWidth: true
                text: qsTr ("TX Full") + ":"
                horizontalAlignment: Text.AlignRight
            }

            Label {
                id: txFull
                Layout.fillWidth: true
                text: Globals.invalidStr
            }

            Label {
                Layout.fillWidth: true
                text: qsTr ("Receive Errors") + ":"
                horizontalAlignment: Text.AlignRight
            }

            Label {
                id: receiveErrors
                Layout.fillWidth: true
                text: Globals.invalidStr
            }

            Label {
                Layout.fillWidth: true
                text: qsTr ("Transmit Errors") + ":"
                horizontalAlignment: Text.AlignRight
            }

            Label {
                id: transmitErrors
                Layout.fillWidth: true
                text: Globals.invalidStr
            }
        }
    }
}
