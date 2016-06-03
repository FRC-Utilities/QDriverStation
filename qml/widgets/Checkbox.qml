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
import "../globals.js" as Globals

Item {
    //
    // Holds the text/caption of the control
    //
    property string text

    //
    // Misc. properties of the checkbox
    //
    property bool enabled: true
    property bool checked: false
    property bool leftToRight: false
    property alias backgroundColor: checkbox.color

    //
    // Gives direct access to the caption/label control
    //
    property alias caption: label

    //
    // Resize the control to fit the text length and height
    //
    height: Math.max (checkbox.height, label.height)
    width: checkbox.implicitWidth + label.implicitWidth + (2 * Globals.spacing)

    //
    // Holds the actual checkbox, because the rectangle is like a box...Ba Dum Tss!
    //
    Rectangle {
        id: checkbox
        border.width: Globals.scale (1)
        implicitWidth: Globals.scale (16)
        implicitHeight: Globals.scale (16)
        color: Globals.Colors.WindowBackground
        border.color: Globals.Colors.WidgetBorder

        anchors {
            verticalCenter: parent.verticalCenter
            left: leftToRight ? label.right : parent.left
            leftMargin: leftToRight ? Globals.spacing : 0
        }

        Icon {
            name: icons.fa_check
            anchors.centerIn: parent
            color: checked ? Globals.Colors.AlternativeHighlight : parent.color
        }
    }

    ///
    /// The text of the control
    ///
    Label {
        id: label
        text: parent.text

        anchors {
            margins: Globals.spacing * 0.75
            verticalCenter: parent.verticalCenter
            left: leftToRight ? parent.left : checkbox.right
        }
    }

    //
    // Inverts the check-state of the widget when clicked
    //
    MouseArea {
        anchors.fill: parent
        enabled: parent.enabled
        onClicked: {
            Globals.normalBeep()
            parent.checked = !parent.checked
        }
    }
}
