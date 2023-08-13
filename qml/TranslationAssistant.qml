import QtQuick 2.8
import QtQuick.Layouts 1.2
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
        hoverEnabled: true
        opacity: hovered ? 1.0 : 0.5

        text: ">"

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
        visible: width > 0

        Rectangle {
            id: background

            anchors.fill: parent

            color: "white"
        }

        TranslationListView {
            id: translationListView

            model: TranslationAssistant

            anchors.fill: parent
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

                PropertyChanges {
                    target: trayIndicator
                    text: "<"
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
