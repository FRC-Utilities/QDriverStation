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

    property bool enabled: true
    property bool colorfull: false
    property alias text: label.text
    property alias icon: icon.name
    property alias radius: base.radius
    property string background: enabled ? (colorfull ? "#286" : "#ccc") : "#888"
    property string foreground: enabled ? (colorfull ? "#fff" : "#3a3a3a") : "#444"

    signal clicked

    //--------------------------------------------------------------------------
    // Properties
    //--------------------------------------------------------------------------

    id: button
    width: q_app.scale (128)
    height: q_app.scale (32)

    //--------------------------------------------------------------------------
    // Behaviors
    //--------------------------------------------------------------------------

    Behavior on opacity { NumberAnimation{} }

    onOpacityChanged: {
        focus = opacity > 0
        enabled = opacity > 0
    }

    //--------------------------------------------------------------------------
    // Objects
    //--------------------------------------------------------------------------

    Rectangle {
        id: base
        radius: width / 12
        anchors.fill: parent

        color: {            
            if (!button.enabled)
                return "#888"

            else if (button.enabled) {
                if (mouseArea.containsMouse)
                    return Qt.lighter (button.background, 1.2)
            }

            return button.background
        }

        Behavior on color { ColorAnimation{} }
    }

    Row {
        anchors.centerIn: parent
        spacing: q_app.scale (4)

        Icon {
            id: icon
            color: button.foreground
            iconSize: q_app.scale (18)
            anchors.verticalCenter: parent.verticalCenter
        }

        Label {
            id: label
            fontSize: "medium"
            color: button.foreground
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        enabled: parent.enabled
        hoverEnabled: parent.enabled
        onClicked: button.enabled ? button.clicked() : undefined
    }
}
