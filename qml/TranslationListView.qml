import QtQuick 2.8
import QtQuick.Controls 2.1

ListView {
    id: root

    delegate: Rectangle {
        width: parent.width
        height: 50
        color: index % 2 ? "red" : "blue"
        Text {
            anchors.centerIn: parent
            text: index
        }
    }
}
