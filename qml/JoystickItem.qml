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

Rectangle {
    //
    // The joystick that this item represents
    //
    property int jsIndex: 0

    //
    // The joystick that is currently selected by the user
    //
    property int currentJS: 0

    //
    // Holds the title/name of the joystick
    //
    property string jsName

    //
    // Emitted when the item is clicked, this is used by the
    // Joystick tab to redraw the joystick indicators to the
    // selected joystick
    //
    signal selected

    //
    // Emitted when the user enables or disables this joystick.
    // This is used by the Joysticks tab to update the blacklist
    // status of the selected joystick and redraw the controls
    //
    signal blacklistedChanged

    //
    // The size of the control
    //
    height: text.implicitHeight * 1.5
    anchors {
        left: parent.left
        right: parent.right
    }

    //
    // If the control is selected, then its background will be
    // a dark grey, if the control is hovered by the mouse, then
    // it will be light grey.
    //
    // If the control is not selected and not hovered, it will have
    // no specific background color
    //
    color: {
        if (mouse.containsMouse)
            return Qt.lighter (Globals.Colors.WidgetBackground, 1.2)

        else if (currentJS === jsIndex)
            return Globals.Colors.WidgetBackground

        return "transparent"
    }

    //
    // Displays the joystick name
    //
    Label {
        id: text
        maximumLineCount: 1
        text: "  " + jsName
        elide: Text.ElideRight

        anchors {
            right: icon.left
            left: parent.left
            verticalCenter: parent.verticalCenter
        }
    }

    //
    // This is the principal mouse area, if it is clicked, then
    // the selected joystick will change to this joystick
    //
    MouseArea {
        id: mouse
        hoverEnabled: true

        anchors {
            top: parent.top
            right: icon.left
            left: parent.left
            bottom: parent.bottom
        }

        onClicked: {
            selected()
            Globals.normalBeep()
        }
    }

    //
    // The blacklist icon/button. When clicked, the joystick
    // blacklist status will be toggled
    //
    Rectangle {
        id: icon
        width: Globals.scale (32)

        anchors {
            top: parent.top
            right: parent.right
            bottom: parent.bottom
        }

        color: {
            if (blacklistMouse.containsMouse
                    || (mouse.containsMouse && currentJS !== jsIndex))
                return Qt.lighter (Globals.Colors.WidgetBackground, 1.2)

            else if (currentJS === jsIndex)
                return Globals.Colors.WidgetBackground

            return "transparent"
        }

        //
        // The power icon.
        // When the joystick is disabled, it will be red (off).
        // When the joystick is enabled, it will be green (on).
        //
        Icon {
            name: icons.fa_power_off
            anchors.centerIn: parent
            color: QJoysticks.isBlacklisted (jsIndex) ?
                       Globals.Colors.IndicatorError :
                       Globals.Colors.HighlightColor
        }

        //
        // Toggles the blacklist state of the joystick
        //
        MouseArea {
            id: blacklistMouse
            hoverEnabled: true
            anchors.fill: parent
            onClicked: {
                Globals.normalBeep()
                blacklistedChanged()
            }
        }

        //
        // Allows the button to gently change its background color
        //
        Behavior on color {
            ColorAnimation {
                duration: Globals.slowAnimation
            }
        }
    }

    //
    // Allows the control to gently change its background color
    //
    Behavior on color {
        ColorAnimation {
            duration: Globals.slowAnimation
        }
    }
}
