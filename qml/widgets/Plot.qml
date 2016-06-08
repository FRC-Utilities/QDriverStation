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

Rectangle {
    id: plot

    //
    // Defines the refresh interval (in milliseconds) of the graph
    //
    property int refreshInterval: 50

    //
    // Defines the width of each bar generated during each refresh
    //
    property double rectWidth: Globals.scale (2)

    //
    // Gives direct access to the canvas object
    //
    property alias canvasObject: canvas

    //
    // Defines the color to use to draw the lines
    //
    property string barColor: Globals.Colors.HighlightColor

    //
    // Display options
    //
    property int value: 0
    property int minimumValue: 0
    property int maximumValue: 100

    //
    // Emitted when the timer expires and a canvas repaint is done
    //
    signal refreshed

    //
    // Calculates the ratio between the current value and the maxinum value
    //
    function getLevel() {
        return Math.max (value / maximumValue, minimumValue / maximumValue)
    }

    //
    // Changes the time in which the graph resets itself
    //
    function setSpeed (seconds) {
        /* Get current time required to reset the graph */
        var pixelsPerSec = rectWidth * (1000 / refreshInterval)
        var resetTime = width / pixelsPerSec

        /* Do a rule of three to obtain new refresh interval */
        var newInterval = (seconds * refreshInterval) / resetTime

        /* Apply obtained interval */
        refreshInterval = newInterval
        timer.interval = refreshInterval
    }

    //
    // Forces the canvas to clear its plot
    //
    function clear() {
        timer.currentPos = canvas.width / rectWidth
    }

    //
    // Rectangle color & border
    //
    border.width: Globals.scale (1)
    color: Globals.Colors.WindowBackground
    border.color: Globals.Colors.WidgetBorder

    //
    // Refreshes the graph on real-time
    //
    Timer {
        id: timer
        interval: refreshInterval
        Component.onCompleted: start()

        property int currentPos: 0

        onTriggered: {
            if (plot.visible)
                ++currentPos

            canvas.requestPaint()
            parent.refreshed()
        }
    }

    //
    // The actual canvas used to draw the graph
    //
    Canvas {
        id: canvas
        anchors.fill: parent
        renderStrategy: Canvas.Threaded
        anchors.margins: parent.border.width

        onPaint: {
            /* Lazy is good sometimes */
            if (plot.visible) {
                /* Get drawing context */
                var context = getContext('2d')

                /* Set the bar color */
                context.fillStyle = barColor

                /* Calculate X and Y coordinates */
                var yOffset = (1 - getLevel()) * height
                var xOffset = timer.currentPos * rectWidth

                /* Reset the graph if it is greater than the width */
                if (xOffset > canvas.width) {
                    xOffset = 0
                    timer.currentPos = 0
                    context.clearRect (0, 0, canvas.width, canvas.height)
                }

                /* Draw a single bar */
                context.fillRect (xOffset, yOffset, rectWidth, height)
            }

            /* Restart the timer */
            timer.restart()
        }
    }
}
