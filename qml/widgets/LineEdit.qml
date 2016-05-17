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
    // Gives direct access to the line edit
    //
    property alias editor: input

    //
    // Holds the text of the line edit
    //
    property alias text: input.text

    //
    // Holds the placeholder text of the control, which is
    // shown when the text is empty
    //
    property string placeholder

    //
    // The colors of the base rectangle and the text
    //
    property string foregroundColor: Globals.Colors.TextAreaForeground
    property string backgroundColor: Globals.Colors.TextAreaBackground

    //
    // Control size
    //
    height: Globals.scale (24)
    implicitWidth: Globals.scale (128)
    implicitHeight: Globals.scale (24)

    //
    // Base rectangle of the line edit
    //
    Rectangle {
        anchors.fill: parent
        color: backgroundColor
        border.width: Globals.scale (1)
        opacity: parent.enabled ? 1 :0.5
        border.color: Globals.Colors.WidgetBorder
    }

    //
    // Placeholder text edit
    //
    TextInput {
        opacity: 0.5
        readOnly: true
        text: placeholder
        anchors.fill: parent
        color: foregroundColor
        visible: input.text === ""
        font.family: Globals.uiFont
        font.pixelSize: Globals.scale (12)
        anchors.margins: Globals.scale (5)
    }

    //
    // The actual line edit
    //
    TextInput {
        id: input
        anchors.fill: parent
        color: foregroundColor
        readOnly: !parent.enabled
        font.family: Globals.uiFont
        font.pixelSize: Globals.scale (12)
        anchors.margins: Globals.scale (5)
        selectionColor: Globals.Colors.HighlightColor
    }

    //
    // Beep when the text is changed
    //
    onTextChanged: enabled ? Globals.normalBeep() : undefined
}
