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

import "widgets"
import "globals.js" as Globals

ColumnLayout {
    spacing: Globals.spacing

    //
    // Built with Qt icon
    //
    Image {
        smooth: true
        sourceSize.width: Globals.scale (64)
        sourceSize.height: Globals.scale (64)
        source: "qrc:/images/BuiltWithQt.svg"
        anchors.horizontalCenter: parent.horizontalCenter
    }

    //
    // A large label displaying the application name
    //
    Label {
        size: large
        text: appDspName
        font.bold: true
        anchors.horizontalCenter: parent.horizontalCenter
    }

    //
    // A medium label displaying the application version
    //
    Label {
        size: medium
        text: qsTr ("Version") + " " + appVersion
        anchors.horizontalCenter: parent.horizontalCenter
    }

    //
    // Website and report bug buttons
    //
    RowLayout {
        Layout.fillWidth: true
        spacing: Globals.scale (-1)
        anchors.horizontalCenter: parent.horizontalCenter

        Button {
            text: qsTr ("Visit Website")
            onClicked: Qt.openUrlExternally ("http://qdriverstation.sf.net/")
        }

        Button {
            text: qsTr ("Report Bug")
            onClicked: Qt.openUrlExternally ("http://github.com/wint-3794/qdriverstation/issues")
        }
    }
}
