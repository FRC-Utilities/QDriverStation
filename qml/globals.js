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

//
// All the colors used by the application are defined here
//
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
    'IndicatorWarning'          : "#FFAA22",
    'HighlightColor'            : "#52D000",
    'IconColor'                 : "#969696",
    'PanelBackground'           : "#464646",
    'AlternativeHighlight'      : "#01D3FF",
    'EnableButtonSelected'      : "#2EDC00",
    'DisableButtonSelected'     : "#FF0000",
    'EnableButtonUnselected'    : "#002F00",
    'DisableButtonUnselected'   : "#880000",
    'CPUProgress'               : "#DCAE00",
}

//
// Rename to "Colors" and comment the section above to enable it
//
var ExperimentalTheme = {
    'Foreground'                : "#D3DAE3",
    'Background'                : "#3E424D",
    'WidgetBorder'              : "#2b2e39",
    'WidgetBackground'          : "#3E424D",
    'WidgetForeground'          : "#D3DAE3",
    'WindowBackground'          : "#2F343F",
    'WidgetBackgroundSelected'  : "#3E424D",
    'WidgetForegroundSelected'  : "#D3DAE3",
    'WidgetBackgroundHighlight' : "#505666",
    'WidgetForegroundHighlight' : "#D3DAE3",
    'TextAreaBackground'        : "#D3DAE3",
    'TextAreaForeground'        : "#282828",
    'IndicatorGood'             : "#5DBA6F",
    'IndicatorError'            : "#EB594D",
    'IndicatorWarning'          : "#2B610C",
    'HighlightColor'            : "#5DBA6F",
    'IconColor'                 : "#D3DAE3",
    'PanelBackground'           : "#3E424D",
    'AlternativeHighlight'      : "#3DA6B9",
    'EnableButtonSelected'      : "#4AA614",
    'DisableButtonSelected'     : "#D11616",
    'EnableButtonUnselected'    : "#002F00",
    'DisableButtonUnselected'   : "#2B610C",
    'CPUProgress'               : "#FFA726",
}

//
// Global-wide animation speeds
//
var slowAnimation = 200
var fastAnimation = 100

//
// Global-wide layout spacing value
//
var spacing = scale (10)

//
// Used in all text-based status indicators (such as voltage indicator)
//
var invalidStr = "--.--"

//
// Specified the UI and monospace fonts used by the application
//
var uiFont = "Ubuntu"
var monoFont = "Ubuntu Mono"

//
// Returns the adjusted input for the screen size and density
//
function scale (input) {
    return input * cRatio;
}

//
// A very fast beep, some speakers may have trouble reproducing this
//
function fastBeep() {
    beep (220, 50)
}

//
// A medium-length beep, used when clicking/interacting with UI elements (such as buttons)
//
function normalBeep() {
    beep (220, 100)
}

//
// Only used to get the user's attention (e.g. when we cannot enable the robot)
//
function longBeep() {
    beep (220, 200)
}

//
// Beep with a custom frequency (in HZ) and time (in milliseconds)
//
function beep (frequency, time) {
    cBeeper.beep (frequency, time)
}

//
// Parses the input text (already formatted in morse) and generates
// the appropiate sounds. This is used in the status widget to generate
// several sound tones.
//
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
