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

import "../Widgets"
import "../Globals.js" as Globals

Plot {
    //
    // The graph will be black when there are no communications with the robot
    //
    property string noCommsColor: "#000"

    //
    // Gets current voltage and changes plot settings accordingly
    //
    function update() {
        value = CppDS.voltage

        if (!CppDS.connectedToRobot) {
            barColor = noCommsColor
            value = to * 0.95
        }

        else if (getLevel() > 0.80)
            barColor = Globals.Colors.IndicatorGood
        else if (getLevel() > 0.70)
            barColor = Globals.Colors.IndicatorWarning
        else
            barColor = Globals.Colors.IndicatorError
    }

    //
    // Refresh the graph values from time to time
    //
    onRefreshed: update()
    barColor: Globals.Colors.TextAreaBackground

    //
    // Start graphing from origin, not from the middle or some other place
    //
    from: 0
    Component.onCompleted: update()
    to: CppDS.maximumBatteryVoltage
}
