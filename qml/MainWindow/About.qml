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
import QtQuick.Layouts 1.0

import "../Widgets"
import "../Globals.js" as Globals

ColumnLayout {
    spacing: Globals.spacing

    //
    // The icon of the button
    //
    Icon {
        color: "#fff"
        name: icons.fa_cogs
        size: Globals.scale (64)
        Layout.alignment: Qt.AlignHCenter
    }

    //
    // A large label displaying the application name and version
    //
    Label {
        size: large
        font.bold: true
        Layout.alignment: Qt.AlignHCenter
        text: CppAppDspName + " " + CppAppVersion
    }

    //
    // A medium label displaying the LibDS version
    //
    Label {
        size: medium
        Layout.alignment: Qt.AlignHCenter
        text: "LibDS "
              + CppDS.libDSVersion
              + " (built on "
              + CppDS.buildDate
              + " at "
              + CppDS.buildTime
              + ")"
    }

    //
    // Website and report bug buttons
    //
    RowLayout {
        Layout.fillWidth: true
        spacing: Globals.scale (-1)
        Layout.alignment: Qt.AlignHCenter

        Button {
            text: qsTr ("Visit Website")
            onClicked: Qt.openUrlExternally (CppAppWebsite)
        }

        Button {
            text: qsTr ("Report Bug")
            onClicked: Qt.openUrlExternally (CppAppRepBugs)
        }
    }
}
