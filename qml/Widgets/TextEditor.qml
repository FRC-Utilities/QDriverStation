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
    // If set to true, the text will automatically scroll down when
    // its changed (like a terminal emulator does)
    //
    property bool autoscroll: true

    //
    // Define the colors of the control
    //
    property string foregroundColor: height > Globals.scale (140) ? Globals.Colors.WidgetForeground :
                                                                    Globals.Colors.TextAreaForeground
    property string backgroundColor: height > Globals.scale (140) ? Globals.Colors.WidgetBackground :
                                                                    Globals.Colors.TextAreaBackground

    //
    // Copies the text to the system clipboard
    //
    function copy() {
        /* Get current text */
        var html = editor.getFormattedText (0, editor.length)

        /* Remove HTML codes */
        html = html.replace(/<style([\s\S]*?)<\/style>/gi, '');
        html = html.replace(/<script([\s\S]*?)<\/script>/gi, '');
        html = html.replace(/<\/div>/ig, '\n');
        html = html.replace(/<\/li>/ig, '\n');
        html = html.replace(/<li>/ig, '  *  ');
        html = html.replace(/<\/ul>/ig, '\n');
        html = html.replace(/<\/p>/ig, '\n');
        html = html.replace(/<br\s*[\/]?>/gi, "\n");
        html = html.replace(/<[^>]+>/ig, '');

        /* Copy result to system clipboard */
        Utilities.copy (html)
    }

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

        Behavior on opacity {
            NumberAnimation {
                duration: Globals.slowAnimation
            }
        }
    }

    //
    // A flickable item allows us to implement scrolling for
    // the text editor.
    //
    Flickable {
        id: flick
        clip: true
        interactive: true
        contentWidth: parent.width
        contentHeight: edit.contentHeight
        flickableDirection: Flickable.VerticalFlick

        anchors {
            fill: parent
            margins: Globals.spacing
        }

        //
        // This code was written by a Russian guy, original post:
        // Link: http://www.cyberforum.ru/qt/thread578187.html
        //
        function ensureVisible (r) {
            if (autoscroll) {
                if (contentX >= r.x)
                    contentX = r.x;
                else if (contentX + width <= r.x + r.width)
                    contentX = r.x + r.width - width;
                if (contentY >= r.y)
                    contentY = r.y;
                else if (contentY + height <= r.y + r.height)
                    contentY = r.y + r.height - height;
            }
        }

        //
        // The actual text editor
        //
        TextEdit {
            id: edit
            selectByMouse: true
            color: foregroundColor
            readOnly: !parent.enabled
            font.family: Globals.uiFont
            font.pixelSize: Globals.scale (12)
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            selectionColor: Globals.Colors.HighlightColor
            onCursorRectangleChanged: flick.ensureVisible (cursorRectangle)

            anchors {
                left: parent.left
                right: parent.right
                margins: Globals.scale (5)
            }
        }
    }

    //
    // The scrollbar
    //
    Scrollbar {
        id: scroll
        mouseArea: mouse
        scrollArea: flick
        height: parent.height
        width: Globals.scale (8)

        anchors {
            top: parent.top
            right: parent.right
            bottom: parent.bottom
            margins: Globals.scale (6)
        }
    }

    //
    // Used to show the scrollbar when mouse is over control
    //
    MouseArea {
        id: mouse
        hoverEnabled: true
        anchors.fill: parent
        onContainsMouseChanged: scroll.showControl()
    }
}
