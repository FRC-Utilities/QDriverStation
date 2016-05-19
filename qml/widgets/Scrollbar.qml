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
    id: container
    opacity: 0

    property variant mouseArea
    property variant scrollArea
    property variant orientation: Qt.Vertical

    //
    // Shows the control and enables the 'watchdog' timer
    //
    function showControl() {
        if (scroll.height < container.height) {
            opacity = 1
            timer.start()
        }
    }

    //
    // Used to calculate the position that the scrollbar
    // should have to stay in reference with the height/width of
    // the parent flickable
    //
    function calculatePosition()
    {
        var ny = 0;

        if (container.orientation == Qt.Vertical)
            ny = scrollArea.visibleArea.yPosition * container.height;

        else
            ny = scrollArea.visibleArea.xPosition * container.width;

        if (ny > Globals.scale (2))
            return ny;

        else return Globals.scale (2);
    }

    //
    // Used to calculate the size that the scrollbar should have
    //
    function calculateSize()
    {
        var nh, ny;

        if (container.orientation == Qt.Vertical)
            nh = scrollArea.visibleArea.heightRatio * container.height;
        else
            nh = scrollArea.visibleArea.widthRatio * container.width;

        if (container.orientation == Qt.Vertical)
            ny = scrollArea.visibleArea.yPosition * container.height;
        else
            ny = scrollArea.visibleArea.xPosition * container.width;

        if (ny > Globals.scale (3)) {
            var t;

            if (container.orientation == Qt.Vertical)
                t = Math.ceil (container.height - ny);

            else
                t = Math.ceil (container.width - ny);

            if (nh > t)
                return t

            else
                return nh
        }

        else return nh + ny;
    }

    //
    // Enables a smooth fade when the control is hovered
    // or changes its size
    //
    Behavior on opacity {
        NumberAnimation {
            duration: Globals.slowAnimation
        }
    }

    //
    // Hides the control after certain amount of time without
    // any activity
    //
    Timer {
        id: timer
        interval: 800
        onTriggered: {
            if (opacity > 0 && !mouseArea.containsMouse)
                opacity = 0
        }
    }

    //
    // The actual iOS-like scrollbar
    //
    Rectangle {
        id: scroll
        opacity: 0.5
        radius: Globals.scale (5)
        onHeightChanged: showControl()
        color: Globals.Colors.TextAreaBackground
        width: container.orientation == Qt.Vertical ? container.width : calculateSize()
        height: container.orientation == Qt.Vertical ? calculateSize() : container.height
        x: container.orientation == Qt.Vertical ? Globals.scale (2) : calculatePosition()
        y: container.orientation == Qt.Vertical ? calculatePosition() : Globals.scale (2)
    }
}
