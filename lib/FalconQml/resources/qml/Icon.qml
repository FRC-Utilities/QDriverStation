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

Label {

    //--------------------------------------------------------------------------
    // Custom properties
    //--------------------------------------------------------------------------

    property string name: ""
    property var iconSize: q_app.scale (24)

    property var icons: {
        "": "" ,
                "folder": "",
                "book": "",
                "hdd": "",
                "volume": "",
                "pencil": "",
                "upload": "",
                "download": "",
                "question": "",
                "fa-spinner": "",
                "envelope-o": "",
                "bitcoin": "",
                "bar-chart": "",
                "dashboard": "",
                "grid": "",
                "google" : "",
                "fullscreen": "",
                "gamepad": "",
                "camera": "",
                "home": "",
                "terminal": "",
                "check-circle": "",
                "check-square-o": "" ,
                "circle": "",
                "exclamation-triangle": "",
                "calendar": "",
                "github": "",
                "file": "",
                "clock": "",
                "bookmark-o": "",
                "user": "",
                "comments-o": "",
                "check": "",
                "ellipse-h": "" ,
                "ellipse-v": "",
                "save": "",
                "smile-o": "",
                "spinner": "",
                "square-o": "",
                "times": "",
                "times-circle": "",
                "plus": "",
                "bell-o": "",
                "bell": "",
                "chevron-left": "",
                "chevron-right": "",
                "chevron-down": "",
                "cog": "",
                "minus": "",
                "dashboard": "",
                "calendar-empty": "",
                "calendar": "",
                "bars":"",
                "inbox": "",
                "list": "",
                "long-list": "",
                "comment": "",
                "tasks": "",
                "bug": "",
                "code-fork": "",
                "clock-o": "",
                "lightbulb": "",
                "comments": "",
                "pencil-square-o":"",
                "check-square-o":"",
                "picture-o":"",
                "trash": "",
                "code": "",
                "users": "",
                "exchange": "",
                "link": "",
                "settings": "",
                "about": "",
                "clip": "",
                "globe": "",
                "help" : "",
                "heart": "",
                "send": "",
                "refresh": "",
                "confirm": "",
                "cancel": "",
                "save": "",
                "exclamation": "",
                "search": ""
    }

    //--------------------------------------------------------------------------
    // Properties
    //--------------------------------------------------------------------------

    text: icons[name]
    font.pixelSize: iconSize
    font.family: "FontAwesome"
    verticalAlignment: Text.AlignVCenter
    horizontalAlignment: Text.AlignHCenter

    //--------------------------------------------------------------------------
    // Behaviors
    //--------------------------------------------------------------------------

    Behavior on color { ColorAnimation{} }
    Behavior on opacity { NumberAnimation{} }

}

