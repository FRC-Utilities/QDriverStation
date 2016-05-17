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

var Colors = {
    'Foreground'                : "#FFFFFF",
    'Background'                : "#4B4B4B",
    'WidgetBorder'              : "#282828",
    'WidgetBackground'          : "#4B4B4B",
    'WidgetForeground'          : "#DEDEDE",
    'WindowBackground'          : "#2A2A2A",
    'WidgetBackgroundSelected'  : "#3B3B3B",
    'WidgetForegroundSelected'  : "#DEDEDE",
    'WidgetBackgroundHighlight' : "#8B8B8B",
    'WidgetForegroundHighlight' : "#DEDEDE",
    'TextAreaBackground'        : "#D4D4D4",
    'TextAreaForeground'        : "#000000",
    'IndicatorGood'             : "#64FF00",
    'IndicatorError'            : "#FF1400",
    'IndicatorWarning'          : "#FF751A",
    'HighlightColor'            : "#52D000",
    'IconColor'                 : "#969696",
    'PanelBackground'           : "#464646",
    'CloseButton'               : "#5D1111",
    'AlternativeHighlight'      : "#01D3FF",
    'EnableButtonSelected'      : "#2EDC00",
    'DisableButtonSelected'     : "#FF0000",
    'EnableButtonUnselected'    : "#002F00",
    'DisableButtonUnselected'   : "#880000",
    'CPUProgress'               : "#DCAE00",
}

var ControlModes = {
    'Test'         : 0,
    'Autonomous'   : 1,
    'Teleoperated' : 2,
}

var slowAnimation = 200
var fastAnimation = 100
var spacing       = scale (10)
var invalidStr    = "--.--"
var uiFont        = "Ubuntu"
var monoFont      = "Ubuntu Mono"
var updatesURL    = "https://raw.githubusercontent.com/WinT-3794/QDriverStation/updater/current"

function scale (input) {
    return input * cRatio;
}

function fastBeep() {
    beep (220, 50)
}

function normalBeep() {
    beep (220, 100)
}

function longBeep() {
    beep (220, 200)
}

function beep (frequency, time) {
    cBeeper.beep (frequency, time)
}

function morse (input, frequency) {
    for (var i = 0; i < input.length; ++i) {
        var time = 0
        var base = 50
        var character = input.charAt (i)

        if (character === ".")
            time = base
        else if (character === "-")
            time = base * 3
        else if (character === " ")
            time = base * 3
        else if (character === "/")
            time = base * 7

        beep (frequency, time)
        beep (0, base)
    }
}

function updateControlMode (mode, enabled) {
    switch (mode) {
    case ControlModes.Test:
        DriverStation.startTest (enabled)
        break;
    case ControlModes.Autonomous:
        DriverStation.startAutonomous (enabled)
        break;
    case ControlModes.Teleoperated:
        DriverStation.startTeleoperated (enabled)
        break;
    default:
        ControlModes.disableRobot()
        break;
    }
}
