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

        ColumnLayout {
            anchors.fill: parent

            Label {
                id: titleLabel
                
                Layout.fillWidth: true

                font.pixelSize: Qt.application.font.pixelSize * 1.3
                font.bold: true
                
                text: stackView.currentItem ? stackView.currentItem.title : "N/A"
            }

            // Back button
            Button {
                id: backButton

                Layout.fillWidth: true

                enabled: stackView.depth > 1

                text: qsTr("Back")
                font.pixelSize: Qt.application.font.pixelSize * 1.3

                onClicked: {
                    stackView.pop();
                }
            }

            StackView {
                id: stackView
                
                Layout.fillWidth: true
                Layout.fillHeight: true

                clip: true

                initialItem: TranslationListViewPage {}
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

    RowLayout {

        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.right: parent.right
        anchors.rightMargin: 10
        
        RoundButton {
            id: pendingChanges

            text: qsTr("Pending changes")
            font.pixelSize: Qt.application.font.pixelSize * 2

            onClicked: {
                if(stackView.currentItem && !stackView.currentItem.isPendingChangesPage)
                {
                    stackView.push("PendingChangesPage.qml");
                }
            }
        }

        RoundButton {
            id: saveButton

            text: qsTr("Save")
            font.pixelSize: Qt.application.font.pixelSize * 2

            onClicked: {
                console.log("Saving translations...");
            }
        }
    }

    Connections {
        target: TranslationAssistant

        function onSelectedTextChanged() {
            if(TranslationAssistant.selectedText.length > 0) {
                stackView.push("TranslationsForSelectedTextPage.qml", { "stackView": stackView });
            }
        }
    }
}
