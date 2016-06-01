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

Item {
    function updateGraphTimes (seconds) {
        cpu.setSpeed (seconds)
        voltage.setSpeed (seconds)
    }

    Component.onCompleted: oneMin.checked = true

    ColumnLayout {
        anchors.fill: parent
        spacing: Globals.spacing

        RowLayout {
            Layout.fillHeight: false
            spacing: Globals.scale (-1)

            Label {
                text: qsTr ("CPU Usage") + ":"
            }

            Item {
                Layout.fillWidth: true
            }

            Button {
                id: twelveSecs
                checkable: true
                text: qsTr ("12 s")
                width: Globals.scale (48)

                onCheckedChanged: {
                    if (checked) {
                        oneMin.checked = false
                        fiveMin.checked = false
                        twelveSecs.checked = true

                        updateGraphTimes (12)
                    }

                    if (!oneMin.checked && !fiveMin.checked && !twelveSecs.checked)
                        checked = true
                }
            }

            Button {
                id: oneMin
                checkable: true
                text: qsTr ("1 m")
                width: Globals.scale (48)

                onCheckedChanged: {
                    if (checked) {
                        oneMin.checked = true
                        fiveMin.checked = false
                        twelveSecs.checked = false

                        updateGraphTimes (60)
                    }

                    if (!oneMin.checked && !fiveMin.checked && !twelveSecs.checked)
                        checked = true
                }
            }

            Button {
                id: fiveMin
                checkable: true
                text: qsTr ("5 m")
                width: Globals.scale (48)

                onCheckedChanged: {
                    if (checked) {
                        oneMin.checked = false
                        fiveMin.checked = true
                        twelveSecs.checked = false

                        updateGraphTimes (300)
                    }

                    if (!oneMin.checked && !fiveMin.checked && !twelveSecs.checked)
                        checked = true
                }
            }
        }

        CpuUsageGraph {
            id: cpu
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Label {
            text: qsTr ("Robot Voltage") + ":"
        }

        VoltageGraph {
            id: voltage
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}
