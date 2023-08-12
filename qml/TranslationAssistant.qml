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
        width: parent.width / 2

        ListView {
            id: listView
            anchors.fill: parent
            model: TranslationAssistant
            delegate: ItemDelegate {
                width: parent.width
                height: 50
                text: model.source + ", " + model.translation + ", " + model.isFinished

                onClicked: {
                    model.translation = "Changed"
                    model.isFinished = true
                }
            }
        }
    }
}
