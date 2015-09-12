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

import "."
import QtQuick 2.0

Item {

    //--------------------------------------------------------------------------
    // Custom properties
    //--------------------------------------------------------------------------

    property alias text: label.text

    //--------------------------------------------------------------------------
    // Functions
    //--------------------------------------------------------------------------

    function showToolbar() {
        opacity = 1
    }

    function hideToobar() {
        opacity = 0
    }


    //--------------------------------------------------------------------------
    // Properties
    //--------------------------------------------------------------------------

    height: q_app.scale (56)

    anchors {
        top: parent.top
        left: parent.left
        right: parent.right
    }

    Behavior on opacity { NumberAnimation{} }

    //--------------------------------------------------------------------------
    // Objects
    //--------------------------------------------------------------------------

    Rectangle {
        id: base
        color: "#222"
        anchors.fill: parent
    }

    Rectangle {
        height: q_app.scale (3)
        color: Qt.lighter (base.color, 1.05)

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
    }

    Rectangle {
        height: q_app.scale (1)
        color: Qt.lighter (base.color, 1.1)

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
    }

    Rectangle {
        height: q_app.scale (4)
        color: Qt.darker (base.color, 1.4)

        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
    }

    Rectangle {
        color: "#aaaaaa"
        height: q_app.scale (3)
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
    }

    Rectangle {
        color: "#cccccc"
        height: q_app.scale (2)
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
    }

    Rectangle {
        color: "#eeeeee"
        height: q_app.scale (1)
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
    }

    Label {
        id: label
        color: "#fff"
        centered: true
        font.bold: true
        font.pixelSize: q_app.scale (16)

        anchors {
            verticalCenter: parent.verticalCenter
        }
    }

    Icon {
        name: "bars"
        color: "#fff"
        iconSize: q_app.scale (24)

        anchors {
            left: parent.left
            leftMargin: q_app.scale (12)
            verticalCenter: parent.verticalCenter
        }
    }
}
