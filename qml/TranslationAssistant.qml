import QtQuick 2.8
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.1
import TranslationAssistant 1.0

Item {
    id: root

    anchors.fill: parent

    Rectangle {
        color: "white"
        anchors.right: parent.right
        height: parent.height
        width: parent.width / 3

        Label {
            text: qsTr("Translation Assistant")
            anchors.centerIn: parent
        }
    }
}
