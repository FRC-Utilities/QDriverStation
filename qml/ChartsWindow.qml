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
    id: charts

    x: Globals.scale (300)
    y: Globals.scale (200)
    title: qsTr ("Robot Charts")
    color: Globals.Colors.Background

    minimumWidth: Globals.scale (720)
    minimumHeight: Globals.scale (360)

    flags: Qt.Window |
           Qt.WindowTitleHint |
           Qt.WindowSystemMenuHint |
           Qt.WindowCloseButtonHint |
           Qt.WindowMinimizeButtonHint

    Component.onCompleted: {
        if (showOnLaunch.checked)
            show()
    }

    Settings {
        category: "Charts Window"
        property alias x: charts.x
        property alias y: charts.y
        property alias width: charts.width
        property alias height: charts.height
        property alias showOnInt: showOnLaunch.checked
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: Globals.spacing
        anchors.margins: Globals.spacing

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: Globals.spacing

            ColumnLayout {
                spacing: Globals.spacing

                Label {
                    text: qsTr ("Voltage") + ":"
                }

                VoltageGraph {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
            }

            ColumnLayout {
                spacing: Globals.spacing

                Label {
                    text: qsTr ("CPU Usage") + ":"
                }

                CpuUsageGraph {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: Globals.spacing

            Checkbox {
                checked: false
                id: showOnLaunch
                text: qsTr ("Show this window on application launch")
            }

            Item {
                Layout.fillWidth: true
            }

            Button {
                text: qsTr ("Close")
                onClicked: charts.close()
            }
        }
    }
}
