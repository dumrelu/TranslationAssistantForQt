import QtQuick 2.12

Item {
    id: root

    anchors.fill: parent

    Text {
        id: text

        objectName: "text2"
        anchors.centerIn: parent
        text: qsTr("Text2")
    }

    Timer {
        interval: 1000
        running: true
        onTriggered: {
            text.text = qsTr("Text2_changed");
        }
    }
}