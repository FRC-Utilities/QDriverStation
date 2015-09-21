import QtQuick 2.0
import "qrc:/falcon/qml" as Falcon

Falcon.FalconApp {
    id: app
    width: 600
    height: 400
    title: "Falcon Example"
    Component.onCompleted: q_app.setToolbarText (title)

    Column {
        spacing: q_app.scale (12)
        anchors.centerIn: parent

        Falcon.Icon {
            name: "book"
            iconSize: q_app.scale (96)
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Falcon.Label {
            font.bold: true
            fontSize: "x-large"
            text: qsTr ("This is a title")
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Falcon.Label {
            text: qsTr ("This is a normal text")
            anchors.horizontalCenter: parent.horizontalCenter
        }

        // Just a separator...
        Item {
            width: height
            height: q_app.scale (8)
        }

        Row {
            spacing: q_app.scale (8)

            Falcon.Button {
                icon: "github"
                text: qsTr ("GitHub")
                onClicked: Qt.openUrlExternally ("http://github.com/")
            }

            Falcon.Button {
                icon: "google"
                background: "#48d"
                foreground: "white"
                text: qsTr ("Google")
                onClicked: Qt.openUrlExternally ("http://google.com/ncr")
            }

            Falcon.Button {
                icon: "cancel"
                background: "#f44"
                foreground: "white"
                text: qsTr ("Close")
                onClicked: app.close()
            }
        }
    }

    Falcon.BottomBar {
        Row {
            anchors.centerIn: parent
            spacing: parent.width / 5

            Falcon.IconButton {
                name: "home"
                text: qsTr ("Home")
                anchors.verticalCenter: parent.verticalCenter
            }

            Falcon.IconButton {
                name: "cog"
                text: qsTr ("Preferences")
                anchors.verticalCenter: parent.verticalCenter
            }

            Falcon.IconButton {
                name: "bug"
                text: qsTr ("Bugs")
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }
}
