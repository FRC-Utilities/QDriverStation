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
import QtQuick.Window 2.0
import QtQuick.Layouts 1.0
import Qt.labs.settings 1.0

import "widgets"
import "globals.js" as Globals

Item {
    property double currentPos: 0
    property int refreshInterval: 50
    property double rectWidth: Globals.scale (2)

    Timer {
        repeat: true
        interval: refreshInterval
        Component.onCompleted: start()

        onTriggered: {
            currentPos += 1
            plot.requestPaint()
        }
    }

    Plot {
        id: plot
        anchors.fill: parent

        onPaint: {
            var context = getContext('2d')
            var voltage = DriverStation.currentBatteryVoltage() /
                          DriverStation.nominalBatteryVoltage()

            voltage = Math.min (voltage, 0.95) // Do not exceed 95%
            voltage = Math.max (voltage, 0.20) // Used during no robot comm.

            /* Set stroke color */
            if (voltage >= 0.8)
                context.fillStyle = Globals.Colors.IndicatorGood
            else if (voltage >= 0.7)
                context.fillStyle = Globals.Colors.IndicatorWarning
            else if (!DriverStation.isConnectedToRobot())
                context.fillStyle = Globals.Colors.CPUProgress
            else
                context.fillStyle = Globals.Colors.IndicatorError

            /* Calculate X and Y coordinates */
            var xOffset = Globals.scale (currentPos * rectWidth)
            var yOffset = (1 - voltage) * height

            /* Reset the graph if it is greater than the width */
            if (xOffset > plot.width) {
                xOffset = 0
                currentPos = 0
                context.clearRect (0, 0, plot.width, plot.height)
            }

            /* Draw a rectangle with its highest part representing the voltage */
            context.fillRect (xOffset, yOffset, rectWidth, height)
        }
    }
}
