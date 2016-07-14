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

import "../Widgets"
import "../Globals.js" as Globals

Item {
    //
    // Changes the horizontal refresh rate of the charts
    //
    function updateGraphTimes (seconds) {
        loss.clear()
        voltage.clear()
        loss.setSpeed (seconds)
        voltage.setSpeed (seconds)
    }

    //
    // Make sure that the "1 min" button is checked on start-up
    //
    Component.onCompleted: oneMin.checked = true

    //
    // Displays all the widgets in a vertical layout
    //
    ColumnLayout {
        anchors.fill: parent
        spacing: Globals.spacing

        //
        // Show the refresh speed buttons horizontally
        //
        RowLayout {
            Layout.fillHeight: false
            spacing: Globals.scale (-1)

            //
            // Packet loss label
            //
            Label {
                text: qsTr ("Packet Loss %") + ":"
            }

            //
            // Horizontal spacer
            //
            Item {
                Layout.fillWidth: true
            }

            //
            // "12 s" button
            //
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

            //
            // "1 m" button
            //
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

            //
            // "5 m" button
            //
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

        //
        // Packet loss chart
        //
        Plot {
            id: loss
            value: 0
            minimumValue: 0
            maximumValue: 100
            Layout.fillWidth: true
            Layout.fillHeight: true
            barColor: Globals.Colors.PacketLoss
            onRefreshed: value = Math.max (1, DriverStation.packetLoss())
            Component.onCompleted: value = Math.max (1, DriverStation.packetLoss())
        }

        //
        // Robot voltage label
        //
        Label {
            text: qsTr ("Robot Voltage") + ":"
        }

        //
        // Robot voltage chart
        //
        VoltageGraph {
            id: voltage
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}
