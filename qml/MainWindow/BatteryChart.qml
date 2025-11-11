import QtQuick 2.0

import "../Globals.js" as Globals

Item {
    property var diodeHeight: height * 0.20
    property var backgroundColor: Globals.Colors.IconColor

    //
    // Background rectangle
    //
    Rectangle {
        id: cover
        height: Globals.scale (4)
        color: parent.backgroundColor

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            topMargin: parent.diodeHeight
        }
    }

    //
    // Left diode
    //
    Rectangle {
        width: parent.width * 0.2
        color: parent.backgroundColor
        height: parent.diodeHeight - Globals.scale (2)

        anchors {
            top: parent.top
            left: parent.left
            leftMargin: cover.height
        }
    }

    //
    // Right diode
    //
    Rectangle {
        width: parent.width * 0.2
        color: parent.backgroundColor
        height: parent.diodeHeight - Globals.scale (2)

        anchors {
            top: parent.top
            right: parent.right
            rightMargin: cover.height
        }
    }

    //
    // Graph
    //
    Rectangle {
        color: parent.backgroundColor

        anchors {
            fill: parent
            leftMargin: Globals.scale (2)
            rightMargin: Globals.scale (2)
            topMargin: parent.diodeHeight + (cover.height / 2)
        }

        VoltageGraph {
            border.color: parent.color
            Component.onCompleted: setSpeed (24)
            color: Globals.Colors.WindowBackground
            noCommsColor: Globals.Colors.WindowBackground

            anchors {
                fill: parent
                topMargin: 0
                margins: Globals.scale (2)
            }
        }
    }
}
