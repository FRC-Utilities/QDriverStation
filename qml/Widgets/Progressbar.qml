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
import "../Globals.js" as Globals

Rectangle {
    //
    // Holds the relative value of the progressbar
    //
    property int value: 0

    //
    // The values hold the 'range' of the progressbar
    //
    property int minimumValue: 0
    property int maximumValue: 100

    //
    // Default caption will display current relative value of progressbar
    //
    property string text: value + "%"

    //
    // Define the colors of the progressbar
    //
    property string barColor: Globals.Colors.HighlightColor
    property string textColor: Globals.Colors.TextAreaForeground

    //
    // Set height of widget and width of border
    //
    height: Globals.scale (14)
    border.width: Globals.scale (1)

    //
    // Set background color and border color
    //
    color: Globals.Colors.WindowBackground
    border.color: Globals.Colors.WidgetBorder

    //
    // Animate the progrssbar when it changes its value
    //
    Behavior on value {
        NumberAnimation {
            duration: Globals.slowAnimation
        }
    }

    //
    // The 'foreground' part of the progress bar that displays its value
    //
    Rectangle {
        y: 0
        x: 0
        color: barColor
        radius: parent.radius
        height: parent.height
        border.width: parent.border.width
        border.color: parent.border.color
        width: parent.width * ((minimumValue + value) / maximumValue)
    }

    //
    // The text/caption of the progressbar
    //
    Label {
        text: parent.text
        anchors.centerIn: parent
        opacity: font.pixelSize > Globals.scale (8)
        font.pixelSize: Math.min (Globals.scale (12), parent.height * (2/3))
    }
}
