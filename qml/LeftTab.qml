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
    id: tab
    spacing: Globals.spacing

    signal flashStatusIndicators
    signal windowModeChanged (var isDocked)

    Component.onCompleted: Globals.logInit ("Left Section")

    //
    // Contains the tab switcher buttons
    //
    Column {
        Layout.fillWidth: false
        Layout.fillHeight: true
        spacing: Globals.scale (-1)

        Button {
            icon: icons.fa_dashboard
            caption.font.bold: true
            width: Globals.scale (36)
            height: Globals.scale (36)
            onClicked: leftTab.showOperator()
            textColor: operator.visible ? Globals.Colors.AlternativeHighlight :
                                          Globals.Colors.Foreground
        }

        Button {
            icon: icons.fa_stethoscope
            caption.font.bold: true
            width: Globals.scale (36)
            height: Globals.scale (36)
            onClicked: leftTab.showDiagnostics()
            textColor: diagnostics.visible ? Globals.Colors.AlternativeHighlight :
                                             Globals.Colors.Foreground
        }

        Button {
            icon: icons.fa_wrench
            caption.font.bold: true
            width: Globals.scale (36)
            height: Globals.scale (36)
            onClicked: leftTab.showPreferences()
            textColor: preferences.visible ? Globals.Colors.AlternativeHighlight :
                                             Globals.Colors.Foreground
        }

        Button {
            icon: icons.fa_usb
            caption.font.bold: true
            width: Globals.scale (36)
            height: Globals.scale (36)
            onClicked: leftTab.showJoysticks()
            textColor: joysticks.visible ? Globals.Colors.AlternativeHighlight :
                                           Globals.Colors.Foreground
        }
    }

    //
    // Contains the actual controls
    //
    Panel {
        id: leftTab
        Layout.fillWidth: true
        Layout.fillHeight: true

        function updateWidth() {
            if (joysticks.opacity > 0 && Layout.minimumWidth > 1)
                Layout.minimumWidth = joysticks.getMinimumWidth() * 1.2
            else
                Layout.minimumWidth = 0
        }

        function hideWidgets() {
            operator.opacity = 0
            joysticks.opacity = 0
            preferences.opacity = 0
            diagnostics.opacity = 0
        }

        function showOperator() {
            hideWidgets()
            updateWidth()
            operator.opacity = 1
        }

        function showJoysticks() {
            hideWidgets()
            updateWidth()
            joysticks.opacity = 1
        }

        function showPreferences() {
            hideWidgets()
            updateWidth()
            preferences.opacity = 1
        }

        function showDiagnostics() {
            hideWidgets()
            updateWidth()
            diagnostics.opacity = 1
        }

        Connections {
            target: DriverStation
            onJoystickCountChanged: leftTab.updateWidth()
        }

        Component.onCompleted: showOperator()

        Operator {
            opacity: 1
            id: operator
            visible: opacity > 0
            anchors.fill: parent
            anchors.margins: Globals.spacing
            onFlashStatusIndicators: tab.flashStatusIndicators()
            onWindowModeChanged: tab.windowModeChanged (isDocked)
        }

        Diagnostics {
            opacity: 0
            id: diagnostics
            visible: opacity > 0
            anchors.fill: parent
            anchors.margins: Globals.spacing
        }

        Preferences {
            opacity: 0
            id: preferences
            visible: opacity > 0
            anchors.fill: parent
            anchors.margins: Globals.spacing
        }

        Joysticks {
            opacity: 0
            id: joysticks
            visible: opacity > 0
            anchors.fill: parent
            anchors.margins: Globals.spacing
        }
    }
}
