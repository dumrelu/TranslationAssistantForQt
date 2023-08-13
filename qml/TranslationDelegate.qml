import QtQuick 2.8
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.2
import TranslationAssistant 1.0

Rectangle {
    id: root

    property bool expanded: false
    property bool selected: false

    property real titleFontFactor: 1.5
    property real labelFontFactor: 0.85

    signal titleClicked();

    height: columnLayout.height + columnLayout.anchors.topMargin * 2
    
    clip: true
    radius: 5

    color: "transparent"
    border.width: 2
    border.color: "black"

    Behavior on height {
        NumberAnimation {
            duration: 500
        }
    }

    ColumnLayout {
        id: columnLayout

        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.right: parent.right
        anchors.rightMargin: 10

        spacing: 0
    
        RowLayout {
            id: titleBar

            Layout.fillWidth: true
            
            spacing: 20

            Label {
                id: titleLabel

                Layout.fillWidth: true

                elide: Label.ElideRight
                font.pixelSize: Qt.application.font.pixelSize * root.titleFontFactor
                text: model.translation ? model.translation : model.source

                MouseArea {
                    anchors.fill: parent

                    onClicked: {
                        root.titleClicked();
                    }
                }
            }
            
            CheckBox {
                id: translationCheckBox

                checked: model.isFinished

                onCheckedChanged: {
                    model.isFinished = checked;
                }
            }
        }

        Rectangle {
            id: separator

            Layout.fillWidth: true
            Layout.preferredHeight: 1

            color: "black"
            visible: root.expanded
        }

        Label {
            id: contextLabel

            Layout.fillWidth: true
            Layout.bottomMargin: 0

            visible: root.expanded
            elide: Label.ElideRight
            font.pixelSize: Qt.application.font.pixelSize * root.labelFontFactor

            text: qsTr("Context")
        }

        TextArea {
            id: contextTextArea

            Layout.fillWidth: true
            Layout.topMargin: 0
            
            visible: root.expanded
            readOnly: true
            wrapMode: TextArea.Wrap

            text: model.context

            Rectangle {
                anchors.bottom: parent.bottom
                width: parent.width
                height: 1

                color: "lightgray"
                visible: root.expanded
            }
        }

        Label {
            id: sourceLabel

            Layout.fillWidth: true
            Layout.bottomMargin: 0

            visible: root.expanded
            elide: Label.ElideRight
            font.pixelSize: Qt.application.font.pixelSize * root.labelFontFactor

            text: qsTr("Source text")
        }

        TextArea {
            id: sourceTextArea

            Layout.fillWidth: true
            Layout.topMargin: 0
            
            visible: root.expanded
            readOnly: true
            wrapMode: TextArea.Wrap

            text: model.source

            Rectangle {
                anchors.bottom: parent.bottom
                width: parent.width
                height: 1

                color: "lightgray"
                visible: root.expanded
            }
        }

        Label {
            id: translationLabel

            Layout.fillWidth: true
            Layout.bottomMargin: 0

            visible: root.expanded
            elide: Label.ElideRight
            font.pixelSize: Qt.application.font.pixelSize * root.labelFontFactor

            text: qsTr("Translation")
        }

        TextArea {
            id: translationTextArea

            Layout.fillWidth: true
            Layout.topMargin: 0
            
            visible: root.expanded
            wrapMode: TextArea.Wrap

            placeholderText: qsTr("Enter translation here")
            text: model.translation

            Keys.onReturnPressed: function(event) {
                if (event.modifiers & Qt.ControlModifier)
                {
                    translationTextArea.insert(translationTextArea.length, "\n");
                }
                else
                {
                    translationTextArea.editingFinished();
                }
            }

            onEditingFinished: function() {
                model.translation = translationTextArea.text;
            }
        }
    }
}