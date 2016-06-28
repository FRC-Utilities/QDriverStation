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
import "../globals.js" as Globals

Rectangle {
    //
    // Holds the text displayed by the button
    //
    property string text

    //
    // Holds the icon displayed by the button
    //
    property string icon

    //
    // Misc. properties
    //
    property bool enabled: true
    property bool checked: false
    property bool checkable: false

    //
    // Gives direct access to the text control
    //
    property alias caption: label

    //
    // Holds the list of the icon names
    //
    property alias icons: awesome.icons

    //
    // These properties control the size of the text and the icon
    //
    property alias textSize: label.size
    property alias iconSize: awesome.size

    //
    // These properties control the colors of the text and the icon
    //
    property string textColor: Globals.Colors.Foreground
    property string baseColor: Globals.Colors.WidgetBackground

    //
    // Emitted when the button is clicked, you can control what to
    // do when the button is clicked with the onClicked: property
    //
    signal clicked

    //
    // Change the background color when the button is pressed or checked
    //
    color: {
        if (checked || mouse.pressed)
            return Qt.darker (baseColor, 1.7)

        return baseColor
    }

    //
    // Border size and color
    //
    border.width: Globals.scale (1)
    border.color: Globals.Colors.WidgetBorder

    //
    // You may need to re-define this values when dealing with layouts
    //
    height: Globals.scale (24)
    width: Math.max (label.implicitWidth + Globals.scale (12), Globals.scale (120))

    //
    // Perform a simple animation when the background color is changed
    //
    Behavior on color {
        ColorAnimation {
            duration: Globals.slowAnimation
        }
    }

    //
    // The actual label/caption/text of the button
    //
    Label {
        id: label
        color: textColor
        text: parent.text
        anchors.fill: parent
        opacity: parent.enabled ? 1 : 0.5
        anchors.margins: Globals.scale (1)
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }

    //
    // The icon of the button
    //
    Icon {
        id: awesome
        size: large
        color: textColor
        name: parent.icon
        anchors.fill: parent
        anchors.margins: Globals.scale (1)
    }

    //
    // Allows the Button to know if the mouse is over the control
    // or pressing the control...
    //
    MouseArea {
        id: mouse
        hoverEnabled: true
        anchors.fill: parent
        enabled: parent.enabled

        onClicked: {
            Globals.normalBeep()
            parent.clicked()

            if (parent.checkable)
                parent.checked = !parent.checked
        }
    }
}
