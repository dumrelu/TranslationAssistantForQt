import QtQuick 2.8
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.1
import TranslationAssistant 1.0

Page {
    id: root

    property StackView stackView: null

    title: qsTr("Selected text translations")

    ColumnLayout {
        anchors.fill: parent

        Button {
            Layout.fillWidth: true

            text: qsTr("Clear selection")

            onClicked: {
                TranslationAssistant.clearSelectedText();
                if(root.stackView)
                {
                    root.stackView.pop();
                }
            }
        }

        TranslationListView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            highlightSelections: false

            model: TranslationAssistant.verifiedTranslationsModel
        }
    }
}