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

Text {
    //
    // Holds the size of the text, this can be a custom assigned
    // value or one of the predefined sizes of this control
    //
    property int size: normal

    //
    // Predefined text sizes, use them when possible to maintain
    // UI consistency throughout the application
    //
    readonly property int small:  Globals.scale (10)
    readonly property int large:  Globals.scale (16)
    readonly property int normal: Globals.scale (12)
    readonly property int medium: Globals.scale (14)

    //
    // Ensure that the text is not pixelated or edgy
    //
    smooth: true

    //
    // Font and color properties
    //
    font.pixelSize: size
    font.family: Globals.uiFont
    color: Globals.Colors.Foreground

    //
    // Perform a simple transition animation when the color is changed
    //
    Behavior on color {
        ColorAnimation {
            duration: Globals.fastAnimation
        }
    }
}
