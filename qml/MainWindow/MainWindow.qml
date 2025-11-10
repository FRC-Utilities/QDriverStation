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
import QtQuick.Window 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.15
import Qt.labs.settings 1.0

import "../Globals.js" as Globals

Window {
    id: mw

    //
    // Gets the \c x value used in the initial launch
    //
    function getDefaultX() {
        return (Screen.desktopAvailableWidth - getMinimumWidth()) / 2
    }

    //
    // Gets the \c y value used in the initial launch
    //
    function getDefaultY() {
        return Screen.desktopAvailableHeight - (getMinimumHeight() * 1.2)
    }

    //
    // If set to true, the window will dock to the bottom of the screen
    //
    property bool docked: false

    //
    // Holds the old values of x and y
    //
    property int oldX: getDefaultX()
    property int oldY: getDefaultY()

    //
    // Calculates the minimum width of the window
    //
    function getMinimumWidth() {
        return layout.implicitWidth + (2 * layout.x)
    }

    //
    // Calculates the minimum height of the window
    //
    function getMinimumHeight() {
        return Math.max(205, layout.implicitHeight + (2 * layout.y))
    }

    //
    // The actual docking procedures
    //
    function updateWindowMode() {
        if (!visible)
            return

        if (docked) {
            if (CppIsUnix) {
                x = 0
                width = Screen.desktopAvailableWidth
                minimumWidth = Screen.desktopAvailableWidth
            }

            else {
                showMaximized()
            }

            y = Screen.desktopAvailableHeight - height
        }

        else {
            showNormal()
            width = getMinimumWidth()
            minimumWidth = getMinimumWidth()

            x = oldX
            y = oldY
        }
    }

    //
    // Initial position
    //
    x: getDefaultX()
    y: getDefaultY()

    //
    // Update custom variables
    //
    onDockedChanged: updateWindowMode()
    onXChanged: oldX = !docked ? x : oldX
    onYChanged: oldY = !docked ? y : oldY

    //
    // Window geometry hacks
    //
    height: getMinimumHeight()
    minimumWidth: getMinimumWidth()
    minimumHeight: getMinimumHeight()
    maximumHeight: getMinimumHeight()
    onHeightChanged: height = getMinimumHeight()

    //
    // To hell with you X11
    //
    flags: Qt.Dialog | Qt.WindowCloseButtonHint | Qt.WindowTitleHint

    //
    // Misc. properties
    //
    visible: false
    color: Globals.Colors.WindowBackground
    title: CppAppDspName + " - " + qsTr ("Version") + " " + CppAppVersion

    //
    // Save window geometry
    //
    Settings {
        category: "MainWindow"
        property alias x: mw.x
        property alias y: mw.y
        property alias oldX: mw.oldX
        property alias oldY: mw.oldY
        property alias docked: mw.docked
    }

    //
    // Animate window position when dock mode changes
    //
    Behavior on x { NumberAnimation { duration: Globals.fastAnimation }}
    Behavior on y { NumberAnimation { duration: Globals.slowAnimation }}

    //
    // Display the left tab, status controls and right tab horizontally
    //
    RowLayout {
        id: layout
        x: Globals.spacing
        y: Globals.spacing
        width: Math.max (implicitWidth, mw.width - (2 * x))
        height: Math.max (implicitHeight, mw.height - (2 * y))

        LeftTab {
            Layout.fillWidth: false
            Layout.fillHeight: true
            Layout.minimumWidth: Globals.scale (440)
            onWindowModeChanged: mw.docked = isDocked
            onFlashStatusIndicators: status.flashStatusIndicators()
        }

        Status {
            id: status
            Layout.fillWidth: false
            Layout.fillHeight: true
        }

        RightTab {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumWidth: Globals.scale (420)
        }
    }
}
