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
import QtQuick.Layouts 1.0

import "widgets"
import "globals.js" as Globals

RowLayout {
    spacing: Globals.spacing    

    //
    // Contains the actual tab controls/buttons
    //
    Panel {
        id: rightTab
        Layout.fillWidth: true
        Layout.fillHeight: true

        function hideWidgets() {
            about.opacity = 0
            charts.opacity = 0
            messages.opacity = 0
        }

        function showMessages() {
            hideWidgets()
            messages.opacity = 1
        }

        function showCharts() {
            hideWidgets()
            charts.opacity = 1
        }

        function showAbout() {
            hideWidgets()
            about.opacity = 1
        }

        Component.onCompleted: showMessages()

        Messages {
            opacity: 1
            id: messages
            visible: opacity > 0
            anchors.fill: parent
            anchors.margins: Globals.spacing
        }

        Charts {
            opacity: 0
            id: charts
            visible: opacity > 0
            anchors.fill: parent
            anchors.margins: Globals.spacing
        }

        About {
            id: about
            opacity: 0
            visible: opacity > 0
            anchors.fill: parent
            anchors.margins: Globals.spacing
        }
    }

    //
    // Contains the tab switcher buttons
    //
    ColumnLayout {
        Layout.fillHeight: true
        spacing: Globals.scale (-1)
        Layout.minimumWidth: Globals.scale (36)
        Layout.maximumWidth: Globals.scale (36)

        Button {
            icon: icons.fa_envelope
            caption.font.bold: true
            width: Globals.scale (36)
            height: Globals.scale (36)
            onClicked: rightTab.showMessages()
            textColor: messages.visible ? Globals.Colors.AlternativeHighlight :
                                          Globals.Colors.Foreground
        }

        Button {
            caption.font.bold: true
            icon: icons.fa_area_chart
            width: Globals.scale (36)
            height: Globals.scale (36)
            onClicked: rightTab.showCharts()
            textColor: charts.visible ? Globals.Colors.AlternativeHighlight :
                                         Globals.Colors.Foreground
        }

        Button {
            icon: icons.fa_info
            caption.font.bold: true
            width: Globals.scale (36)
            height: Globals.scale (36)
            onClicked: rightTab.showAbout()
            textColor: about.visible ? Globals.Colors.AlternativeHighlight :
                                       Globals.Colors.Foreground
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: Globals.spacing
        }

        Button {
            icon: icons.fa_cog
            caption.font.bold: true
            width: Globals.scale (36)
            height: Globals.scale (36)
            onClicked: window.showSettingsWindow()
        }

        Button {
            icon: icons.fa_keyboard_o
            caption.font.bold: true
            width: Globals.scale (36)
            height: Globals.scale (36)
            onClicked: window.showVirtualJoystickWindow()
        }
    }
}
