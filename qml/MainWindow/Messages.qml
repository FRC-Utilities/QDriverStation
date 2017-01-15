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
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.4

import "../Widgets"
import "../Globals.js" as Globals

ColumnLayout {
    spacing: Globals.spacing

    //
    // Write to the console every time the DS receives a message
    //
    Connections {
        target: DS
        onNewMessage: messages.editor.append (message)
    }

    //
    // Logs menu
    //
    Menu {
        id: logsContextMenu

        MenuItem {
            text: qsTr ("Open Application Logs")
            onTriggered: DSLogger.openLogsPath()
        }

        MenuItem {
            text: qsTr ("Open Current Log")
            onTriggered: DSLogger.openCurrentLog()
        }
    }

    //
    // Draw the action buttons
    //
    RowLayout {
        Layout.fillWidth: true
        spacing: Globals.scale (-1)

        Button {
            text: qsTr ("Logs") + "..."
            onClicked: logsContextMenu.popup()
        }

        Item {
            width: Globals.spacing
            height: Globals.spacing
        }

        Button {
            icon: icons.fa_copy
            width: Globals.scale (48)
            height: Globals.scale (24)
            iconSize: Globals.scale (12)

            onClicked: {
                messages.copy()
                messages.editor.append ("<font color=#888>** <font color=#AAA> "
                                        + qsTr ("Information")
                                        + ":</font> "
                                        + qsTr ("Console output copied to clipboard")
                                        + "</font>")
            }
        }

        Button {
            icon: icons.fa_trash
            width: Globals.scale (48)
            height: Globals.scale (24)
            iconSize: Globals.scale (12)
            onClicked: messages.text = ""
        }
    }

    //
    // Draw the console
    //
    TextEditor {
        id: messages
        editor.readOnly: true
        Layout.fillWidth: true
        Layout.fillHeight: true
        editor.textFormat: Text.RichText
        editor.wrapMode: TextEdit.WordWrap
        editor.font.family: Globals.monoFont
        editor.font.pixelSize: Globals.scale (13)
        foregroundColor: Globals.Colors.WidgetForeground
        backgroundColor: Globals.Colors.WindowBackground
    }
}
