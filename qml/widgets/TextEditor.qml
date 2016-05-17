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
    // Gives us direct access to the text editor
    //
    property alias editor: edit

    //
    // Gives us access to the text of the text editor
    //
    property alias text: edit.text

    //
    // Define the colors of the control
    //
    property string foregroundColor: height > Globals.scale (140) ? Globals.Colors.WidgetForeground :
                                                                    Globals.Colors.TextAreaForeground
    property string backgroundColor: height > Globals.scale (140) ? Globals.Colors.WidgetBackground :
                                                                    Globals.Colors.TextAreaBackground

    //
    // Define a standard size for the widget
    //
    width: Globals.scale (250)
    height: Globals.scale (120)

    //
    // The widget drawing (e.g. background, border, etc)
    //
    Rectangle {
        anchors.fill: parent
        color: backgroundColor
        border.width: Globals.scale (1)
        opacity: parent.enabled ? 1 :0.5
        border.color: Globals.Colors.WidgetBorder
    }

    //
    // The actual text editor
    //
    TextEdit {
        id: edit
        anchors.fill: parent
        color: foregroundColor
        readOnly: !parent.enabled
        font.family: Globals.uiFont
        font.pixelSize: Globals.scale (12)
        anchors.margins: Globals.scale (5)
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        selectionColor: Globals.Colors.HighlightColor
    }
}
