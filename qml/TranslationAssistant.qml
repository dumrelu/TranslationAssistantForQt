import QtQuick 2.0

Rectangle {
    id: root

    anchors.fill: parent
    
    color: "transparent"

    Text {
        anchors.right: parent.right
        anchors.top: parent.top

        font.pixelSize: 20
        font.bold: true
        text: "Test"
    }
}
