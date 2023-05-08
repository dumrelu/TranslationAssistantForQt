import QtQuick 2.8
import QtQuick.Controls 2.1
import TranslationAssistant 1.0

Item {
    id: root

    anchors.fill: parent

    Button {
        id: trayIndicator

        anchors.top: trayContainer.top
        anchors.topMargin: 10
        anchors.right: trayContainer.left

        width: 30
        height: 30
        opacity: 0.75

        text: trayContainer.state === "hidden" ? "<" : ">"

        onClicked: {
            trayContainer.state = trayContainer.state === "hidden" ? "visible" : "hidden"
        }
    }

    Item {
        id: trayContainer

        anchors.top: parent.top
        anchors.right: parent.right

        width: parent.width / 2.5
        height: parent.height

        // TODO: Use a StackView
        Page {
            id: background
            anchors.fill: parent
        }

        TranslationListView {
            anchors.fill: parent
            model: TranslationAssistant
        }

        state: "visible"
        states: [
            State {
                name: "visible"
            },
            State {
                name: "hidden"
                PropertyChanges {
                    target: trayContainer
                    width: 0
                }
            }
        ]

        transitions: Transition {
            from: "*"
            to: "*"

            NumberAnimation {
                properties: "width"
                duration: 500
                easing.type: Easing.InOutQuad
            }
        }
    }
}
