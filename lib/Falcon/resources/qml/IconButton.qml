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

Item {

    //--------------------------------------------------------------------------
    // Custom properties
    //--------------------------------------------------------------------------

    property string text
    property string name
    property bool enabled: true
    property bool selected: false
    property string color: enabled ? (selected ? "#064" : "#222") : "#111"

    signal clicked

    //--------------------------------------------------------------------------
    // Properties
    //--------------------------------------------------------------------------

    id: button
    width: q_app.scale (30)
    height: q_app.scale (30)

    //--------------------------------------------------------------------------
    // Behaviors
    //--------------------------------------------------------------------------

    Behavior on opacity { NumberAnimation{} }

    //--------------------------------------------------------------------------
    // Objects
    //--------------------------------------------------------------------------

    Column {
        anchors.fill: parent
        spacing: q_app.scale (4)

        Icon {
            id: icon
            name: button.name
            color: button.color
            iconSize: q_app.scale (18)
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Label {
            text: button.text
            color: button.color
            fontSize: q_app.scale (8)
            anchors.horizontalCenter: icon.horizontalCenter
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: button.enabled ? button.clicked() : undefined
    }
}
