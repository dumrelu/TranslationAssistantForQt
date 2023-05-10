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
        visible: width > 0

        // TODO: Use a StackView
        Page {
            id: background
            anchors.fill: parent
        }

        ColumnLayout {
            anchors.fill: parent

            Label {
                Layout.fillWidth: true
                Layout.preferredHeight: 30
                
                elide: Text.ElideRight
                text: qsTr("Selected text item") + ": " + TranslationAssistant.selectedTranslationText
            }

            TranslationListView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                model: !TranslationAssistant.selectedTranslationText ? TranslationAssistant : TranslationAssistant.verifiedTranslationsModel
            }
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
