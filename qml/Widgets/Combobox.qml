import QtQuick 2.0
import QtQuick.Controls 2.15

import "../Globals.js" as Globals

ComboBox {
    id: combo

    //
    // Moves the current label to the right if set to true
    //
    property bool alignRight: false

    //
    // Qt 6 ComboBox with simplified styling
    //
    background: Rectangle {
        border.width: Globals.scale (1)
        color: Globals.Colors.WidgetBackground
        border.color: Globals.Colors.WidgetBorder

        Icon {
            size: Globals.scale (12)
            name: icons.fa_chevron_down

            anchors {
                right: parent.right
                margins: Globals.spacing
                verticalCenter: parent.verticalCenter
            }
        }
    }

    contentItem: Text {
        leftPadding: alignRight ? 0 : Globals.spacing
        rightPadding: alignRight ? Globals.spacing + Globals.scale(12) : 0
        smooth: true
        text: combo.displayText
        font.family: Globals.uiFont
        color: Globals.Colors.Foreground
        font.pixelSize: Globals.scale (12)
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: alignRight ? Text.AlignRight : Text.AlignLeft
        elide: Text.ElideRight
    }
}
