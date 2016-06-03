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
    // Holds the text/caption/label of the control
    //
    property string text

    //
    // Misc. properties...
    //
    property bool enabled: false
    property bool checked: false
    property bool showBorder: false
    property bool leftToRight: false
    property string textColor: Globals.Colors.Foreground

    //
    // LED on/off colors
    //
    property string poweredColor: Globals.Colors.IndicatorGood
    property string unpoweredColor: Globals.Colors.IndicatorError

    //
    // Gives direct access to the caption control
    //
    property alias caption: label

    //
    // Gives access to the LED size
    //
    property int ledWidth: Globals.scale (28)
    property int ledHeight: Globals.scale (12)

    //
    // Control sizes
    //
    implicitHeight: Math.max (ledHeight, label.height)
    implicitWidth: ledWidth + label.implicitWidth + (2 * Globals.spacing)

    ///
    /// This is the actual LED...
    ///
    Rectangle {
        id: led
        implicitWidth: ledWidth
        implicitHeight: ledHeight
        border.color: Globals.Colors.WidgetBorder
        color: checked ? poweredColor : unpoweredColor
        border.width: Globals.scale (showBorder ? 1 : 0)

        Behavior on color {
            ColorAnimation {
                duration: Globals.slowAnimation
            }
        }

        anchors {
            margins: Globals.spacing
            verticalCenter: parent.verticalCenter
            left: leftToRight ? label.right : parent.left
        }
    }

    //
    // The text of the LED/checkbox
    //
    Label {
        id: label
        text: parent.text
        color: parent.textColor

        anchors {
            margins: Globals.spacing
            verticalCenter: parent.verticalCenter
            left: leftToRight ? parent.left : led.right
        }
    }

    //
    // Inverts the checked state of the widget when clicked
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
