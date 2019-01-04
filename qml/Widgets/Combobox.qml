import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

import "../Globals.js" as Globals

ComboBox {
    id: combo

    //
    // Moves the current label to the right if set to true
    //
    property bool alignRight: false

    //
    // We can do a combo-box from scratch, but I'm lazy
    //
    style: ComboBoxStyle {
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

        label: Text {
            id: _label
            smooth: true
            text: control.currentText
            font.family: Globals.uiFont
            color: Globals.Colors.Foreground
            font.pixelSize: Globals.scale (12)
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: alignRight ? Text.AlignRight : Text.AlignLeft

            anchors {
                verticalCenter: parent.verticalCenter
                left: alignRight ? undefined: parent.left
                right: alignRight ? parent.right : undefined
                margins: alignRight ? Globals.spacing + Globals.scale(12) : 0
            }
        }
    }
}
