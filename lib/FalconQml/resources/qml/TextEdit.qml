/*
 * Copyright (c) 2015 Alex Spataru <alex_spataru@outlook.com>
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
import QtQuick.Controls.Styles 1.0

TextEdit {

    //--------------------------------------------------------------------------
    // Custom Properties
    //--------------------------------------------------------------------------

    property string fontSize: "medium"

    //--------------------------------------------------------------------------
    // Properties
    //--------------------------------------------------------------------------

    color: "#222"
    selectedTextColor: "#fff"
    opacity: enabled ? 1 : 0.5
    font.family: q_global.font
    textFormat: Text.PlainText
    font.pixelSize: q_app.getSize (fontSize)
    selectionColor: enabled ? "#286" : "#888"
    wrapMode: TextEdit.WrapAtWordBoundaryOrAnywhere

    //--------------------------------------------------------------------------
    // Objects
    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------
    // Behaviors
    //--------------------------------------------------------------------------

    Behavior on color { ColorAnimation{} }
    Behavior on opacity { NumberAnimation{} }
    Behavior on selectionColor { ColorAnimation{} }

    onOpacityChanged: {
        focus = opacity > 0
        enabled = opacity > 0
    }
}
