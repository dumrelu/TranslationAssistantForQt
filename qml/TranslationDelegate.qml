import QtQuick 2.8
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.2
import TranslationAssistant 1.0

ItemDelegate {
    id: root

    text: model.source

    Rectangle {
        anchors.fill: parent
        color: "white"
    }

    onClicked: {
        TranslationAssistant.highlightTranslation(model.id);
    }
}