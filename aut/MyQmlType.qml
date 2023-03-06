import QtQuick 2.12

Item {
    id: root

    anchors.fill: parent

    Text {
        objectName: "text2"
        anchors.centerIn: parent
        text: qsTr("Text2")
    }
}