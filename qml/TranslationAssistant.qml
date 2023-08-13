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

        ColumnLayout {
            id: columnLayout

            anchors.fill: parent
            anchors.margins: 10

            RowLayout {
                Button {
                    id: backButton

                    Layout.preferredWidth: 30
                    Layout.preferredHeight: 30

                    text: "<"
                    enabled: translationListView.state !== ""

                    onClicked: {
                        if(translationListView.state === "relevant_translations")
                        {
                            TranslationAssistant.clearRelevantTranslations();
                        }
                    }
                }

                Label {
                    id: title
                    
                    font.pixelSize: Qt.application.font.pixelSize * 1.5
                    font.bold: true
                    
                    text: qsTr("Translation Assistant")
                }
            }

            TranslationListView {
                id: translationListView

                Layout.fillWidth: true
                Layout.fillHeight: true

                model: TranslationAssistant.relevantTranslationsModel

                add: Transition {
                    NumberAnimation { properties: "x,y"; duration: 500; easing.type: Easing.OutBounce }
                }

                displaced: Transition {
                    NumberAnimation { properties: "x,y"; duration: 500; easing.type: Easing.OutBounce }
                }

                states: [
                    State {
                        name: "relevant_translations"
                        when: translationListView.count !== TranslationAssistant.rowCount()
                    }
                ]
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
