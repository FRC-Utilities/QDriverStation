/*
 * Copyright (c) 2015 WinT 3794 <http://wint3794.org>
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
import "qrc:/falcon/qml" as Falcon

Falcon.FalconApp {
    width: 320
    height: 480
    title: qsTr ("QDriverStation")
    Component.onCompleted: q_app.setToolbarText (title)

    Column {
        anchors.centerIn: parent
        spacing: q_app.scale (8)

        Falcon.Icon {
            name: "code"
            iconSize: q_app.scale (96)
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Falcon.Label {
            centered: true
            font.bold: true
            fontSize: "large"
            text: qsTr ("Cooking in Progress...")
        }

        Falcon.Label {
            centered: true
            text: qsTr ("Check back later or contribute!")
        }

        Item {
            width: q_app.scale (48)
            height: q_app.scale (48)
        }

        Falcon.Button {
            icon: "heart"
            background: "#f00"
            foreground: "#fff"
            text: qsTr ("Contribute!")
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: Qt.openUrlExternally ("https://github.com/WinT-3794/QDriverStation")
        }
    }

    Falcon.BottomBar {
        id: bottomBar
        height: q_app.scale (48)

        Row {
            anchors.centerIn: parent
            spacing: parent.width / 6

            Falcon.IconButton {
                name: "home"
                text: qsTr ("Home")
                anchors.verticalCenter: parent.verticalCenter
            }

            Falcon.IconButton {
                name: "gamepad"
                text: qsTr ("Control")
                anchors.verticalCenter: parent.verticalCenter
            }

            Falcon.IconButton {
                name: "terminal"
                text: qsTr ("Console")
                anchors.verticalCenter: parent.verticalCenter
            }

            Falcon.IconButton {
                name: "cog"
                text: qsTr ("Settings")
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }
}
