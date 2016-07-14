import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

import "../Globals.js" as Globals

ComboBox {
    id: combo

    //
    // We can do a combo-box from scratch, but I'm lazy
    //
    style: ComboBoxStyle {
        background: Rectangle {
            border.width: Globals.scale (1)
            color: Globals.Colors.WidgetBackground
            border.color: Globals.Colors.WidgetBorder

            Icon {
                anchors.top: parent.top
                size: Globals.scale (12)
                name: icons.fa_chevron_down
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.margins: Globals.scale (5)
            }
        }

        label: Text {
            smooth: true
            text: control.currentText
            font.family: Globals.uiFont
            color: Globals.Colors.Foreground
            font.pixelSize: Globals.scale (12)
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
